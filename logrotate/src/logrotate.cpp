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

#include "logrotate.h"
#include "logrotatehelper.ipp"


/// @param log_prefix to use for the file
LogRotate::LogRotate(const std::string& log_prefix, const std::string& log_directory,
                     std::unique_ptr<g2LogConfig> config)
    : pimpl_(new LogRotateHelper(log_prefix, log_directory))
    , log_file_with_path_(pimpl_->log_file_with_path_)
{}


LogRotate::~LogRotate() {
    pimpl_.reset();
    std::cerr << "\nExiting, log location: " << log_file_with_path_ << std::endl;
}


/// @param logEntry to write to file
void LogRotate::save(std::string logEntry) {
    pimpl_->fileWrite(logEntry);
}

/// Attempt to change the current log file to another name/location.
/// @return filename with full path if successful, else empty string
std::string LogRotate::changeLogFile(const std::string& log_directory) {
    return pimpl_->changeLogFile(log_directory);
}

/// @return the current file name to write to
std::string LogRotate::logFileName() {
    return log_file_with_path_;
}


/**
 * Set the max number of archived logs.
 * @param max_size
 */
void LogRotate::setMaxArchiveLogCount(int max_size) {
  pimpl_->setMaxArchiveLogCount(max_size);
}

/**
 * Set the max log size in bytes.
 * @param max_file_size
 */
void LogRotate::setMaxLogSize(int max_file_size) {
   pimpl_->setMaxLogSize(max_file_size);
}