/** ==========================================================================
* 2011 by KjellKod.cc, modified by Vrecan in https://bitbucket.org/vrecan/g2log-dev
* 2015, adopted by KjellKod for g3log at:https://github.com/KjellKod/g3sinks
*
* This code is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*
* PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
* ********************************************* */

#pragma once


#include <string>
#include <map>
#include <chrono>

namespace  LogRotateUtility {
   using  steady_time_point = std::chrono::time_point<std::chrono::steady_clock>;
   static const std::string file_name_time_formatted = "%Y%m%d-%H%M%S";

   // check for filename validity -  filename should not be part of PATH
   bool isValidFilename(const std::string& prefix_filename);

    /// @return a corrected prefix, if needed,
   /// illegal characters are removed from @param prefix input
   std::string prefixSanityFix(std::string prefix);

   /// @return the file header
   std::string header();

   /// @return result as time from the file name
   bool getDateFromFileName(const std::string& app_name, const std::string& file_name, long& result);

   /**
    * Loop through the files in the folder
    * @param dir
    * @param file_name
    */
   void expireArchives(const std::string& dir, const std::string& app_name, unsigned long max_log_count);

   /// @return all the found files in the directory that follow the expected log name pattern
   /// std::map<long: timestamp, std::string : name>
   std::map<long, std::string> getLogFilesInDirectory(const std::string& dir, const std::string& app_name);

   /// create the file name
   std::string createLogFileName(const std::string& verified_prefix);


   /// @return true if @param complete_file_with_path could be opened
   /// @param outstream is the file stream
   bool openLogFile(const std::string& complete_file_with_path, std::ofstream& outstream);

   /// create the file
   std::unique_ptr<std::ofstream> createLogFile(const std::string& file_with_full_path);

} // anonymous
