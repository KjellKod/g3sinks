/** ==========================================================================
* 2015 by KjellKod.cc
*
* This code is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*
* PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
* ********************************************* */


#pragma once

#include <g3sinks/LogRotate.h>
#include <utility>
#include <memory>
#include <vector>
#include <g3log/loglevels.hpp>

class LogRotateWithFilter {

  public:
   LogRotateWithFilter();
   virtual ~LogRotateWithFilter();
   using LogRotatePtr = std::shared_ptr<LogRotate>;
   using LogRotateUniquePtr = std::unique_ptr<LogRotate>;
   
   using FilterOut = std::vector<LEVELS>;
   using LogAndFilter = std::pair<LogRotatePtr, FilterOut>;

   void AddLogging(LogRotateUniquePtr logToFile, FilterOut ignoreLevels);


private:
   std::vector<LogAndFilter> filteredOut;

};