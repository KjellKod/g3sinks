
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

#include "g3sinks/LogRotateUtility.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <g3log/time.hpp>
#include <iomanip>
#include <ios>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

namespace fs = std::filesystem;
namespace LogRotateUtility {

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__MINGW32__)
   // http://stackoverflow.com/questions/321849/strptime-equivalent-on-windows
   char* strptime(const char* s, const char* f, struct tm* tm) {
      // Isn't the C++ standard lib nice? std::get_time is defined such that its
      // format parameters are the exact same as strptime. Of course, we have to
      // create a string stream first, and imbue it with the current C locale, and
      // we also have to make sure we return the right things if it fails, or
      // if it succeeds, but this is still far simpler an implementation than any
      // of the versions in any of the C standard libraries.
      std::istringstream input(s);
      input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
      input >> std::get_time(tm, f);
      if (input.fail()) {
         return nullptr;
      }
      return (char*) (s + input.tellg());
   }
#endif

   // check for filename validity -  filename should not be part of PATH
   bool isValidFilename(const std::string& prefix_filename) {
      std::string illegal_characters("/,|<>:#$%{}()[]\'\"^!?+* ");
      size_t pos = prefix_filename.find_first_of(illegal_characters, 0);
      if (pos != std::string::npos) {
         std::cerr << "Illegal character [" << prefix_filename.at(pos) << "] in logname prefix: "
                   << "[" << prefix_filename << "]" << std::endl;
         return false;
      } else if (prefix_filename.empty()) {
         std::cerr << "Empty filename prefix is not allowed" << std::endl;
         return false;
      }

      return true;
   }
   /// @return a corrected prefix, if needed,
   /// illegal characters are removed from @param prefix input
   std::string prefixSanityFix(std::string prefix) {
      prefix.erase(std::remove_if(prefix.begin(), prefix.end(), ::isspace), prefix.end());
      prefix.erase(std::remove(prefix.begin(), prefix.end(), '/'), prefix.end());   // '/'
      prefix.erase(std::remove(prefix.begin(), prefix.end(), '\\'), prefix.end());  // '\\'
      prefix.erase(std::remove(prefix.begin(), prefix.end(), '.'), prefix.end());   // '.'
      if (!isValidFilename(prefix)) {
         return "";
      }
      return prefix;
   }

   /// @return the file header
   std::string header() {
      std::ostringstream ss_entry;
      //  Day Month Date Time Year: is written as "%a %b %d %H:%M:%S %Y"
      //   and formatted output as : Wed Sep 19 08:28:16 2012
      auto now = std::chrono::system_clock::now();
      ss_entry << "\ng3log: created log file at: " << g3::localtime_formatted(now, "%a %b %d %H:%M:%S %Y") << "\n";
      return ss_entry.str();
   }

   /// @return result as time from the file name, return by reference also the complete date string
   long getDateFromFileName(const std::string& app_name, const std::string& file_name, std::string& r_date_string) {
      if (file_name.find(app_name) != std::string::npos) {
         using namespace std;
         const regex date_regex(".*([0-9]{4}-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{2})+\\.gz");
         smatch date_match;
         if (regex_match(file_name, date_match, date_regex)) {
            if (date_match.size() == 2) {
               r_date_string = date_match[1].str();
               struct tm tm{};
               time_t t;
               if (strptime(r_date_string.c_str(), "%Y-%m-%d-%H-%M-%S", &tm) == nullptr) {
                  return (long) 0;
               }
               t = mktime(&tm);
               if (t == -1) {
                  return (long) 0;
               }
               return (long) t;
            }
         }
      }
      return (long) 0;
   }

   std::string sanityFixPath(std::string path) {
      // Unify the delimeters,. maybe sketchy solution but it seems to work
      // on at least win7 + ubuntu. All bets are off for older windows
      std::string last_character{path.back()};
      if ("/" != last_character) {
         path += "/";
      }

      std::replace(path.begin(), path.end(), '\\', '/');

      // clean up in case of multiples
      auto contains_end = [&](std::string& in) -> bool {
         size_t size = in.size();
         if (!size) return false;
         char end = in[size - 1];
         return (end == '/' || end == ' ');
      };

      while (contains_end(path)) {
         path.erase(path.size() - 1);
      }

      if (!path.empty()) {
         path.insert(path.end(), '/');
      }

      return path;
   }

   std::string createPathToFile(std::string path, std::string file_name) {
      path.insert(path.size(), file_name);
      return path;
   }

   /**
    * Loop through the files in the folder. Delete old compressed files until max_log_count is reached
    * @param dir
    * @param file_name
    */
   void expireArchives(const std::string& dir, const std::string& app_name, unsigned long max_log_count) {
      auto compressed_files = getCompressedLogFilesInDirectory(dir, app_name);
      if (compressed_files.size() > max_log_count) {
         auto logs_to_delete = compressed_files.size() - max_log_count;
         for (const auto& p : compressed_files) {
            if (logs_to_delete <= 0) {
               break;
            }
            --logs_to_delete;  // decrement the number of files to delete, even if the file could not be deleted
            std::error_code ec_file;
            auto filename_with_path = p.second;
            if (false == fs::remove(filename_with_path, ec_file)) {
               std::cerr << " Unable to delete " << filename_with_path << " " << ec_file.message() << std::endl;
            }
         }
      }
   }
   /// return all files in the directory
   std::vector<std::string> getFilesInDirectory(const std::string& path) {
      if (!fs::exists(path)) {
         return {};
      }

      std::vector<std::string> files;
      for (const auto& entry : fs::directory_iterator(path)) {
         files.push_back(entry.path().string());
      }
      return files;
   }

   /// returns both compressed and plain text log files
   std::vector<std::string> getAllLogFilesInDirectory(const std::string& path, const std::string& app_name) {
      auto any_files = getFilesInDirectory(path);

      std::vector<std::string> files;
      for (const auto& current_file : any_files) {
         if (current_file.find(app_name) != std::string::npos) {
            files.push_back(current_file);
         }
      }
      return files;
   }

   std::map<long, std::string> getCompressedLogFilesInDirectory(const std::string& path, const std::string& app_name) {
      auto log_type_files = getAllLogFilesInDirectory(path, app_name);
      std::map<long, std::string> files;

      for (const auto& current_file : log_type_files) {
         std::string r_date_result;
         auto time = getDateFromFileName(app_name, current_file, r_date_result);
         if (time > 0) {
            files.insert(std::pair<long, std::string>(time, current_file));
         }
      }
      return files;
   }

   /// create the file name
   std::string addLogSuffix(const std::string& raw_name) {
      std::stringstream oss_name;
      oss_name << raw_name << ".log";
      return oss_name.str();
   }

   /// @return true if @param complete_file_with_path could be opened
   /// @param outstream is the file stream
   bool openLogFile(const std::string& complete_file_with_path, std::ofstream& outstream) {
      std::ios_base::openmode mode = std::ios_base::out;  // for clarity: it's really overkill since it's an ofstream
      mode |= std::ios_base::app;
      outstream.open(complete_file_with_path, mode);
      if (!outstream.is_open()) {
         std::ostringstream ss_error;
         ss_error << "FILE ERROR:  could not open log file:[" << complete_file_with_path << "]";
         ss_error << "\n\t\t std::ios_base state = " << outstream.rdstate();
         std::cerr << ss_error.str().c_str() << std::endl;
         outstream.close();
         return false;
      }
      return true;
   }

   /// create the file
   std::unique_ptr<std::ofstream> createLogFile(const std::string& file_with_full_path) {
      std::unique_ptr<std::ofstream> out(new std::ofstream);
      std::ofstream& stream(*(out.get()));
      bool success_with_open_file = openLogFile(file_with_full_path, stream);
      if (false == success_with_open_file) {
         out.release();
      }
      return out;
   }
}  // namespace LogRotateUtility
