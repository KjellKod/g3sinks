/** ==========================================================================
* 2011 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*/

#include <gtest/gtest.h>
#include <iostream>
#include <g3log/logworker.hpp>
#include <g3log/g3log.hpp>

int main(int argc, char *argv[])
{

  testing::InitGoogleTest(&argc, argv);
  srand(time(NULL));
  std::stringstream fileName;
  fileName << "UnitTest" << geteuid();
  auto logger_n_handle = g3::LogWorker::createWithDefaultLogger(fileName.str(), "/tmp");
  g3::initializeLogging(logger_n_handle.worker.get());
  int return_value = RUN_ALL_TESTS();
  g3::internal::shutDownLogging();
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "FINISHED WITH THE TESTING" << std::endl;
  return return_value;
}
