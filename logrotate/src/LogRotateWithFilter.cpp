/** ==========================================================================
* 2015 by KjellKod.cc
*
* This code is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*
* PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
* ********************************************* */

#include "g3sinks/LogRotateWithFilter.h"
#include <memory>
#include <algorithm>
#include <iostream> // to remove

// helper function to create an logging sink with filter
std::unique_ptr<LogRotateWithFilter> LogRotateWithFilter::CreateLogRotateWithFilter(std::string filename, std::string directory, std::vector<LEVELS> filter) {
    auto logRotatePtr = std::make_unique<LogRotate>(filename, directory);
    return std::make_unique<LogRotateWithFilter>(std::move(logRotatePtr), filter);
}



/// @param logToFile rotate file logging sink
/// @param removes all log entries with LEVELS in this filter
LogRotateWithFilter::LogRotateWithFilter(LogRotateUniquePtr logToFile, IgnoreLogLevelsFilter ignoreLevels)
    : _logger(std::move(logToFile))
    , _filter(std::move(ignoreLevels)) {}


LogRotateWithFilter::~LogRotateWithFilter() {}

/// @param logEntry saves log entry that are not in the filter
void LogRotateWithFilter::save(g3::LogMessageMover logEntry) {
    auto level = logEntry.get()._level;
    bool isNotInFilter_ = (_filter.end() == std::find(_filter.begin(), _filter.end(), level));

    if(isNotInFilter_) {
      _logger->save(logEntry.get().toString());
   }
}

std::string LogRotateWithFilter::changeLogFile(const std::string& log_directory) {
    return _logger->changeLogFile(log_directory);
}


/// @return the current filename
std::string LogRotateWithFilter::logFileName() {
    return _logger->logFileName();
}

/// @param max_size sets the maximum number of archived logs
/// when logging and it finally gets out of archived logs to
/// write to then it will overwrite old compressed logs
void LogRotateWithFilter::setMaxArchiveLogCount(int max_size) {
    _logger->setMaxArchiveLogCount(max_size);
}

/// @param max_file_size sets the max log size in bytes
void LogRotateWithFilter::setMaxLogSize(int max_file_size) {
    _logger->setMaxLogSize(max_file_size);
}

/**
* Flush policy: Default is every single time (i.e. policy of 1). 
*
* If the system logs A LOT then it is likely better to allow for the system to buffer and write
* all the entries at once. 
* 
* 0: System decides, potentially very long time
* 1....N: Flush logs every n entry 
*/
void LogRotateWithFilter::setFlushPolicy(size_t flush_policy){
   _logger->setFlushPolicy(flush_policy);
}

/**
* Force flush of log entries. This should normally be policed with the @ref setFlushPolicy
* but is great for unit testing and if there are special circumstances where you want to see
* the logs faster than the flush_policy
*/
void LogRotateWithFilter::flush(){
   _logger->flush();
}
