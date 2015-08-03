#include "RotateFileTest.h"
#include "g3sinks/LogRotate.h"
#include "g3sinks/LogRotateWithFilter.h"
#include <iostream>
#include <cerrno>
#include <cstring>


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

// test to implement (and did actually not exist in vrecan/g2log-dev or elsewhere)
// =====================
// changeLogFile
// logFileName
// setMaxArchiveLogCount
// setMaxLogSize