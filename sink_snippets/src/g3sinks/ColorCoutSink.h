#pragma once
#include <string>
#include <iostream>
#include <g3log/logmessage.hpp>

#ifdef _WIN32
#include "ColorCoutSinkWindows.h"
#else
#include "ColorCoutSinkLinux.h"
#endif

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

