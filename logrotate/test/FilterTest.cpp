/** ==========================================================================
* 2015 by KjellKod.cc
*
* This code is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*
* PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
* ********************************************* */

#include "FilterTest.h"
#include "g3sinks/LogRotateWithFilter.h"
#include <iostream>
#include <cerrno>

#include <g3log/std2_make_unique.hpp>
#include "RotateTestHelper.h"

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__MINGW32__)
#define F_OK 0
#endif

#if (defined(CHANGE_G3LOG_DEBUG_TO_DBUG))
#define DEBUG DBUG
#endif

using namespace RotateTestHelper;

namespace { // anonymous
    g3::LogMessageMover CreateLogEntry(const LEVELS level, std::string content, std::string file, int line, std::string function) {
        auto message = g3::LogMessage(file, line, function, level);
        message.write().append(content);
        return g3::MoveOnCopy<g3::LogMessage>(std::move(message));
    }


#define CREATE_LOG_ENTRY(level, content) CreateLogEntry(level, content, __FILE__, __LINE__, __FUNCTION__)


} // anonymous

TEST_F(FilterTest, CreateObject) {
    std::string logfilename;
    {
        auto logRotatePtr = std2::make_unique<LogRotate>(_filename, _directory);

        LogRotateWithFilter logWithFilter(std::move(logRotatePtr), {});
    } // RAII flush of log
    auto name = std::string{_directory + _filename + ".log"};
    int check = access(name.c_str(), F_OK); // check that the file exists
    EXPECT_EQ(check, 0) << std::strerror(errno) << " : " << name;
}



TEST_F(FilterTest, CreateObjectUsingHelper) {
    auto name = std::string{_directory + _filename + ".log"};
    int check = access(name.c_str(), F_OK); // check that the file exists
    EXPECT_NE(check, 0) << std::strerror(errno) << " : " << name;

    {
        auto filterSinkPtr = LogRotateWithFilter::CreateLogRotateWithFilter(_filename, _directory, {});
    } // raii
    check = access(name.c_str(), F_OK); // check that the file exists
    EXPECT_EQ(check, 0) << std::strerror(errno) << " : " << name;
    auto content = ReadContent(name);
    EXPECT_TRUE(Exists(content, "g3log: created log file at:"));
    EXPECT_TRUE(Exists(content, "file shutdown at:"));
}


TEST_F(FilterTest, NothingFiltered) {
    {
        auto filterSinkPtr = LogRotateWithFilter::CreateLogRotateWithFilter(_filename, _directory, {});
        auto message0 = CREATE_LOG_ENTRY(INFO, "Hello World");
        filterSinkPtr->save(message0);

        auto message1 = std::move(CREATE_LOG_ENTRY(DEBUG, "Hello D World"));
        filterSinkPtr->save(message1);

        auto message2 = std::move(CREATE_LOG_ENTRY(WARNING, "Hello W World"));
        filterSinkPtr->save(message2);

        auto message3 = std::move(CREATE_LOG_ENTRY(FATAL, "Hello F World"));
        filterSinkPtr->save(message3);

    } // raii

    auto name = std::string{_directory + _filename + ".log"};
    auto content = ReadContent(name);
    EXPECT_TRUE(Exists(content, "Hello World")) << content;
    EXPECT_TRUE(Exists(content, "Hello D World")) << content;
    EXPECT_TRUE(Exists(content, "Hello W World")) << content;
    EXPECT_TRUE(Exists(content, "Hello F World")) << content;
}

TEST_F(FilterTest, FilteredAndNotFiltered) {
    {
        auto filterSinkPtr = LogRotateWithFilter::CreateLogRotateWithFilter(_filename, _directory, {DEBUG, INFO, WARNING, FATAL});
        auto message0 = CREATE_LOG_ENTRY(INFO, "Hello World");
        filterSinkPtr->save(message0);
        
        auto message1 = std::move(CREATE_LOG_ENTRY(DEBUG, "Hello D World"));
        filterSinkPtr->save(message1);

        auto message2 = std::move(CREATE_LOG_ENTRY(WARNING, "Hello W World"));
        filterSinkPtr->save(message2);

        auto message3 = std::move(CREATE_LOG_ENTRY(FATAL, "Hello F World"));
        filterSinkPtr->save(message3);

        auto newLevel = LEVELS{123, "MadeUpLevel"};
        auto message4 = std::move(CREATE_LOG_ENTRY(newLevel, "Hello New World"));
        filterSinkPtr->save(message4);

    } // raii

    auto name = std::string{_directory + _filename + ".log"};
    auto content = ReadContent(name);
    EXPECT_FALSE(Exists(content, "Hello World")) << content;
    EXPECT_FALSE(Exists(content, "Hello D World")) << content;
    EXPECT_FALSE(Exists(content, "Hello W World")) << content;
    EXPECT_FALSE(Exists(content, "Hello F World")) << content;

    // Not filtered will exist
    EXPECT_TRUE(Exists(content, "Hello New World")) << content;

    const auto level1 = INFO;
    const auto level2 = INFO;
    EXPECT_TRUE(level1 == level2);
}


