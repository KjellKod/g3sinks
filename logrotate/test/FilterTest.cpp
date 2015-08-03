#include "FilterTest.h"
#include "g3sinks/LogRotateWithFilter.h"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <g3log/std2_make_unique.hpp>

// ReadContent
#include <sstream>
#include <string>
#include <vector>
#include <cerrno>
#include <fstream>


namespace { // anonymous
/// Ref: StackOverflow + http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
    std::string ReadContent(const std::string filename) {
        std::ifstream readIn(filename.c_str(), std::ios::in | std::ios::binary);
        if (readIn) {
            std::shared_ptr<void> raii(nullptr, [&](void*) {readIn.close(); std::cout << "closed file: " << filename << std::endl;});

            std::string contents;
            readIn.seekg(0, std::ios::end);
            contents.resize(readIn.tellg());
            readIn.seekg(0, std::ios::beg);
            readIn.read(&contents[0], contents.size());
            return contents;
        }
        throw errno;
    }



    bool Exists(const std::string content, const std::string expected) {
        auto found = content.find(expected);
        return found != std::string::npos;
    }

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
        
    } // raii

    auto name = std::string{_directory + _filename + ".log"};
    auto content = ReadContent(name);
    std::cout << "CONTENT [\n" << content << "\n ]" << std::endl;
}







