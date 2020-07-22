#pragma once
// TraceView command line interface
// https://docs.microsoft.com/en-us/windows-hardware/drivers/devtest/traceview-control-commands?redirectedfrom=MSDN
// Windows SDK contains the TraceView : https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/


#include <windows.h> // Defines macros used by TraceLoggingProvider.h
#include <TraceLoggingProvider.h>  // The native TraceLogging API


#include <g3log/logmessage.hpp>
#include <g3log/std2_make_unique.hpp>

TRACELOGGING_DECLARE_PROVIDER(traceProvider);

#define TraceLogWrite(eventName, ...) \
    _TlgWrite_imp(_TlgWrite, \
    traceProvider, eventName, \
    (NULL, NULL), \
    __VA_ARGS__)

/* *** BEGIN EXAMPLE CLIENT TRACELOGGING DEFINITION ***
// REQUIRES CLIENT TRACELOGGING DEFINITION AS BELOW
// Typically Performed at the top of your main file
#include <TraceLoggingProvider.h>

TRACELOGGING_DEFINE_PROVIDER(
  traceProvider,
  "g3logTracelogging",
  // {977bf0ce-b000-5495-59a6-c39d65f0bec1}
  (0x977bf0ce, 0xb000, 0x5495, 0x59, 0xa6, 0xc3, 0x9d, 0x65, 0xf0, 0xbe, 0xc1));
 *** END EXAMPLE CLIENT TRACELOGGING DEFINITION *** */

/* *** BEGIN EXAMPLE CLIENT USAGE ***
// Typically performed immediately in the main function
// First you must register the traceProvider declared above
TraceLoggingRegister(traceProvider);
using namespace g3;
std::unique_ptr<LogWorker> logworker{ LogWorker::createLogWorker() };
auto sinkHandle = logworker->addSink(std2::make_unique<g3logTracelogging>(),
  &g3Tracelogging::ReceiveLogMessage);

// initialize the logger before it can receive LOG calls
initializeLogging(logworker.get());
LOG(WARNING) << "g3logTracelogging test: This log call, may or may not happend before"
<< "the sinkHandle->call below";
 *** END EXAMPLE CLIENT USAGE *** */

struct g3logTracelogging {

  void ReceiveLogMessage(g3::LogMessageMover logEntry) {
    auto level = logEntry.get()._level;

    if (logEntry.get().level() == "DEBUG") {
      TraceLogWrite(
        "g3logTracelogging",
        TraceLoggingString(logEntry.get().level().c_str(), "LOG_LEVEL"),
        TraceLoggingString(logEntry.get().threadID().c_str(), "THREAD_ID"),
        TraceLoggingString(logEntry.get().timestamp().c_str(), "TIME_STAMP"),
        TraceLoggingString(logEntry.get().file().c_str(), "FILE"),
        TraceLoggingString(logEntry.get().file_path().c_str(), "FILE_PATH"),
        TraceLoggingString(logEntry.get().function().c_str(), "FUNCTION"),
        TraceLoggingString(logEntry.get().message().c_str(), "MESSAGE"),
        TraceLoggingLevel(5));
    }
    else if (logEntry.get().level() == "INFO") {
      TraceLogWrite(
        "g3logTracelogging",
        TraceLoggingString(logEntry.get().level().c_str(), "LOG_LEVEL"),
        TraceLoggingString(logEntry.get().threadID().c_str(), "THREAD_ID"),
        TraceLoggingString(logEntry.get().timestamp().c_str(), "TIME_STAMP"),
        TraceLoggingString(logEntry.get().file().c_str(), "FILE"),
        TraceLoggingString(logEntry.get().file_path().c_str(), "FILE_PATH"),
        TraceLoggingString(logEntry.get().function().c_str(), "FUNCTION"),
        TraceLoggingString(logEntry.get().message().c_str(), "MESSAGE"),
        TraceLoggingLevel(4));
    }
    else if (logEntry.get().level() == "WARNING") {
      TraceLogWrite(
        "g3logTracelogging",
        TraceLoggingString(logEntry.get().level().c_str(), "LOG_LEVEL"),
        TraceLoggingString(logEntry.get().threadID().c_str(), "THREAD_ID"),
        TraceLoggingString(logEntry.get().timestamp().c_str(), "TIME_STAMP"),
        TraceLoggingString(logEntry.get().file().c_str(), "FILE"),
        TraceLoggingString(logEntry.get().file_path().c_str(), "FILE_PATH"),
        TraceLoggingString(logEntry.get().function().c_str(), "FUNCTION"),
        TraceLoggingString(logEntry.get().message().c_str(), "MESSAGE"),
        TraceLoggingLevel(3));
    }
    else if (logEntry.get().level() == "FATAL") {
      TraceLogWrite(
        "g3logTracelogging",
        TraceLoggingString(logEntry.get().level().c_str(), "LOG_LEVEL"),
        TraceLoggingString(logEntry.get().threadID().c_str(), "THREAD_ID"),
        TraceLoggingString(logEntry.get().timestamp().c_str(), "TIME_STAMP"),
        TraceLoggingString(logEntry.get().file().c_str(), "FILE"),
        TraceLoggingString(logEntry.get().file_path().c_str(), "FILE_PATH"),
        TraceLoggingString(logEntry.get().function().c_str(), "FUNCTION"),
        TraceLoggingString(logEntry.get().message().c_str(), "MESSAGE"),
        TraceLoggingLevel(1));
    }

  }
};
