/** ==========================================================================
* 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*/

#include <gtest/gtest.h>
#include <iostream>
#include <g3log/logworker.hpp>
#include <g3log/g3log.hpp>
#include <g3log/std2_make_unique.hpp>
#include <LogRotate.h>

int main(int argc, char *argv[])
{

  testing::InitGoogleTest(&argc, argv);
  srand(time(NULL));
  std::stringstream fileName;
  fileName << "UnitTest" << geteuid();
  auto uniqueLoggerPtr = g3::LogWorker::createWithNoSink();
  auto handle = uniqueLoggerPtr->addSink(std2::make_unique<LogRotate>(fileName.str(), "/tmp/"), &LogRotate::save);

  g3::initializeLogging(uniqueLoggerPtr.get());
  std::cout << "Logging to: " << handle->call(&LogRotate::logFileName).get() << std::endl;

  int return_value = RUN_ALL_TESTS();
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "FINISHED WITH THE TESTING" << std::endl;
  return return_value;
}
