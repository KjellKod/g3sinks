/** ==========================================================================
 * 2015 by KjellKod.cc
 *
 * This code is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * ============================================================================*
 * PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
 * ********************************************* */

#include "RotateFileTest.h"
#include <cerrno>
#include <chrono>
#include <cstring>
#include <iostream>
#include "RotateTestHelper.h"
#include "g3sinks/LogRotate.h"
#include "g3sinks/LogRotateUtility.h"
#include "g3sinks/LogRotateWithFilter.h"
using namespace RotateTestHelper;

namespace fs = std::filesystem;

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__MINGW32__)
   #define F_OK 0
#else
   #include <unistd.h>
#endif

TEST_F(RotateFileTest, CreateObject) {
   std::string logfilename;
   {
      LogRotate logrotate(_filename, _directory);
      logfilename = logrotate.logFileName();
      logrotate.save("test");
   }  // RAII flush of log
   auto name = std::string{_directory + _filename + ".log"};
   int check = access(name.c_str(), F_OK);  // check that the file exists
   EXPECT_EQ(check, 0) << std::strerror(errno) << " : " << name;
}

TEST_F(RotateFileTest, ChangeLogFile) {
   std::string logfilename;
   {
      LogRotate logrotate(_filename, _directory);
      logfilename = logrotate.logFileName();
      logrotate.save("test");
      auto name = std::string{_directory + _filename + ".log"};
      EXPECT_TRUE(fs::exists(name));
      auto expected_newname = std::string{_directory + "some_new_file.log"};
      EXPECT_FALSE(fs::exists(expected_newname));

      // by default, if name is not given then the default name is kept
      auto newname = logrotate.changeLogFile(_directory);
      EXPECT_NE(expected_newname, newname);
      EXPECT_EQ(logfilename, newname);

      newname = logrotate.changeLogFile(_directory, "some_new_file");
      EXPECT_EQ(expected_newname, newname);

   }  // RAII flush of log
}

TEST_F(RotateFileTest, setMaxLogSize) {
   LogRotate logrotate(_filename, _directory);
   auto logfilename = logrotate.logFileName();
   std::string gone{"Soon to be missing words"};
   logrotate.save(gone);
   logrotate.setMaxLogSize(static_cast<int>(gone.size()));
   std::string first_message_in_new_log = "first message";
   logrotate.save(first_message_in_new_log);

   auto content = ReadContent(logfilename);
   auto exists = Exists(content, gone);
   EXPECT_FALSE(exists) << "\n\tcontent:" << content << "-\n\tentry: " << gone;

   exists = Exists(content, first_message_in_new_log);
   EXPECT_TRUE(exists) << "\n\tcontent:" << content << "-\n\tentry: " << gone;
}

TEST_F(RotateFileTest, setMaxLogSizeAndRotate) {
   std::string newFileName = "new_sink_name";
   LogRotate logrotate(_filename, _directory);

   logrotate.changeLogFile(_directory, newFileName);
   auto logfilename = logrotate.logFileName();
   EXPECT_EQ(_directory + newFileName + ".log", logfilename);

   std::string gone{"Soon to be missing words"};
   logrotate.save(gone);
   logrotate.setMaxLogSize(static_cast<int>(gone.size()));

   std::string first_message_in_new_log = "first message";
   logrotate.save(first_message_in_new_log);
   EXPECT_TRUE(fs::exists(logfilename));
   auto content = ReadContent(logfilename);
   auto exists = Exists(content, gone);
   EXPECT_FALSE(exists) << "\n\tcontent:" << content << "-\n\tentry: " << gone;
   EXPECT_TRUE(fs::exists(logfilename));
   exists = Exists(content, first_message_in_new_log);
   EXPECT_TRUE(exists) << "\n\tcontent:" << content << "-\n\tentry: " << gone;
   EXPECT_TRUE(fs::exists(logfilename));
   auto allFiles = LogRotateUtility::getAllLogFilesInDirectory(_directory, newFileName + ".log");
   auto allCompressedFiles = LogRotateUtility::getCompressedLogFilesInDirectory(_directory, newFileName + ".log");
   EXPECT_EQ(allFiles.size(), 2) << FlattenToString(allFiles);
   EXPECT_EQ(allCompressedFiles.size(), 1) << FlattenToString(allCompressedFiles);
}

