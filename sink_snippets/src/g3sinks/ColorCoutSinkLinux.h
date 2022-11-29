#pragma once

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
