#include <Windows.h>
#include <TraceLoggingProvider.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <g3sinks/g3logtracelogging.h>
#include <g3log/loglevels.hpp>
#include <g3log/std2_make_unique.hpp>
#include <iostream>


// ref: https://docs.microsoft.com/en-us/windows/win32/api/traceloggingprovider/nf-traceloggingprovider-tracelogging_define_provider
TRACELOGGING_DEFINE_PROVIDER(
   traceProvider,
   "g3logTracelogging",
   // {977bf0ce-b000-5495-59a6-c39d65f0bec1}
   (0x977bf0ce, 0xb000, 0x5495, 0x59, 0xa6, 0xc3, 0x9d, 0x65, 0xf0, 0xbe, 0xc1));


// The main function bootstraps into the IFrameworkView.
int main(HINSTANCE, HINSTANCE, PWSTR, int) {
   {
      std::cout << "Starting the tracelogging example. The events can be viewed with Windows 'Event Viewer' " << std::endl;
      TraceLoggingRegister(traceProvider);
      using namespace g3;
      std::unique_ptr<LogWorker> logworker{ LogWorker::createLogWorker() };
      auto sinkHandle = logworker->addSink(std2::make_unique<g3logTracelogging>(),
                                           &g3logTracelogging::ReceiveLogMessage);

      // initialize the logger before it can receive LOG calls
      initializeLogging(logworker.get());
      LOG(G3LOG_DEBUG) << "g3logTracelogging test: DEBUG";
      LOG(INFO) << "g3logTracelogging test: INFO";
      LOG(WARNING) << "g3logTracelogging test: WARNING";
#if 0
      LOG(FATAL) << "g3logTracelogging test: FATAL";
#endif

   } // scope exit g3log will flush all logs.
   // https://docs.microsoft.com/en-us/windows/win32/tracelogging/tracelogging-native-quick-start
   // Stop TraceLogging and unregister the provider
   TraceLoggingUnregister(traceProvider);
   std::cout << "The tracelogging example is finished. The events can be viewed with Windows 'Event Viewer' " << std::endl;

   return 0;
}
