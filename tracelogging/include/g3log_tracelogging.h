#pragma once
#include <g3log/logmessage.hpp>

// Trace Logging
#include <TraceLoggingProvider.h>

TRACELOGGING_DECLARE_PROVIDER(traceProvider);

#define TraceLogWrite(eventName, ...) \
    _TlgWrite_imp(_TlgWrite, \
    traceProvider, eventName, \
    (NULL, NULL), \
    __VA_ARGS__)

// *** BEGIN EXAMPLE CLIENT TRACELOGGING DEFINITION ***
// REQUIRES CLIENT TRACELOGGING DEFINITION AS BELOW
// Typically Performed at the top of your main file
//#include <TraceLoggingProvider.h>
//
//TRACELOGGING_DEFINE_PROVIDER(
//  traceProvider,
//  "ndtech",
//  // {c78f9e8e-2e05-5320-8f4e-7bd922ecb3a0}
//  (0xc78f9e8e, 0x2e05, 0x5320, 0x8f, 0x4e, 0x7b, 0xd9, 0x22, 0xec, 0xb3, 0xa0));
// *** END EXAMPLE CLIENT TRACELOGGING DEFINITION ***

// *** BEGIN EXAMPLE CLIENT USAGE ***
//// Typically performed immediately in the main function
//// First you register the traceProvider declared above
//TraceLoggingRegister(traceProvider);
//using namespace g3;
//std::unique_ptr<LogWorker> logworker{ LogWorker::createLogWorker() };
//auto sinkHandle = logworker->addSink(std::make_unique<g3logTracelogging>(),
//  &g3logTracelogging::ReceiveLogMessage);
//
//// initialize the logger before it can receive LOG calls
//initializeLogging(logworker.get());
//LOG(WARNING) << "g3logTracelogging test: This log call, may or may not happend before"
//<< "the sinkHandle->call below";
// *** END EXAMPLE CLIENT USAGE ***

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
