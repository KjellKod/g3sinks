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
#include <g3log/std2_make_unique.hpp>

// helper function to create an logging sink with filter
std::unique_ptr<LogRotateWithFilter> LogRotateWithFilter::CreateLogRotateWithFilter(std::string filename, std::string directory, std::vector<LEVELS> filter) {
    auto logRotatePtr = std2::make_unique<LogRotate>(filename, directory);
    auto logWithFilter = std2::make_unique<LogRotateWithFilter>(std::move(logRotatePtr), filter);
    return std::move(logWithFilter);
}



/// @param logToFile rotate file logging sink
/// @param removes all log entries with LEVELS in this filter
LogRotateWithFilter::LogRotateWithFilter(LogRotateUniquePtr logToFile, IgnoreLogLevelsFilter ignoreLevels)
    : _logger(std::move(logToFile))
    , _filter(std::move(ignoreLevels)) {}


LogRotateWithFilter::~LogRotateWithFilter() {}

/// @param logEntry saves log entry that are not in the filter
void LogRotateWithFilter::save(g3::LogMessageMover logEntry) {
    _logger->save(logEntry.get().toString());
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