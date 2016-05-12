
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
#include <iostream>
#include <sstream>
#include <algorithm>
#include <g3log/time.hpp>
#include <regex>
#include <boost/filesystem.hpp>
#include <ios>
#include <fstream>
#include <iomanip>
#include <map>


namespace  LogRotateUtility {

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__MINGW32__)
   //http://stackoverflow.com/questions/321849/strptime-equivalent-on-windows
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
      return (char*)(s + input.tellg());
   }
#endif



   // check for filename validity -  filename should not be part of PATH
   bool isValidFilename(const std::string& prefix_filename) {
      std::string illegal_characters("/,|<>:#$%{}()[]\'\"^!?+* ");
      size_t pos = prefix_filename.find_first_of(illegal_characters, 0);
      if (pos != std::string::npos) {
         std::cerr << "Illegal character [" << prefix_filename.at(pos) << "] in logname prefix: " << "[" << prefix_filename << "]" << std::endl;
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
      prefix.erase(std::remove(prefix.begin(), prefix.end(), '/'), prefix.end()); // '/'
      prefix.erase(std::remove(prefix.begin(), prefix.end(), '\\'), prefix.end()); // '\\'
      prefix.erase(std::remove(prefix.begin(), prefix.end(), '.'), prefix.end()); // '.'
      if (!isValidFilename(prefix)) {
         return "";
      }
      return prefix;
   }

   /// @return the file header
   std::string header() {
      std::ostringstream ss_entry;
      //  Day Month Date Time Year: is written as "%a %b %d %H:%M:%S %Y" and formatted output as : Wed Sep 19 08:28:16 2012
      ss_entry << "\ng3log: created log file at: " << g3::localtime_formatted(g3::systemtime_now(), "%a %b %d %H:%M:%S %Y") << "\n";
      return ss_entry.str();
   }

   /// @return result as time from the file name
   bool getDateFromFileName(const std::string& app_name, const std::string& file_name, long& result) {
      if (file_name.find(app_name) != std::string::npos) {
         std::string suffix = file_name.substr(app_name.size());
         if (suffix.empty()) {
            //this is the main log file
            return false;
         }
         using namespace std;

         regex date_regex("\\.(\\d{4}-\\d{2}-\\d{2}-\\d{2}-\\d{2}-\\d{2})\\.gz");
         smatch date_match;
         if (regex_match(suffix, date_match, date_regex)) {
            if (date_match.size() == 2) {
               std::string date = date_match[1].str();
               struct tm tm;
               time_t t;
               if (strptime(date.c_str(), "%Y-%m-%d-%H-%M-%S", &tm) == nullptr) {
                  return false;
               }
               t = mktime(&tm);
               if (t == -1) {
                  return false;
               }
               result = (long) t;
               return true;
            }
         }
      }
      return false;
   }

   std::string pathSanityFix(std::string path, std::string file_name) {
      // Unify the delimeters,. maybe sketchy solution but it seems to work
      // on at least win7 + ubuntu. All bets are off for older windows
      std::replace(path.begin(), path.end(), '\\', '/');

      // clean up in case of multiples
      auto contains_end = [&](std::string & in) -> bool {
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

      path.insert(path.size(), file_name);
      return path;
   }

   /**
    * Loop through the files in the folder
    * @param dir
    * @param file_name
    */
   void expireArchives(const std::string& dir, const std::string& app_name, unsigned long max_log_count) {
      std::map<long, std::string> files;
      boost::filesystem::path dir_path(dir);


      boost::filesystem::directory_iterator end_itr;
      if (!boost::filesystem::exists(dir_path)) return;

      for (boost::filesystem::directory_iterator itr(dir_path); itr != end_itr; ++itr) {
         std::string current_file(itr->path().filename().string());
         long time = 0;
         if (getDateFromFileName(app_name, current_file, time)) {
            files.insert(std::pair<long, std::string > (time, current_file));
         }
      }

      //delete old logs.
      ptrdiff_t logs_to_delete = files.size() - max_log_count;
      if (logs_to_delete > 0) {

         for (std::map<long, std::string>::iterator it = files.begin(); it != files.end(); ++it) {
            if (logs_to_delete <= 0) {
               break;
            }

            std::string filename_with_path(pathSanityFix(dir, it->second));
            remove(filename_with_path.c_str());
            --logs_to_delete;
         }
      }
   }

    std::map<long, std::string> getLogFilesInDirectory(const std::string& dir, const std::string& app_name) {
        std::map<long, std::string> files;
        boost::filesystem::path dir_path(dir);
  
 
       boost::filesystem::directory_iterator end_itr;
       if (!boost::filesystem::exists(dir_path)) return {};
 
       for (boost::filesystem::directory_iterator itr(dir_path); itr != end_itr; ++itr) {
          std::string current_file(itr->path().filename().string());
          long time = 0;
          if (getDateFromFileName(app_name, current_file, time)) {
             files.insert(std::pair<long, std::string > (time, current_file));
          }
       }
 
       return files;
    }


   /// create the file name
   std::string createLogFileName(const std::string& verified_prefix) {
      std::stringstream oss_name;
      oss_name << verified_prefix << ".log";
      return oss_name.str();
   }


   /// @return true if @param complete_file_with_path could be opened
   /// @param outstream is the file stream
   bool openLogFile(const std::string& complete_file_with_path, std::ofstream& outstream) {
      std::ios_base::openmode mode = std::ios_base::out; // for clarity: it's really overkill since it's an ofstream
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
}