#pragma once
#include <string>
#include <iostream>
#include <g3log/logmessage.hpp>

struct ColorCoutSink {

// Linux xterm color
// http://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
  enum FG_Color {YELLOW = 33, RED = 31, GREEN=32, WHITE = 97};

  FG_Color GetColor(const LEVELS level) const {
     if (level.value == WARNING.value) { return YELLOW; }
     if (level.value == DEBUG.value) { return GREEN; }
     if (g3::internal::wasFatal(level)) { return RED; }

     return WHITE;
  }
  
  void ReceiveLogMessage(g3::LogMessageMover logEntry) {
     auto level = logEntry.get()._level;
     auto color = GetColor(level);

     std::cout << "\033[" << color << "m" 
       << logEntry.get().toString() << "\033[m" << std::endl;
  }
};


/* Example usage
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

int main() {
   using namespace g3;
   std::unique_ptr<LogWorker> logworker {LogWorker::createLogWorker()};
   auto sinkHandle = logworker->addSink(std2::make_unique<CustomSink>(),
                                     &CustomSink::ReceiveLogMessage);

 ... continuation
*/

