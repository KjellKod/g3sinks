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
#include <filesystem>
#include "g3sinks/LogRotateUtility.h"

namespace fs = std::filesystem;

class FilterTest : public ::testing::Test {
public:

   FilterTest() {
   };
protected:

   virtual void SetUp() {
      _filename = "g3sink_filter_rotatefile_test";
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__MINGW32__)
      _directory = "./g3log_test_directory";
#else
      _directory = "/tmp/g3log_test_directory/";
#endif
   }

   virtual void TearDown() {
      auto allFiles = LogRotateUtility::getFilesInDirectory(_directory);
      for (auto& filename : allFiles) {
         std::error_code ec_file;
         if (false == fs::remove(filename, ec_file)) {
            ADD_FAILURE() << "UNABLE to remove file: " << filename << " " << ec_file.message() << std::endl;
         }
         std::error_code ec_dir;
         if (fs::exists(_directory) && !fs::remove(_directory, ec_dir)) {
            ADD_FAILURE() << "UNABLE to remove directory: " << _directory << " " << ec_dir.message() << std::endl;
         }
      }
   }

   std::string _filename;
   std::string _directory;
};
