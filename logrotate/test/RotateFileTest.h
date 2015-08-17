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
#include <unistd.h>
#include <cstring>
#include <cerrno>

class RotateFileTest : public ::testing::Test {
 public:

   RotateFileTest() {
   };
 protected:


   virtual void SetUp() {
      _filename = "g3sink_rotatefile_test";
      _directory = "/tmp/";
      _filesToRemove.push_back(std::string(_directory + _filename + ".log"));
   }

   virtual void TearDown() {
      for (auto filename : _filesToRemove) {
         auto success = unlink(filename.c_str());
         if (-1 == success) {
            std::cout << "error deleting: " << filename << ": " <<  std::strerror(errno) << std::endl;
         }
      }
   }

   std::string _filename;
   std::string _directory;
   std::vector<std::string> _filesToRemove;
};
