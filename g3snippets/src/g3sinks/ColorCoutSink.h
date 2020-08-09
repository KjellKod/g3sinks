#pragma once
#include <string>
#include <iostream>
#include <g3log/logmessage.hpp>

// NOTE: This works only on Linux/OSX
// TODO KjellKod: For Windows terminals you can tweak this easily: https://stackoverflow.com/a/4053879/1066879
struct ColorCoutSink {
   // Linux xterm color
   // http://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
   enum FG_Color {YELLOW = 33, RED = 31, GREEN = 32, WHITE = 37};

   FG_Color GetColor(const LEVELS level) const {
      if (level.value == WARNING.value) { return YELLOW; }
      if (level.value == G3LOG_DEBUG.value) { return GREEN; }
      if (g3::internal::wasFatal(level)) { return RED; }

      return WHITE;
   }

   void reset() {
      std::ostringstream oss;
      oss << "\033[" << WHITE << "m" << " " << "\033[m" << std::endl;
      std::cout << oss.str();
   }

   void ReceiveLogMessage(g3::LogMessageMover logEntry) {
      auto level = logEntry.get()._level;
      auto color = GetColor(level);

      reset();
      std::ostringstream oss;
      oss << "\033[" << color << "m"
                << logEntry.get().toString() << "\033[m" << std::endl;
      std::cout << oss.str();
      reset();
   }
};


/* Example usage from snippet_coloredcout_main.cpp
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <iostream>
#include "g3sinks/ColorCoutSink.h"

int main() {
   using namespace g3;
   {
      std::unique_ptr<LogWorker> logworker {LogWorker::createLogWorker()};
      g3::initializeLogging(logworker.get());
      auto sinkHandle = logworker->addSink(std::make_unique<ColorCoutSink>(),
                                           &ColorCoutSink::ReceiveLogMessage);

      LOG(DEBUG) << "A DEBUG message in green";
      LOG(INFO) << "An INFO message in white";
      ...
*/

