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
#include <cstdlib>

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
      //for (auto filename : _filesToRemove) {
      //   auto success = unlink(filename.c_str());
      //   if (-1 == success) {
      //      std::cout << "error deleting: " << filename << ": " <<  std::strerror(errno) << std::endl;
      //   }
      //}

//      std::string removeTgz_1 = std::string("rm -f ") + _directory  + "g3sink_rotatefile_test*.gz";
//      system(removeTgz_1.c_str());

  //    std::string removeTgz_2 = std::string("rm -f ") + _directory  + "new_sink_name*.gz";
    //  system(removeTgz_2.c_str());
      
   }

   std::string _filename;
   std::string _directory;
   std::vector<std::string> _filesToRemove;
};
