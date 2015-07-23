#include "FilterTest.h"
#include "g3sinks/LogRotateWithFilter.h"
#include <iostream>
#include <cerrno>
#include <cstring>

TEST_F(FilterTest, CreateObject) {
   std::string logfilename;
    {
      LogRotateWithFilter logWithFilter;
    } // RAII flush of log
}

