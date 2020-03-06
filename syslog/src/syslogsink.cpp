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

void SyslogSink::setIdentity(const char* id)
{
// https://www.gnu.org/software/libc/manual/html_node/openlog.html
// syslog doesn't copy the identity string internally, it only keeps a pointer to the string.
// We have to create a new string, then switch the pointer used by syslog, then
// delete the previous string.
// note that we have only one element in the list normally,
// but here we need to insert a second one temporarily, as 
// syslog's internal pointer must never point to an invalid memory location.
_identity_list.push_front(std::string(id));
::openlog(_identity_list.begin() -> c_str(), _option, _facility);
// at this point syslog has a new valid string, we can remove the previous one:
_identity_list.pop_back();
}

// The actual log receiving function
void SyslogSink::syslog(LogMessageMover message)
{
    if(_firstEntry)
    {
        openlog(_identity_list.begin() -> c_str(), _option, _facility);
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
    
    _identity_list.push_front(std::string(identity));
    
}

SyslogSink::~SyslogSink()
{
    ::closelog();
}

}
