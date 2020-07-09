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
#include <memory>



struct LogRotateHelper;

/**
* \param log_prefix is the 'name' of the binary, this give the log name 'LOG-'name'-...
* \param log_directory gives the directory to put the log files */
class LogRotate {
  public:
    LogRotate(const LogRotate&) = delete;
    LogRotate& operator=(const LogRotate&) = delete;

    LogRotate(const std::string& log_prefix, const std::string& log_directory);
    virtual ~LogRotate();


    void save(std::string logEnty);
    std::string changeLogFile(const std::string& log_directory, const std::string& new_name="");
    std::string logFileName();

    // After hitting the max, the oldest compressed log file will be deleted
    void setMaxArchiveLogCount(int max_size);
    int getMaxArchiveLogCount();
    
    void setFlushPolicy(size_t flush_policy); // 0: never (system auto flush), 1 ... N: every n times
    void flush();


    // After max_file_size_in_bytes the next log entry will trigger log 
    // compression to a gz file and log entries will start fresh
    void setMaxLogSize(int max_file_size_in_bytes);
    int getMaxLogSize();

    bool rotateLog();

  private:
    std::unique_ptr<LogRotateHelper> pimpl_;
};


