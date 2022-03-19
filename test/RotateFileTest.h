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
#include <g3sinks/LogRotateUtility.h>
#include <vector>
#include <algorithm>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class RotateFileTest : public ::testing::Test {
 public:

   RotateFileTest() {
   };
 protected:


   virtual void SetUp() {
      _filename = "g3sink_rotatefile_test";
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__MINGW32__)
      _directory = "./";
#else
      _directory = "/tmp/not_yet_existing_g3log_directory/";
#endif
                   _filesToRemove.push_back(std::string(_directory + _filename + ".log"));
   }

   virtual void TearDown() {
      auto allFiles = LogRotateUtility::getLogFilesInDirectory(_directory, _filename + ".log");
      const int kFilePathIndex = 1;
      for (auto& p : allFiles) {
         std::string file = std::get<kFilePathIndex>(p);
         if ((std::find(_filesToRemove.begin(), _filesToRemove.end(), file) == _filesToRemove.end())) {
            _filesToRemove.push_back(_directory + std::get<kFilePathIndex>(p));
         }
      }


      const std::error_condition ok;
      for (auto filename : _filesToRemove) {
         std::error_code ec_file;
         fs::remove(filename, ec_file);
         if (ok != ec_file) {
            ADD_FAILURE() << "UNABLE to remove: " << filename << " " << ec_file.message() << std::endl;
         }
      }
      std::error_code ec_dir;
      fs::remove(_directory, ec_dir);
      if (ok != ec_dir) {
            ADD_FAILURE() << "UNABLE to remove: " << _directory << " " << ec_dir.message() << std::endl;
      }

   }

   std::string _filename;
   std::string _directory;
   std::vector<std::string> _filesToRemove;
};
