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

#include <cstdio>
#include <string>
#include <memory>
#include <fstream>
#include <algorithm>
#include <future>
#include <cassert>
#include <chrono>
#include <g3log/time.hpp>
#include <zlib.h>
#include <regex>
#include <map>
#include <vector>
#include <ctime>
#include <iostream>
#include <sstream>
#include "g3sinks/LogRotateUtility.h"

using namespace LogRotateUtility;

/** The Real McCoy Background worker, while g3::LogWorker gives the
 * asynchronous API to put job in the background the LogRotateHelper
 * does the actual background thread work
 *
 * Flushing of log entries will happen according to flush policy:
 * 0 is never (system decides, and when there is a log rotation)
 * 1 ... N means every x entry (1 is every time, 2 is every other time etc)
 * Default is to flush every single time
 */
struct LogRotateHelper {
   LogRotateHelper& operator=(const LogRotateHelper&) = delete;
   LogRotateHelper(const LogRotateHelper& other) = delete;
   LogRotateHelper(const std::string& log_prefix, const std::string& log_directory, size_t flush_policy = 1);
   ~LogRotateHelper();

   void setMaxArchiveLogCount(int size);
   void setMaxLogSize(int size);
   void fileWrite(std::string message);
   void fileWriteWithoutRotate(std::string message);
   void flushPolicy();
   void setFlushPolicy(size_t flush_policy);
   void flush();

   std::string changeLogFile(const std::string& directory, const std::string& new_name = "");
   std::string logFileName();
   bool archiveLog();


   void addLogFileHeader();
   bool rotateLog();
   void setLogSizeCounter();
   bool createCompressedFile(std::string file_name, std::string gzip_file_name);
   std::ofstream& filestream() {
      return *(outptr_.get());
   }

   std::string log_file_with_path_;
   std::string log_directory_;
   std::string log_prefix_backup_;
   std::unique_ptr<std::ofstream> outptr_;
   steady_time_point steady_start_time_;
   int max_log_size_;
   int max_archive_log_count_;
   int cur_log_size_;
   size_t flush_policy_;
   size_t flush_policy_counter_;




};

LogRotateHelper::LogRotateHelper(const std::string& log_prefix, const std::string& log_directory, size_t flush_policy)
   : log_file_with_path_(log_directory)
   , log_directory_(log_directory)
   , log_prefix_backup_(log_prefix)
   , outptr_(new std::ofstream)
   , steady_start_time_(std::chrono::steady_clock::now())
   , flush_policy_(flush_policy)
   , flush_policy_counter_(flush_policy) {
   log_prefix_backup_ = prefixSanityFix(log_prefix);
   max_log_size_ = 524288000;
   max_archive_log_count_ = 10;
   if (!isValidFilename(log_prefix_backup_)) {
      std::cerr << "g3log: forced abort due to illegal log prefix [" << log_prefix << "]" << std::endl;
      abort();
   }

   std::string file_name = createLogFileName(log_prefix_backup_);
   log_file_with_path_ = log_directory + file_name;
   outptr_ = createLogFile(log_file_with_path_);
   assert((nullptr != outptr_) && "cannot open log file at startup");
   addLogFileHeader();
   setLogSizeCounter();
}

/**
 * Max number of archived logs to keep.
 * @param max_size
 */
void LogRotateHelper::setMaxArchiveLogCount(int max_size) {
   max_archive_log_count_ = max_size;
}

/**
 * Set the max file size in bytes.
 * @param max_size
 */
void LogRotateHelper::setMaxLogSize(int max_size) {
   max_log_size_ = max_size;
}

LogRotateHelper::~LogRotateHelper() {
   std::ostringstream ss_exit;
   ss_exit << "\ng3log file shutdown at: " << g3::localtime_formatted(g3::systemtime_now(), g3::internal::time_formatted) << "\n\n";
   filestream() << ss_exit.str() << std::flush;
}

void LogRotateHelper::fileWrite(std::string message) {
   rotateLog();
   fileWriteWithoutRotate(message);
}

