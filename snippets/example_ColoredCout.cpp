
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "ColorCoutSink.hpp"



void doADebugLogCall() {
   LOG(DEBUG) << "Hello World!";
}

void doAnInfoLogCall() {
   LOG(INFO) << "¡Hola El Mundo!";
}

void doAWarningLogCall() {
   LOG(WARNING) << "Hallo Welt!";
}


// the fatal call will trigger a non-colored stackdump in addition to the colored stackdump
// the output of the non-colored stackdump happens concurrently with the other sink outputs so 
// the output might be in a different order. The colored output will ALWAYS be in the same FIFO order 
// but possibly with the main-thread, white stackdump, interleaved. 
// Colored order
// green - debug
// white - info
// yellow - warning
// red - fatal + stackdump
void doAFatalLogCall() {
   LOG(FATAL) << "Hej Världen!";
}


int main() {
   using namespace g3;
   std::unique_ptr<LogWorker> logworker {LogWorker::createLogWorker()};
   initializeLogging(logworker.get());
   auto sinkHandle = logworker->addSink(std::make_unique<ColorCoutSink>(),
                                        &ColorCoutSink::ReceiveLogMessage);
   doADebugLogCall();
   doAnInfoLogCall();
   doAWarningLogCall();
   doAFatalLogCall(); // this will kill the program

   return 0;
}

