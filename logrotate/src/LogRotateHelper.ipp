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
#include <boost/filesystem.hpp>
#include <regex>
#include <map>
#include <vector>
#include <ctime>
#include <iostream>
#include <sstream>

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__MINGW32__)

//http://stackoverflow.com/questions/321849/strptime-equivalent-on-windows

#include <time.h>
#include <iomanip>

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

namespace {
    using  steady_time_point = std::chrono::time_point<std::chrono::steady_clock>;

    static const std::string file_name_time_formatted = "%Y%m%d-%H%M%S";

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
                    if (strptime(date.c_str(), "%Y-%m-%d-%H-%M-%S", &tm) == NULL) {
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

    /**
     * Loop through the files in the folder
     * @param dir
     * @param file_name
     */
    void expireArchives(const std::string dir, const std::string& app_name, unsigned long max_log_count) {
        std::map<long, std::string> files;
        boost::filesystem::path dir_path(dir);


        boost::filesystem::directory_iterator end_itr;
        if (!boost::filesystem::exists(dir_path)) return;

        for (boost::filesystem::directory_iterator itr(dir_path);itr != end_itr; ++itr) {
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

} // anonymous


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
    std::streamoff cur_log_size_;
    size_t flush_policy_;
    size_t flush_policy_counter_;




};

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

LogRotateHelper::LogRotateHelper(const std::string& log_prefix, const std::string& log_directory,size_t flush_policy)
    : log_file_with_path_(log_directory)
    , log_directory_(log_directory)
    , log_prefix_backup_(log_prefix)
    , outptr_(new std::ofstream)
    , steady_start_time_(std::chrono::steady_clock::now())
    , flush_policy_(flush_policy)
    , flush_policy_counter_(flush_policy)
     { 
    log_prefix_backup_ = prefixSanityFix(log_prefix);
    max_log_size_ = 524288000;
    max_archive_log_count_ = 10;    
    if (!isValidFilename(log_prefix_backup_)) {
        std::cerr << "g3log: forced abort due to illegal log prefix [" << log_prefix << "]" << std::endl;
        abort();
    }

    std::string file_name = createLogFileName(log_prefix_backup_);
    log_file_with_path_ = pathSanityFix(log_directory, file_name);
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
                fileWrite("Failed to compress log!");
                return false;
            }
            is.close();
            if (remove(log_file_with_path_.c_str()) == -1) {
                fileWrite("Failed to remove old log!");
            }
            changeLogFile(log_directory_);
            std::ostringstream ss;
            ss << "Log rotated Archived file name: " << gz_file_name.str().c_str();
            fileWrite(ss.str());
            ss.clear();
            ss.str("");
            ss << log_prefix_backup_ << ".log";
            // << ", data:" << ss.str()
            // << " count: " << max_archive_log_count_ << std::endl << std::flush;
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
    const int buffer_size = 16184;
    char buffer[buffer_size];
    FILE* input = fopen(file_name.c_str(), "rb");
    gzFile output = gzopen(gzip_file_name.c_str(), "wb");

    if (input == NULL || output == NULL) {
        return false;
    }

    size_t N;
    while ((N = fread(buffer, 1, buffer_size, input)) > 0) {
        gzwrite(output, buffer, N);
    }
    if (gzclose(output) != Z_OK);
    if (fclose(input) != 0) {
        return false;
    }

    return true;
}

std::string LogRotateHelper::logFileName() {
    return log_file_with_path_;
}

void LogRotateHelper::addLogFileHeader() {
    filestream() << header();
}
