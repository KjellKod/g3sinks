#include "Windows.h"
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <g3log_tracelogging/g3log_tracelogging.h>
#include <TraceLoggingProvider.h>

TRACELOGGING_DEFINE_PROVIDER(
  traceProvider,
  "g3log_tracelogging",
  // {977bf0ce-b000-5495-59a6-c39d65f0bec1}
  (0x977bf0ce, 0xb000, 0x5495, 0x59, 0xa6, 0xc3, 0x9d, 0x65, 0xf0, 0xbe, 0xc1));


// The main function bootstraps into the IFrameworkView.
int main(HINSTANCE, HINSTANCE, PWSTR, int)
{
  TraceLoggingRegister(traceProvider);
  using namespace g3;
  std::unique_ptr<LogWorker> logworker{ LogWorker::createLogWorker() };
  auto sinkHandle = logworker->addSink(std::make_unique<g3logTracelogging>(),
    &g3logTracelogging::ReceiveLogMessage);

  // initialize the logger before it can receive LOG calls
  initializeLogging(logworker.get());
  LOG(WARNING) << "g3logTracelogging test: This log call, may or may not happend before"
    << "the sinkHandle->call below";

  g3::internal::shutDownLogging();

  return 0;
}