TEST_F(FilterTest, setFlushPolicy__default__every_time) {
   auto filterSinkPtr = LogRotateWithFilter::CreateLogRotateWithFilter(_filename, _directory, {});
   auto logfilename = filterSinkPtr->logFileName();

   for (size_t i = 0; i < 10; ++i) {
      std::string msg{"message: "};
      msg +=  std::to_string(i) + "\n";
      filterSinkPtr->save(CREATE_LOG_ENTRY(INFO, msg));
      auto content = ReadContent(logfilename);
      auto exists = Exists(content, msg);
      ASSERT_TRUE(exists) << "\n\tcontent:" << content << "-\n\tentry: " << msg;
   }
}

TEST_F(FilterTest, setFlushPolicy__only_when_buffer_is_full) {
   auto filterSinkPtr = LogRotateWithFilter::CreateLogRotateWithFilter(_filename, _directory, {});
   auto logfilename = filterSinkPtr->logFileName();
   filterSinkPtr->setFlushPolicy(0);

   // auto buffer size if by default 1024
   for(int i = 0; i < 10; ++i) {
      filterSinkPtr->save(CREATE_LOG_ENTRY(INFO, "this is a messagen\n"));
   }

   auto content = ReadContent(logfilename);
   auto exists = Exists(content, "this is a message");
   ASSERT_FALSE(exists) << "\n\tcontent:" << content << "-\n\tentry: " << "Y" << ", content.size(): " << content.size();
}

TEST_F(FilterTest, setFlushPolicy__every_third_write) {
   auto filterSinkPtr = LogRotateWithFilter::CreateLogRotateWithFilter(_filename, _directory, {});
   auto logfilename = filterSinkPtr->logFileName();
   filterSinkPtr->setFlushPolicy(3);

   std::string content;
   auto checkIfExist = [&](std::string expected) -> bool {
     content = ReadContent(logfilename);
     bool exists = Exists(content, expected);
     return exists;
   };

   // auto buffer size if by default 1024
   filterSinkPtr->save(CREATE_LOG_ENTRY(INFO, "msg1\n"));
   ASSERT_FALSE(checkIfExist("msg1")) << "\n\tcontent:" << content;

   filterSinkPtr->save(CREATE_LOG_ENTRY(INFO,"msg2\n"));
   ASSERT_FALSE(checkIfExist("msg2")) << "\n\tcontent:" << content;
   
   filterSinkPtr->save(CREATE_LOG_ENTRY(INFO,"msg3\n"));
   ASSERT_TRUE(checkIfExist("msg3")) << "\n\tcontent:" << content;   // 3rd write flushes it + previous

   filterSinkPtr->save(CREATE_LOG_ENTRY(INFO, "msg4\n"));
   ASSERT_FALSE(checkIfExist("msg4")) << "\n\tcontent:" << content;
}


TEST_F(FilterTest, setFlushPolicy__force_flush) {
   auto filterSinkPtr = LogRotateWithFilter::CreateLogRotateWithFilter(_filename, _directory, {});
   auto logfilename = filterSinkPtr->logFileName();
   filterSinkPtr->setFlushPolicy(100);

   std::string content;
   auto checkIfExist = [&](std::string expected) -> bool {
     content = ReadContent(logfilename);
     bool exists = Exists(content, expected);
     return exists;
   };

   // auto buffer size if by default 1024
   filterSinkPtr->save(CREATE_LOG_ENTRY(INFO,"msg1\n"));
   ASSERT_FALSE(checkIfExist("msg1")) << "\n\tcontent:" << content;

   filterSinkPtr->save(CREATE_LOG_ENTRY(INFO, "msg2\n"));
   ASSERT_FALSE(checkIfExist("msg2")) << "\n\tcontent:" << content;
   
   filterSinkPtr->save(CREATE_LOG_ENTRY(INFO, "msg3\n"));
   filterSinkPtr->flush();
   ASSERT_TRUE(checkIfExist("msg3")) << "\n\tcontent:" << content;   // 3rd write flushes it + previous

   filterSinkPtr->save(CREATE_LOG_ENTRY(INFO, "msg4\n"));
   filterSinkPtr->flush();
   ASSERT_TRUE(checkIfExist("msg4")) << "\n\tcontent:" << content;   // 3rd write flushes it + previous
}






