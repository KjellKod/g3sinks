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

#include "g3sinks/LogRotate.h"
#include "LogRotateHelper.ipp"
#include <iostream>


/// @param log_prefix to use for the file
LogRotate::LogRotate(const std::string& log_prefix, const std::string& log_directory)    : pimpl_(new LogRotateHelper(log_prefix, log_directory))
{}


LogRotate::~LogRotate() {
    std::cerr << "\nExiting, log location: " << pimpl_->log_file_with_path_ << std::endl;
}


/// @param logEntry to write to file
void LogRotate::save(std::string logEntry) {
    pimpl_->fileWrite(logEntry);
}

/// Attempt to change the current log file to another name/location.
/// @return filename with full path if successful, else empty string
std::string LogRotate::changeLogFile(const std::string& log_directory, const std::string& new_name) {
    return pimpl_->changeLogFile(log_directory, new_name);
}

/// @return the current file name to write to
std::string LogRotate::logFileName() {
    return pimpl_->logFileName();
}

/**
 * Set the max number of archived logs.
 * @param max_size
 */
void LogRotate::setMaxArchiveLogCount(int max_size) {
    pimpl_->setMaxArchiveLogCount(max_size);
 }


int LogRotate::getMaxArchiveLogCount() {
   return pimpl_->getMaxArchiveLogCount();
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
void LogRotate::setFlushPolicy(size_t flush_policy){
	pimpl_->setFlushPolicy(flush_policy);
}

/**
* Force flush of log entries. This should normally be policed with the @ref setFlushPolicy
* but is great for unit testing and if there are special circumstances where you want to see
* the logs faster than the flush_policy
*/
void LogRotate::flush(){
	pimpl_->flush();
}



/**
 * Set the max log size in bytes.
 * @param max_file_size
 */
void LogRotate::setMaxLogSize(int max_file_size) {
    pimpl_->setMaxLogSize(max_file_size);
}

int LogRotate::getMaxLogSize() {
   return pimpl_->getMaxLogSize();
}