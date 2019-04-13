# g3sinks/tracelogging
sink to allow logging through Windows TraceLogging

# Building and installing
To build and install run the following commands
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=<Debug|Release> -DCMAKE_PREFIX_PATH=<path to install folder> -DCMAKE_INSTALL_PREFIX=<path to install folder> ..
cmake --build . --target INSTALL --config <Release/Debug>
```

# g3log_tracelogging Sink Usage Code Example
In order for this to work properly, g3log will need to be installed and the location of g3logger.dll will need to be part of the path or specifically added by the build command

Kernel32.lib is also a required dependency

This code also relies on Microsft TraceLogging and requires the name of the provider be traceProvider

Something to be aware of is that Windows.h must always be included before including TraceLoggingProvider

Use the traceview.exe tool which is installed with the WindowsSDK to view trace events

See Doug Cook''s excellent video at https://www.youtube.com/watch?v=FYi5No2nwoI for more information about Windows TraceLogging

```
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

  g3::shutDownLogging();

  TraceLoggingUnregister(traceProvider);

  return 0;
}
```
