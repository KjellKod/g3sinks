#pragma once
#include <string>
#include <iostream>
#include <g3log/logmessage.hpp>

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN 1
#endif

#include <Windows.h>
#endif

struct ColorCoutSink {
#ifdef _WIN32
   ColorCoutSink() : hHandle_(GetStdHandle(STD_OUTPUT_HANDLE)) {}

    // Windows colors
    // https://docs.microsoft.com/en-us/windows/console/console-screen-buffers#character-attributes
    enum FG_Color { YELLOW = FOREGROUND_RED | FOREGROUND_GREEN, RED = FOREGROUND_RED, GREEN = FOREGROUND_GREEN, WHITE = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED };
#else
   // Linux xterm color
   // http://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
   enum FG_Color {YELLOW = 33, RED = 31, GREEN = 32, WHITE = 37};
#endif


   FG_Color GetColor(const LEVELS level) const {
      if (level.value == WARNING.value) { return YELLOW; }
      if (level.value == G3LOG_DEBUG.value) { return GREEN; }
      if (g3::internal::wasFatal(level)) { return RED; }

      return WHITE;
   }

   void reset() {
#ifdef _WIN32
        SetConsoleTextAttribute(hHandle_, WHITE);
#else
      std::ostringstream oss;
      oss << "\033[" << WHITE << "m" << " " << "\033[m" << std::endl;
      std::cout << oss.str();
#endif
   }

   void ReceiveLogMessage(g3::LogMessageMover logEntry) {
      auto level = logEntry.get()._level;
      auto color = GetColor(level);

      reset();

#ifdef _WIN32
      auto str = logEntry.get().toString();
      SetConsoleTextAttribute(hHandle_, (WORD)color);
      DWORD written;
      WriteConsoleA(hHandle_, str.c_str(), str.size(), &written, nullptr);
#else
      std::ostringstream oss;
      oss << "\033[" << color << "m"
                << logEntry.get().toString() << "\033[m" << std::endl;
      std::cout << oss.str();
#endif
      reset();
   }

#ifdef _WIN32
  private:
   HANDLE hHandle_;
#endif
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

