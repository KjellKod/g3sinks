/** ==========================================================================
* 2015 by KjellKod.cc
*
* This code is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*
* PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
* ********************************************* */

#include "RotateTestHelper.h"
#include <memory>
#include <fstream>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <cerrno>
#include <unistd.h>

namespace RotateTestHelper {

/// Ref: StackOverflow + http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
   std::string ReadContent(const std::string filename) {
      std::ifstream readIn(filename.c_str(), std::ios::in | std::ios::binary);
      if (readIn) {
         std::shared_ptr<void> raii(nullptr, [&](void*) {
            readIn.close(); //std::cout << __FILE__ << ":" << __LINE__ << " closed file: " << filename << std::endl;
         });

         std::string contents;
         readIn.seekg(0, std::ios::end);
         contents.resize(readIn.tellg());
         readIn.seekg(0, std::ios::beg);
         readIn.read(&contents[0], contents.size());
         return contents;
      }
      throw errno;
   }


   std::string ExtractContent(const std::map<long, std::string>& content) {
      std::string extracted = "\n ";
      for (const auto& pair : content) {
         std::string file = pair.second;
         extracted += file + ", \n";
      }
      extracted += "\n";
      return extracted;
   }



   bool Exists(const std::string content, const std::string expected) {
      auto found = content.find(expected);
      return found != std::string::npos;
   }

   bool DoesFileEntityExist(const std::string& pathToFile) {
      int check = access(pathToFile.c_str(), F_OK);
      bool found = (0 == check);
      if (!found) {
         std::cerr << pathToFile << " was not found: " << std::strerror(errno) << std::endl;
      }
      return found;
   }

} // RotateTestHelper
