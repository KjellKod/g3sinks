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

#include <g3sinks/LogRotate.h>
#include <utility>
#include <memory>
#include <vector>
#include <g3log/loglevels.hpp>
#include <g3log/logmessage.hpp>

/**
* Wraps a LogRotate file logger. It only forwareds log LEVELS
* that are NOT in the filter
*/
class LogRotateWithFilter {
   using LogRotateUniquePtr = std::unique_ptr<LogRotate>;
   using IgnoreLogLevelsFilter = std::vector<LEVELS>;

public:

   static std::unique_ptr<LogRotateWithFilter> CreateLogRotateWithFilter(std::string filename, std::string directory, std::vector<LEVELS> filter);


   LogRotateWithFilter(LogRotateUniquePtr logToFile, IgnoreLogLevelsFilter ignoreLevels);
   virtual ~LogRotateWithFilter();

   void save(g3::LogMessageMover logEntry);
   std::string changeLogFile(const std::string& log_directory);
   std::string logFileName();
   void setMaxArchiveLogCount(int max_size);
   void setMaxLogSize(int max_file_size);
   void setFlushPolicy(size_t flush_policy); // 0: never (system auto flush), 1 ... N: every n times
   void flush();
   void overrideLogDetails(g3::LogMessage::LogDetailsFunc func);




private:
   LogRotateUniquePtr _logger;
   IgnoreLogLevelsFilter _filter;
   g3::LogMessage::LogDetailsFunc _log_details_func;


};