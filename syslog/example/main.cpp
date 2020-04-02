#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <iomanip>
#include <thread>
#include <iostream>
#include <sstream>
#include <memory>
#include <g3sinks/syslogsink.hpp>

#include <g3log/loglevels.hpp>
#ifdef DBUG
// const LEVELS DBUG{G3LOG_DEBUG.value+1, "DBUG"}; // defined already, ref g3log cmake build options
#else 
const LEVELS DBUG{G3LOG_DEBUG.value+1, "DBUG"};
#endif 
// const LEVELS INFO{INFO.value+1, {"INFO"}}; // defined already
const LEVELS NOTE{(INFO.value + WARNING.value)/2, {"NOTE"}};
const LEVELS WARN{WARNING.value, {"WARN"}};
const LEVELS ERRR{WARN.value+1, {"ERRR"}};
const LEVELS FATL{FATAL.value, {"FATL"}};

/*
 * Some example formatters for records
 */
std::string myFormat(const g3::LogMessage& msg)
{
    static std::stringstream ss;
    ss.str("");
    ss << msg.timestamp() << " " 
        << msg.level() << " " 
        << msg.file() << ":" << msg.function() << ":" << msg.line();
    ss << "\t| ";                 
    return ss.str();
}

std::string sysFormat(const g3::LogMessage& msg)
{
    static std::stringstream ss;
    ss.str("");
    ss << "[" << msg.level() << " " 
       << msg.file() << ":" << msg.function() << ":" << msg.line();
    ss << "] ";                 
    return ss.str();
}

   
/*
 * Wrap the unique_ptr for the worker in a singleton 
 */
class logSingleton {
    std::unique_ptr<g3::LogWorker> worker;
    static logSingleton& get() {
        static std::unique_ptr<logSingleton> s_instance;
        if (!s_instance)
            s_instance = std::make_unique<logSingleton>();
        return *s_instance;
    }
    friend void startLogger();
    friend void stopLogger();
};

void stopLogger()
{
    logSingleton::get().worker.reset();    
}

/*
 * This is a useful place to adjust the syslog settings
 * before logging begins.
 */
void startLogger()
{
    // Example of adjusting settings
    auto sink = std::make_unique<g3::SyslogSink>("g3syslog");
    sink->setFormatter(&sysFormat);
    sink->echoToStderr();
    
    // Boilerplate startup code
    auto& inst = logSingleton::get();
    if (inst.worker) {
        stopLogger();
    }
    inst.worker = g3::LogWorker::createLogWorker();
    inst.worker->addSink(std::move(sink), &g3::SyslogSink::syslog);
    g3::initializeLogging(inst.worker.get());
}



int main(int argc, char** argv)
{
    startLogger();
        

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
    LOG(DBUG) << "debug level log";
    LOG(INFO) << "info level log";    
    LOG(NOTE) << "Notice level log";
    LOG(WARN) << "Warning level log";
    LOG(ERRR) << "error level log";
    LOG(FATL) << "fatal level log";
}


