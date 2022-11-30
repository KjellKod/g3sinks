#pragma once

#ifndef WIN32_LEAN_AND_MEAN
   #define WIN32_LEAN_AND_MEAN 1
#endif

#include <Windows.h>

struct ColorCoutSink {
   ColorCoutSink() : hHandle_(GetStdHandle(STD_OUTPUT_HANDLE)) {}

    // Windows colors
    // https://docs.microsoft.com/en-us/windows/console/console-screen-buffers#character-attributes
    enum FG_Color { YELLOW = FOREGROUND_RED | FOREGROUND_GREEN, RED = FOREGROUND_RED, GREEN = FOREGROUND_GREEN, WHITE = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED };


   FG_Color GetColor(const LEVELS level) const {
      if (level.value == WARNING.value) { return YELLOW; }
      if (level.value == G3LOG_DEBUG.value) { return GREEN; }
      if (g3::internal::wasFatal(level)) { return RED; }

      return WHITE;
   }

   void reset() {
      SetConsoleTextAttribute(hHandle_, WHITE);
   }

   void ReceiveLogMessage(g3::LogMessageMover logEntry) {
      auto level = logEntry.get()._level;
      auto color = GetColor(level);

      reset();

      auto str = logEntry.get().toString();
      SetConsoleTextAttribute(hHandle_, (WORD)color);
      
      std::cout << str;

      reset();
   }

   private:
   HANDLE hHandle_;
};
