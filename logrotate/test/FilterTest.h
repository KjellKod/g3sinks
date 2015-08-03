/*
 * File:   FilterTest.h
 * Author: kjell
 *
 */

#pragma once
#include <gtest/gtest.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

class FilterTest : public ::testing::Test {
  public:

    FilterTest() {
    };
  protected:

    virtual void SetUp() {
        _filename = "g3sink_filter_rotatefile_test";
        _directory = "/tmp/";
    };

    virtual void TearDown() {
        auto filename = std::string(_directory + _filename + ".log");
        auto success = unlink(filename.c_str());
        if (-1 == success) {
            std::cout << "error deleting: " << filename << ": " <<  std::strerror(errno) << std::endl;
        }
    };

    std::string _filename;
    std::string _directory;
};
