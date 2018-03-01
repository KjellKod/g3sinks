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
#include "g3sinks/LogRotate.h"
#include "g3sinks/LogRotateWithFilter.h"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <chrono>
#include "RotateTestHelper.h"
#include "g3sinks/LogRotateUtility.h"
using namespace RotateTestHelper;

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
      std::cout << logfilename << std::endl;
      logrotate.save("test");
   } // RAII flush of log
   auto name = std::string{_directory + _filename + ".log"};
   int check = access(name.c_str(), F_OK); // check that the file exists
   EXPECT_EQ(check, 0) << std::strerror(errno) << " : " << name;
}




TEST_F(RotateFileTest, ChangeLogFile) {
   std::string logfilename;
   {
      LogRotate logrotate(_filename, _directory);
      logfilename = logrotate.logFileName();
      std::cout << logfilename << std::endl;
      logrotate.save("test");
      auto name = std::string{_directory + _filename + ".log"};
      EXPECT_TRUE(DoesFileEntityExist(name));
      auto expected_newname = std::string{_directory + "some_new_file.log"};
      EXPECT_FALSE(DoesFileEntityExist(expected_newname));

      // by default, if name is not given then the default name is kept
      _filesToRemove.push_back(expected_newname);
      auto newname = logrotate.changeLogFile(_directory);
      EXPECT_NE(expected_newname, newname);
      EXPECT_EQ(logfilename, newname);


      newname = logrotate.changeLogFile(_directory, "some_new_file");
      EXPECT_EQ(expected_newname, newname);

   } // RAII flush of log

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

TEST_F(RotateFileTest, setMaxLogSizeAndRotate_ValidNewName) {

   std::string newFileName = "new_sink_name";
   LogRotate logrotate(_filename, _directory);

   logrotate.changeLogFile(_directory, newFileName);
   auto logfilename = logrotate.logFileName();
   EXPECT_EQ(_directory + newFileName + ".log", logfilename);
   _filesToRemove.push_back(logfilename);

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

TEST_F(RotateFileTest, setMaxLogSizeAndRotate_EmptyNewName) {
   LogRotate logrotate(_filename, _directory);

   logrotate.changeLogFile(_directory, "");
   auto logfilename = logrotate.logFileName();
   EXPECT_EQ(_directory + _filename + ".log", logfilename);

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


namespace {
   void RotateAndExpireOldLogs(std::string filename, std::string directory, bool changeLogFile = false) {
      LogRotate logrotate(filename, directory);

      if(changeLogFile) {
         logrotate.changeLogFile(directory, "");
      }
      auto logfilename = logrotate.logFileName();
      EXPECT_EQ(directory + filename + ".log", logfilename);

      logrotate.setMaxArchiveLogCount(3);
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
         logrotate.setMaxLogSize(static_cast<int>(gone.size()));

         // force sleep 1 s to trigger new time
         std::this_thread::sleep_for(std::chrono::seconds(1));
      }
   }
} // anonymous namespace

TEST_F(RotateFileTest, rotateAndExpireOldLogs) {
   RotateAndExpireOldLogs(_filename, _directory);
   auto app_name = _filename + ".log";
   auto allFiles = LogRotateUtility::getLogFilesInDirectory(_directory, app_name);
   EXPECT_EQ(allFiles.size(), size_t{3}) << " Failure " << ExtractContent(allFiles);

}

TEST_F(RotateFileTest, rotateAndExpireOldLogsWithoutTrailingSlashForDirectory) {
   auto filename = _filename.substr(0, _filename.size()-1); // remove the trailing '/'
   RotateAndExpireOldLogs(_filename, _directory);
   auto app_name = _filename + ".log";
   std::cout << "dire: " << _directory << std::endl;
   std::cout << "app: " << app_name << std::endl;
   auto allFiles = LogRotateUtility::getLogFilesInDirectory(_directory, app_name);
   EXPECT_EQ(allFiles.size(), size_t{3}) << " Failure " << ExtractContent(allFiles);
}


/*
TEST_F(RotateFileTest, setFlushPolicy__default__every_time) {
   LogRotate logrotate(_filename, _directory);
   auto logfilename = logrotate.logFileName();

   for (size_t i = 0; i < 10; ++i) {
      std::string msg{"message: "};
      msg +=  std::to_string(i) + "\n";
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
   ASSERT_TRUE(checkIfExist("msg3")) << "\n\tcontent:" << content;   // 3rd write flushes it + previous

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
   ASSERT_TRUE(checkIfExist("msg3")) << "\n\tcontent:" << content;   // 3rd write flushes it + previous

   logrotate.save("msg4\n");
   logrotate.flush();
   ASSERT_TRUE(checkIfExist("msg4")) << "\n\tcontent:" << content;   // 3rd write flushes it + previous
}


TEST_F(RotateFileTest, DISABLED_setMaxArchiveLogCount) {
   EXPECT_FALSE(true);
}




// test to implement (and did actually not exist in vrecan/g2log-dev or elsewhere)
// =====================
// changeLogFile
// logFileName
// setMaxArchiveLogCount
// setMaxLogSizeru
*/