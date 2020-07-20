#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <g3log/loglevels.hpp>
#include <g3log/std2_make_unique.hpp>
#include <iomanip>
#include <thread>
#include <iostream>
#include <sstream>
#include <memory>
#include "g3sinks/syslogsink.hpp"


// const LEVELS INFO {INFO.value + 1, {"INFO"}}; // defined already
const LEVELS NOTE {(INFO.value + WARNING.value) / 2, {"NOTE"}};
const LEVELS WARN{WARNING.value, {"WARN"}};
const LEVELS ERRR{WARN.value + 1, {"ERRR"}};
const LEVELS FATL{FATAL.value, {"FATL"}};

/*
 * Some example formatters for records
 */
std::string myFormat(const g3::LogMessage& msg) {
   static std::stringstream ss;
   ss.str("");
   ss << msg.timestamp() << " "
      << msg.level() << " "
      << msg.file() << ":" << msg.function() << ":" << msg.line();
   ss << "\t| ";
   return ss.str();
}

std::string sysFormat(const g3::LogMessage& msg) {
   static std::stringstream ss;
   ss.str("");
   ss << "[" << msg.level() << " "
      << msg.file() << ":" << msg.function() << ":" << msg.line();
   ss << "] ";
   return ss.str();
}


int main(int argc, char** argv) {
   using namespace g3;
   {
      // Example of adjusting settings
      auto sink = std2::make_unique<g3::SyslogSink>("g3syslog");
      sink->setFormatter(&sysFormat);
      sink->echoToStderr();

      std::unique_ptr<LogWorker> logworker {LogWorker::createLogWorker()};
      g3::initializeLogging(logworker.get());
      auto sinkHandle = logworker->addSink(std::move(sink), &g3::SyslogSink::syslog);


      double pi_d = 3.1415926535897932384626433832795;
      float pi_f = 3.1415926535897932384626433832795f;

      LOGF(INFO, "Hi log %d", 123);
      LOG(INFO) << "Test SLOG INFO";
      LOG(G3LOG_DEBUG) << "Test SLOG DEBUG";
      LOG(INFO) << "one: " << 1;
      LOG(INFO) << "two: " << 2;
      LOG(INFO) << "one and two: " << 1 << " and " << 2;
      LOG(G3LOG_DEBUG) << "float 2.14: " << 1000 / 2.14f;
      LOG(G3LOG_DEBUG) << "pi double: " << pi_d;
      LOG(G3LOG_DEBUG) << "pi float: " << pi_f;
      LOG(G3LOG_DEBUG) << "pi float (width 10): " << std::setprecision(10) << pi_f;
      LOGF(INFO, "pi float printf:%f", pi_f);
      LOG_IF(INFO, 1 != 2) << "Turns out that 1 != 2";

      // Custom levels
      LOG(G3LOG_DEBUG) << "debug level log";
      LOG(INFO) << "info level log";
      LOG(NOTE) << "Notice level log";
      LOG(WARN) << "Warning level log";
      LOG(ERRR) << "error level log";
      LOG(FATL) << "fatal level log - this LOG call will kill the process";
   }
   return 0;
}


