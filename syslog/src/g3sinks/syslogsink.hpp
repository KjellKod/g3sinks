/* M.B. Gratton 2019-01-06
 *
 * A G3Log sink that writes to syslog on Linux. This puts your log message into the system
 * logger.  Settings with syslog-ng/rsyslog can then be used to sort these to separate files,
 * forward them to a remote machine, etc.
 *
 * Mapping between G3 and syslog concepts:
 * - The log level is mapped to a syslog level by default as follows
 *      1. G3LOG_DEBUG -> LOG_DEBUG
 *      2. INFO -> LOG_INFO
 *      3. (INFO.value + WARNING.value )/2 -> LOG_NOTICE [there's no notice level in G3)
 *      4. WARNING -> LOG_WARNING
 *      5. FATAL -> LOG_CRIT
 *    If you log to a custom level between these, the level will be "rounded down", e.g. log to a
 *    level of (FATAL.value-1) and it'll be mapped to WARNING
 *    You can adjust this behavior through the setLevel() function.
 * - The "identity" of your process can be changed via the constructor or setIdentity().  This
 *   is usually the primary value for filtering.
 * - The "facility" of the messages is set to LOG_USER by default, but use setFacility() to change.
 * - To get something written to the console, use echoToStderr()
 * - On the first log (only) of your program, a special header can be written.
 *
 * A word of caution: syslog will timestamp each record itself, but with the time that the syslog
 * daemon recieved the message, not the time it was created.  You can include the creation time
 * in your record via the formatter.
 *
 */
#pragma once
#include <map>
#include <string>
#include <list>

namespace g3
{

struct LogMessage;
    
class SyslogSink
{
public:
    using LogLevel = LEVELS;
    using LogDetailsFunc = std::string (*) (const LogMessage&);
    
    SyslogSink(const char* identity = "g3log");
    virtual ~SyslogSink();

    void syslog(LogMessageMover message);
    
    void setFormatter(LogDetailsFunc func) { _log_details_func = func; }
    void setLogHeader(const char* change) { _header = change; }
    void echoToStderr(); // enables the Linux extension LOG_PERROR
    void muteStderr(); // opposite of echoToStderr
    
    void setIdentity(const char* id);
    void setFacility(int facility) { _facility = facility; }
    void setOption(int option) { _option = option; }
    void setLevelMap(std::map<int, int> const& m);

    void setLevel(LogLevel level, int syslevel);

private:
    LogDetailsFunc _log_details_func;
    std::map<int, int> _levelMap;

    std::unique_ptr<std::string> _identity; // buffer for syslog identity change
    int _facility; // syslog facility, defaults to LOG_USER
    int _option; // syslog options, defaults to LOG_PID

    std::string _header; // Written when logging starts
    bool _firstEntry; // notices that logging starts ...

    void openLog();

    SyslogSink& operator=(SyslogSink const&) = delete;
    SyslogSink(SyslogSink const& other) = delete;
    int priority(LogLevel level);
};

}