TEST_F(RotateFileTest, logRotate_setEmptyNewName) {
   LogRotate logrotate(_filename, _directory);
   logrotate.changeLogFile(_directory, "");
   auto logfilename = logrotate.logFileName();
   EXPECT_EQ(_directory + _filename + ".log", logfilename);
}

namespace {
   const size_t kMaxArchiveLogCount = 3;
   void RotateAndExpireOldLogs(std::string filename, std::string directory, bool changeLogFile = false) {
      LogRotate logrotate(filename, directory);

      if (changeLogFile) {
         logrotate.changeLogFile(directory, "");
      }
      auto logfilename = logrotate.logFileName();
      std::string trailing_directory_character{directory.back()};
      if (trailing_directory_character != std::string{"/"}) { // for tests where directory lack trailing "/"
         directory = directory + "/";
      }

      EXPECT_EQ(directory + filename + ".log", logfilename);
      logrotate.setMaxArchiveLogCount(kMaxArchiveLogCount);
      std::string gone{"Soon to be missing words"};
      logrotate.save(gone);
      logrotate.setMaxLogSize(static_cast<int>(gone.size()));

      for (size_t idx = 0; idx < 15; ++idx) {
         std::string first_message_in_new_log = "message #" + std::to_string(idx);
         logrotate.save(first_message_in_new_log);
         auto content = ReadContent(logfilename);
         auto exists = Exists(content, gone);
         EXPECT_FALSE(exists) << "\n\tcontent:" << content << "-\n\tentry: " << gone;
         exists = Exists(content, first_message_in_new_log);
         EXPECT_TRUE(exists) << "\n\tcontent:" << content << "-\n\tentry: " << gone;
         gone = first_message_in_new_log;
         // force sleep 1 s to trigger new time
         std::this_thread::sleep_for(std::chrono::seconds(1));
      }
   }
}  // anonymous namespace

TEST_F(RotateFileTest, rotateAndExpireOldLogs) {
   RotateAndExpireOldLogs(_filename, _directory);
   auto app_name = _filename + ".log";
   auto allFiles = LogRotateUtility::getAllLogFilesInDirectory(_directory, app_name);
   auto allCompressedFiles = LogRotateUtility::getCompressedLogFilesInDirectory(_directory, app_name);
   EXPECT_EQ(allFiles.size(), allCompressedFiles.size() + 1);
   EXPECT_EQ(allCompressedFiles.size(), kMaxArchiveLogCount) << " Failure " << FlattenToString(allFiles);
   for (auto& file : allFiles) {
      ;
      EXPECT_TRUE(fs::exists(file)) << "It should exist but it doesn't: " << file << std::endl;
   }
}

TEST_F(RotateFileTest, getDateFromFileName) {
   std::string file_name = "g3sink_rotatefile_test.log.2022-03-19-14-41-43.gz";
   std::string app_name = "g3sink_rotatefile_test";
   std::string r_date_string;
   std::string expected_date_string = "2022-03-19-14-41-43";
   EXPECT_NE((long) 0, LogRotateUtility::getDateFromFileName(app_name, file_name, r_date_string));
   EXPECT_EQ(r_date_string, expected_date_string);
}


TEST_F(RotateFileTest, rotateAndExpireOldLogsWithoutTrailingSlashForDirectory) {
   auto directory = _directory; //_directory.substr(0, _directory.size() - 1);  // remove the trailing '/'
   RotateAndExpireOldLogs(_filename, directory);
   auto app_name = _filename + ".log";
   auto allFiles = LogRotateUtility::getAllLogFilesInDirectory(directory, app_name);
   auto allCompressedFiles = LogRotateUtility::getCompressedLogFilesInDirectory(directory, app_name);

   EXPECT_NE(allFiles.size(), allCompressedFiles.size());
   EXPECT_EQ(allCompressedFiles.size(), kMaxArchiveLogCount) << " Failure " << FlattenToString(allCompressedFiles);
   for (auto& file : allFiles) {
      EXPECT_TRUE(fs::exists(file)) << "It should exist but it doesn't: " << file << std::endl;
   }
}

TEST_F(RotateFileTest, setFlushPolicy__default__every_time) {
   LogRotate logrotate(_filename, _directory);
   auto logfilename = logrotate.logFileName();

   for (size_t i = 0; i < 10; ++i) {
      std::string msg{"message: "};
      msg += std::to_string(i) + "\n";
      logrotate.save(msg);
      auto content = ReadContent(logfilename);
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__MINGW32__)
      msg.replace(msg.find("\n"), 2, "\r\n");
      auto exists = Exists(content, msg);
#else
      auto exists = Exists(content, msg);
#endif
      ASSERT_TRUE(exists) << "\n\tcontent:" << content << "-\n\tentry: " << msg;
   }
}

