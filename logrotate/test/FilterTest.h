/* 
 * File:   FilterTest.h
 * Author: kjell
 *
 */

#pragma once
#include <gtest/gtest.h>
#include <unistd.h>

class FilterTest : public ::testing::Test {
public:

   FilterTest() {
   };
protected:

   virtual void SetUp() {
      filename_ = "g3sink_filter_rotatefile_test";
      directory_ = "/tmp/";
   };

   virtual void TearDown() {
      unlink(std::string(directory_ + filename_).c_str());
   };

 std::string filename_;
 std::string directory_;
};