void LogRotateHelper::fileWriteWithoutRotate(std::string message) {
   std::ofstream& out(filestream());
   out << message;
   flushPolicy();
   cur_log_size_ += message.size();
}



void LogRotateHelper::flushPolicy() {
   if (0 == flush_policy_) return;
   if (0 == --flush_policy_counter_) {
      flush();
      flush_policy_counter_ = flush_policy_;
   }
}


void LogRotateHelper::setFlushPolicy(size_t flush_policy) {
   flush();
   flush_policy_ = flush_policy;
   flush_policy_counter_ = flush_policy;
}


void LogRotateHelper::flush() {
   filestream() << std::flush;
}



std::string LogRotateHelper::changeLogFile(const std::string& directory, const std::string& new_name) {
   std::string file_name = new_name;
   if (file_name.empty()) {
      file_name = log_prefix_backup_;
   }

   std::string prospect_log = createLogFileName(directory + file_name);
   std::unique_ptr<std::ofstream> log_stream = createLogFile(prospect_log);
   if (nullptr == log_stream) {
      fileWrite("Unable to change log file. Illegal filename or busy? Unsuccessful log name was:" + prospect_log);
      return ""; // no success
   }
   log_prefix_backup_ = file_name;
   addLogFileHeader();
   setLogSizeCounter();

   std::string old_log = log_file_with_path_;
   log_file_with_path_ = prospect_log;
   outptr_ = std::move(log_stream);
   log_directory_ = directory;
   return log_file_with_path_;
}

/**
 * Rotate the logs once they have exceeded our set size.
 * @return
 */
bool LogRotateHelper::rotateLog() {
   std::ofstream& is(filestream());
   if (is.is_open()) {

      if (cur_log_size_ > max_log_size_) {
         is << std::flush;

         std::ostringstream gz_file_name;
         gz_file_name << log_file_with_path_ << ".";
         gz_file_name << g3::localtime_formatted(g3::systemtime_now(), "%Y-%m-%d-%H-%M-%S");
         gz_file_name << ".gz";
         if (!createCompressedFile(log_file_with_path_, gz_file_name.str())) {
            fileWriteWithoutRotate("Failed to compress log!");
            return false;
         }
         is.close();
         if (remove(log_file_with_path_.c_str()) == -1) {
            fileWriteWithoutRotate("Failed to remove old log!");
         }
         changeLogFile(log_directory_);
         std::ostringstream ss;
         ss << "Log rotated Archived file name: " << gz_file_name.str().c_str();
         fileWriteWithoutRotate(ss.str());
         ss.clear();
         ss.str("");
         ss << log_prefix_backup_ << ".log";
         //   << ", data:" << ss.str()
         //   << " count: " << max_archive_log_count_ << std::endl << std::flush;
         expireArchives(log_directory_, ss.str(), max_archive_log_count_);

         return true;
      }
   }
   return false;
}



/**
* Update the internal counter for the g3 log size
*/
void LogRotateHelper::setLogSizeCounter() {
   std::ofstream& is(filestream());
   is.seekp(0, std::ios::end);
   cur_log_size_ = is.tellp();
   is.seekp(0, std::ios::beg);
   flush_policy_counter_ = flush_policy_;
}


/**
 * Create a compressed file of the current log.
 * @param file_name
 * @param gzip_file_name
 * @return
 */
bool LogRotateHelper::createCompressedFile(std::string file_name, std::string gzip_file_name) {
   int buffer_size = 16184;
   char buffer[buffer_size];
   FILE* input = fopen(file_name.c_str(), "rb");
   gzFile output = gzopen(gzip_file_name.c_str(), "wb");

   if (input == NULL || output == NULL) {
      return false;
   }

   int N;
   while ((N = fread(buffer, 1, buffer_size, input)) > 0) {
      gzwrite(output, buffer, N);
   }

   bool close_ok = (gzclose(output) == Z_OK);
   close_ok = (fclose(input) == 0)  && close_ok;

   return close_ok;
}

std::string LogRotateHelper::logFileName() {
   return log_file_with_path_;
}

void LogRotateHelper::addLogFileHeader() {
   filestream() << header();
}
