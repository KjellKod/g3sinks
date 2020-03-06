#include "g3log/logmessage.hpp"
#include "g3sinks/syslogsink.hpp"
#include <syslog.h>

namespace g3
{

void SyslogSink::setLevelMap(std::map<int, int> const& m)
{
    _levelMap = m;
}

void SyslogSink::setLevel(LogLevel level, int syslevel)
{
    _levelMap[level.value] = syslevel;
}

void SyslogSink::echoToStderr()
{
    _option |= LOG_PERROR;
}

// The actual log receiving function
void SyslogSink::syslog(LogMessageMover message)
{
    if(_firstEntry)
    {
        openlog(_identity.c_str(), _option, _facility);
        if(!_header.empty())
        {
            ::syslog(LOG_NOTICE, "%s", _header.c_str());
        }
        _firstEntry = false;
    }
    int level = priority(message.get()._level);
    ::syslog(level, "%s", message.get().toString(_log_details_func).c_str());
}

int SyslogSink::priority(LogLevel level)
{
    auto left = _levelMap.lower_bound(level.value);
    if(left == _levelMap.end())
    {
        return LOG_DEBUG;
    }
    return left->second;
}

SyslogSink::SyslogSink(const char* identity)
    : _log_details_func(&LogMessage::DefaultLogDetailsToString)
    , _identity(identity)
    , _facility(LOG_USER)
    , _option(LOG_PID)
    , _header("")
    , _firstEntry(true)
{
    _levelMap[G3LOG_DEBUG.value] = LOG_DEBUG;
    _levelMap[INFO.value] = LOG_INFO;
    _levelMap[(INFO.value + WARNING.value)/2] = LOG_NOTICE;
    _levelMap[WARNING.value] = LOG_WARNING;
    _levelMap[FATAL.value] = LOG_CRIT;
}

SyslogSink::~SyslogSink()
{
    ::closelog();
}

}
