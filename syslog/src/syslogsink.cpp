#include "g3log/logmessage.hpp"
#include "g3sinks/syslogsink.hpp"
#include <syslog.h>

namespace g3 {

   void SyslogSink::setLevelMap(std::map<int, int> const& m) {
      _levelMap = m;
   }

   void SyslogSink::setLevel(LogLevel level, int syslevel) {
      _levelMap[level.value] = syslevel;
   }

   void SyslogSink::echoToStderr() {
      _option |= LOG_PERROR;
      ::openlog(_identity.get() -> c_str(), _option, _facility);
   }

   void SyslogSink::muteStderr() {
      _option &= ~LOG_PERROR;
      ::openlog(_identity.get() -> c_str(), _option, _facility);
   }

   void SyslogSink::setIdentity(const char* id) {
      // https://www.gnu.org/software/libc/manual/html_node/openlog.html
      // syslog doesn't copy the identity string internally, it only keeps a pointer to the string.
      // We have to create a new string, then switch the pointer used by syslog, then
      // delete the previous string.
      // syslog's internal pointer must never point to an invalid memory location.
      auto new_identity = std::make_unique<std::string>(id);
      ::openlog(new_identity.get() -> c_str(), _option, _facility);
      std::swap(_identity, new_identity);
   }

   // The actual log receiving function
   void SyslogSink::syslog(LogMessageMover message) {
      if (_firstEntry) {
         openlog(_identity.get() -> c_str(), _option, _facility);
         if (!_header.empty()) {
            ::syslog(LOG_NOTICE, "%s", _header.c_str());
         }
         _firstEntry = false;
      }
      int level = priority(message.get()._level);
      ::syslog(level, "%s", message.get().toString(_log_details_func).c_str());
   }

   int SyslogSink::priority(LogLevel level) {
      auto left = _levelMap.lower_bound(level.value);
      if (left == _levelMap.end()) {
         return LOG_DEBUG;
      }
      return left->second;
   }

   SyslogSink::SyslogSink(const char* identity)
      : _log_details_func(&LogMessage::DefaultLogDetailsToString)
      , _facility(LOG_USER)
      , _option(LOG_PID)
      , _header("")
      , _firstEntry(true) {
      _levelMap[G3LOG_DEBUG.value] = LOG_DEBUG;
      _levelMap[INFO.value] = LOG_INFO;
      _levelMap[(INFO.value + WARNING.value) / 2] = LOG_NOTICE;
      _levelMap[WARNING.value] = LOG_WARNING;
      _levelMap[FATAL.value] = LOG_CRIT;

      _identity = std::make_unique<std::string>(identity);
   }

   SyslogSink::~SyslogSink() {
      ::closelog();
   }

}
