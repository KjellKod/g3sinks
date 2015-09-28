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
#include "RotateTestHelper.h"

using namespace RotateTestHelper;


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

TEST_F(RotateFileTest, DISABLED_setMaxArchiveLogCount) {
   EXPECT_FALSE(true);
}



// test to implement (and did actually not exist in vrecan/g2log-dev or elsewhere)
// =====================
// changeLogFile
// logFileName
// setMaxArchiveLogCount
// setMaxLogSizeru