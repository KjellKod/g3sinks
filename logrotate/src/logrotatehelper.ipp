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
#include <boost/regex.hpp>
#include <map>
#include <vector>
#include <ctime>


namespace {

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
        ss_entry << "created log file at: " << g3::localtime_formatted(g3::systemtime_now(), "%a %b %d %H:%M:%S %Y") << "\n";
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
            boost::regex date_regex("\\.(\\d{4}-\\d{2}-\\d{2}-\\d{2}-\\d{2}-\\d{2})\\.gz");
            boost::smatch date_match;
            if (boost::regex_match(suffix, date_match, date_regex)) {
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
        if (!boost::filesystem::exists(dir_path)) return;
        boost::filesystem::directory_iterator end_itr;
        for (boost::filesystem::directory_iterator itr(dir_path);
                itr != end_itr;
                ++itr) {
            std::string current_file(itr->path().filename().c_str());
            long time;
            if (getDateFromFileName(app_name, current_file, time)) {
                files.insert(std::pair<long, std::string > (time, current_file));
            }
        }
        //delete old logs.
        int logs_to_delete = files.size() - max_log_count;
        if (logs_to_delete > 0) {

            for (std::map<long, std::string>::iterator it = files.begin(); it != files.end(); ++it) {
                if (logs_to_delete <= 0) {
                    break;
                }

                std::stringstream ss;
                ss << dir.c_str() << it->second.c_str();
                remove(ss.str().c_str());
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


/** The Real McCoy Background worker, while g2LogWorker gives the
 * asynchronous API to put job in the background the LogRotateHelper
 * does the actual background thread work */
struct LogRotateHelper {
    LogRotateHelper& operator=(const LogRotateHelper&) = delete;
    LogRotateHelper(const LogRotateHelper& other) = delete;
    LogRotateHelper(const std::string& log_prefix, const std::string& log_directory);
    ~LogRotateHelper();

    void setMaxArchiveLogCount(int size);
    void setMaxLogSize(int size);
    void fileWrite(internal::LogEntry message);
    std::string changeLogFile(const std::string& directory);
    std::string logFileName();
    bool archiveLog();


    void addLogFileHeader();
    bool rotateLog();
    bool createCompressedFile(std::string file_name, std::string gzip_file_name);
    std::ofstream& filestream() {
        return *(outptr_.get());
    }

    std::string log_file_with_path_;
    std::string log_directory_;
    std::string log_prefix_backup_;
    std::unique_ptr<std::ofstream> outptr_;
    g2::steady_time_point steady_start_time_;
    int max_log_size_;
    int max_archive_log_count_;




};

LogRotateHelper::LogRotateHelper(const std::string& log_prefix, const std::string& log_directory)
    : log_file_with_path_(log_directory)
    , log_directory_(log_directory)
    , log_prefix_backup_(log_prefix)
    , bg_(kjellkod::Active::createActive())
    , outptr_(new std::ofstream)
    , steady_start_time_(std::chrono::steady_clock::now()) { // TODO: ha en timer function steadyTimer som har koll pÃ¥ start
    log_prefix_backup_ = prefixSanityFix(log_prefix);
    max_log_size_ = 524288000;
    max_archive_log_count_ = 10;
    if (!isValidFilename(log_prefix_backup_)) {
        std::cerr << "g2log: forced abort due to illegal log prefix [" << log_prefix << "]" << std::endl;
        abort();
    }

    std::string file_name = createLogFileName(log_prefix_backup_);
    log_file_with_path_ = log_directory + file_name;
    outptr_ = createLogFile(log_file_with_path_);
    assert((nullptr != outptr_) && "cannot open log file at startup");
    addLogFileHeader();
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
    bg_.reset(); // flush the log queue
    ss_exit << "\n\t\tg2log file shutdown at: " << localtime_formatted(systemtime_now(), internal::time_formatted);
    filestream() << ss_exit.str() << std::flush;
}

void LogRotateHelper::fileWrite(internal::LogEntry message) {
    rotateLog();
    std::ofstream& out(filestream());
    auto system_time = systemtime_now();
    out << "\n" << localtime_formatted(system_time, date_formatted);
    out << " " << localtime_formatted(system_time, time_formatted); // TODO: time kommer frÃ¥n LogEntry
    out << " " << message << std::flush;
}

std::string LogRotateHelper::changeLogFile(const std::string& directory) {
    std::string file_name = createLogFileName(log_prefix_backup_);
    std::string prospect_log = directory + file_name;
    std::unique_ptr<std::ofstream> log_stream = createLogFile(prospect_log);
    if (nullptr == log_stream) {
        backgroundFileWrite("Unable to change log file. Illegal filename or busy? Unsuccessful log name was:" + prospect_log);
        return ""; // no success
    }
    addLogFileHeader();
    std::ostringstream ss_change;

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
        // get length of file:
        is.seekp(0, std::ios::end);
        int length = is.tellp();
        is.seekp(0, std::ios::beg);
        if (length > max_log_size_) {
            std::ostringstream gz_file_name;
            gz_file_name << log_file_with_path_ << ".";
            gz_file_name << g2::localtime_formatted(g2::systemtime_now(), "%Y-%m-%d-%H-%M-%S");
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
            expireArchives(log_directory_, ss.str(), max_archive_log_count_);

            return true;
        }
    }
    return false;
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
    if (gzclose(output) != Z_OK);
    if (fclose(input) != 0) {
        return false;
    }

    return true;
}

std::string LogRotateHelper::fileName() {
    return log_file_with_path_;
}

void LogRotateHelper::addLogFileHeader() {
    filestream() << header();
}

