#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <iostream>
#include "g3sinks/ColorCoutSink.h"


// NOTE: This works only on Linux/OSX
// TODO KJELLKOD For Windows terminals you can tweak this easily: https://stackoverflow.com/a/4053879/1066879
int main() {
   using namespace g3;
   {
      std::unique_ptr<LogWorker> logworker {LogWorker::createLogWorker()};
      g3::initializeLogging(logworker.get());
      auto sinkHandle = logworker->addSink(std::make_unique<ColorCoutSink>(),
                                           &ColorCoutSink::ReceiveLogMessage);
 
      LOG(G3LOG_DEBUG) << "A DEBUG message in green";
      LOG(INFO) << "An INFO message in white";
      LOG(WARNING) << "A WARNING message will be in yellow";
#if 0
      LOG(FATAL) << "This call will exit the example. A failed CHECK call, \n"
                 << "a LOG(FATAL) or a fatal crash (seg fault or similar) will show up in RED\n"
                 << "\n\nThis call will also be DUPLICATED. One FATAL LOG message will be shown through the \n"
                 << "the crash handler and sent to cout, the LOG call will asynchronously send the LOG output to cout.\n"
                 << "Depending on when the crash handler sends the cout message the duplicate message will show up in \n"
                 << "ANY of the defined colors and might be INTERLEAVED with the other messages. Interleaving is to be expected by asynchronously calls"
#endif
   } // at this point all enqueued LOGs will have gone through in FIFO order and the logger is shutdown.

   //
   std::cout << "Check this file (" __FILE__ << ") for " << "\033[" << ColorCoutSink::RED
             << "m" << "FATAL calls" << "\033[" << ColorCoutSink::WHITE << "m" <<
             " and program crashes with stackdump saved to the sink"
             << "\033[m" << std::endl;
   return 0;
}

