//
// g++ test_FileLog.cpp -lg3logger -lpthread
// Sink location: snippets



#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <g3log/loglevels.hpp>
#include <iostream>
#include "RotateTestHelper.h" // borrowed from the unit tests
#include "g3sinks/LogRotate.h"
#include "g3sinks/LogRotateWithFilter.h"

std::string getLogDirectory() {
   std::string directory = "./";
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__MINGW32__)
   directory = "./";
#endif
   return directory;
}



#undef NDEBUG
int main() {

   std::cout << "setting up two sinks for logging" << std::endl;
   std::cout << "#1 LogRotate, saves logs up to X bytes, compresses the big log to gz, and starts a new log file" << std::endl;
   std::cout << "#2 LogRotateWithFilter, similar to LogRotate and filters out log LEVELS that are of less interest" << std::endl;

   std::string infoMsg = "Entries that both sinks will save ";
   std::string debugMsg = "Entries that will be saved to the non-filtered log file ";

   {
      // scope to exit the logger
      std::string rotateFileLog = "logrotate_file_test";
      std::string directory = getLogDirectory();

      using namespace g3;
      std::unique_ptr<LogWorker> logworker {LogWorker::createLogWorker()};
      g3::initializeLogging(logworker.get());

      // ------------ RotateSink
      size_t maxBytesBeforeRotatingFile = 1000;
      auto rotateSinkHandle = logworker->addSink(std::make_unique<LogRotate>(rotateFileLog, directory),
                              &LogRotate::save);
      // change the max file size so we hit it easily
      rotateSinkHandle->call(&LogRotate::setMaxLogSize, maxBytesBeforeRotatingFile).wait();

      // --------------- Filtered RotateSink --------
      // internally the filtered sink uses a normal LogRotate
      std::string filteredFileLog = "filtered_rotate_file_test";
      auto logRotatePtr = std::make_unique<LogRotate>(filteredFileLog, directory);
      logRotatePtr->setMaxLogSize(maxBytesBeforeRotatingFile);
      // for the filterd log rotate we want to ignore all DEBUG LOG calls
      std::vector<LEVELS> filterOut = {G3LOG_DEBUG};
      auto filteredRotateSinkHandle = logworker->addSink(std::make_unique<LogRotateWithFilter>(std::move(logRotatePtr), filterOut),
                                      &LogRotateWithFilter::save);


      // with gz files created very quickly it'll likely only be 1 gz file created since naming
      // is with 1s granularity. That's fine for this test example.
      for (int i = 0; i < 100; ++i) {
         LOG(INFO) << infoMsg << i ; // this will go to only the normal log
         LOG(G3LOG_DEBUG) << debugMsg << i;
      }
   } // the logger will exit, all queues are flushed and log entries are pushed to disk before it exits
   using namespace RotateTestHelper; // borrowed from the unit tests
   auto filteredContent = ReadContent("filtered_rotate_file_test.log");
   auto debugs_are_filtered_out = (false == Exists(filteredContent, debugMsg));
   CHECK(debugs_are_filtered_out);
   auto info_msg_stayed = Exists(filteredContent, infoMsg);
   CHECK(info_msg_stayed);


   // for the normal log file, both the DEBUG and INFO messages can be found
   auto logContent = ReadContent("logrotate_file_test.log");
   auto debugs_msg_stayed = Exists(logContent, debugMsg);
   CHECK(debugs_msg_stayed);
   info_msg_stayed = Exists(logContent, infoMsg);
   CHECK(info_msg_stayed);

   return 0;
}