TEST_F(RotateFileTest, setFlushPolicy__only_when_buffer_is_full) {
   LogRotate logrotate(_filename, _directory);
   auto logfilename = logrotate.logFileName();
   logrotate.setFlushPolicy(0);

   // auto buffer size if by default 1024
   for (int i = 0; i < 10; ++i) {
      logrotate.save("this is a messagen\n");
   }

   auto content = ReadContent(logfilename);
   auto exists = Exists(content, "this is a message");
   ASSERT_FALSE(exists) << "\n\tcontent:" << content << "-\n\tentry: " << "Y" << ", content.size(): " << content.size();
}

TEST_F(RotateFileTest, setFlushPolicy__every_third_write) {
   LogRotate logrotate(_filename, _directory);
   auto logfilename = logrotate.logFileName();
   logrotate.setFlushPolicy(3);

   std::string content;
   auto checkIfExist = [&](std::string expected) -> bool {
      content = ReadContent(logfilename);
      bool exists = Exists(content, expected);
      return exists;
   };

   // auto buffer size if by default 1024
   logrotate.save("msg1\n");
   ASSERT_FALSE(checkIfExist("msg1")) << "\n\tcontent:" << content;

   logrotate.save("msg2\n");
   ASSERT_FALSE(checkIfExist("msg2")) << "\n\tcontent:" << content;

   logrotate.save("msg3\n");
   ASSERT_TRUE(checkIfExist("msg3")) << "\n\tcontent:" << content;  // 3rd write flushes it + previous

   logrotate.save("msg4\n");
   ASSERT_FALSE(checkIfExist("msg4")) << "\n\tcontent:" << content;
}

TEST_F(RotateFileTest, setFlushPolicy__force_flush) {
   LogRotate logrotate(_filename, _directory);
   auto logfilename = logrotate.logFileName();
   logrotate.setFlushPolicy(100);

   std::string content;
   auto checkIfExist = [&](std::string expected) -> bool {
      content = ReadContent(logfilename);
      bool exists = Exists(content, expected);
      return exists;
   };

   // auto buffer size if by default 1024
   logrotate.save("msg1\n");
   ASSERT_FALSE(checkIfExist("msg1")) << "\n\tcontent:" << content;

   logrotate.save("msg2\n");
   ASSERT_FALSE(checkIfExist("msg2")) << "\n\tcontent:" << content;

   logrotate.save("msg3\n");
   logrotate.flush();
   ASSERT_TRUE(checkIfExist("msg3")) << "\n\tcontent:" << content;  // 3rd write flushes it + previous

   logrotate.save("msg4\n");
   logrotate.flush();
   ASSERT_TRUE(checkIfExist("msg4")) << "\n\tcontent:" << content;  // 3rd write flushes it + previous
}



TEST_F(RotateFileTest, rotateLog) {
   LogRotate logrotate(_filename, _directory);
   std::string logfilename = logrotate.logFileName();
   std::string content;
   auto checkIfExist = [&](std::string expected) -> bool {
      content = ReadContent(logfilename);
      bool exists = Exists(content, expected);
      return exists;
   };
   logrotate.save("test1");
   ASSERT_TRUE(checkIfExist("test1")) << "\n\tcontent:" << content;
   EXPECT_TRUE(fs::exists(logfilename));
   logrotate.rotateLog();
   logrotate.save("test2");
   ASSERT_FALSE(checkIfExist("test1")) << "\n\tcontent:" << content;
   ASSERT_TRUE(checkIfExist("test2")) << "\n\tcontent:" << content;
   auto app_name = _filename + ".log";
   logrotate.rotateLog();
   logrotate.save("test3");
   auto allFiles = LogRotateUtility::getAllLogFilesInDirectory(_directory, app_name);
   auto compressedFiles = LogRotateUtility::getCompressedLogFilesInDirectory(_directory, app_name);
   EXPECT_EQ(allFiles.size(), size_t{2}) << "Failure: " << FlattenToString(allFiles);
   EXPECT_EQ(compressedFiles.size(), size_t{1}) << "Failure: " << FlattenToString(compressedFiles);
}