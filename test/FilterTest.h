/** ==========================================================================
* 2015 by KjellKod.cc
*
* This code is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*
* PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
* ********************************************* */

#pragma once
#include <gtest/gtest.h>
#include <cstring>
#include <cerrno>
#include <cstdio>

class FilterTest : public ::testing::Test {
 public:

   FilterTest() {
   };
 protected:

   virtual void SetUp() {
      _filename = "g3sink_filter_rotatefile_test";
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__MINGW32__)
      _directory = "./";
#else
      _directory = "/tmp/";
#endif
      _filesToRemove.push_back(std::string(_directory + _filename + ".log"));
   }

   virtual void TearDown() {
      for (auto filename : _filesToRemove) {
         auto success = std::remove(filename.c_str());
         if (0 != success) {
            std::cerr << "error deleting: " << filename << ": " <<  std::strerror(errno) << std::endl;
         }
      }
   }

   std::string _filename;
   std::string _directory;
   std::vector<std::string> _filesToRemove;

};
