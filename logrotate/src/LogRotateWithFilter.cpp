/** ==========================================================================
* 2015 by KjellKod.cc
*
* This code is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*
* PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
* ********************************************* */

#include "g3sinks/LogRotateWithFilter.h"


LogRotateWithFilter::LogRotateWithFilter() {}
LogRotateWithFilter::~LogRotateWithFilter() {}

/**
* @param logToFile rotating file logging sink
* @param ignoreLevels vector with all logging levels to ignore
*/
void LogRotateWithFilter::AddLogging(LogRotateUniquePtr logToFile, FilterOut ignoreLevels) {

    auto rawPtr = logToFile.release();
    std::shared_ptr<LogRotate> safePtr(rawPtr);
    filteredOut.push_back(std::make_pair(safePtr, ignoreLevels));
}