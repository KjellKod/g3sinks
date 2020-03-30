#pragma once
#include <g3log/logmessage.hpp>
#include <string>
#include <unistd.h>
#include <cstring>

namespace g3{

class FileLogSink {
public:
  FileLogSink(int fileDesc, bool close_by_sink): fd(fileDesc), Close_fd(close_by_sink) {};
  ~FileLogSink() {
       sync();
       if(Close_fd && (fd >= 0)) close(fd);};
  
  void Rotate(int newFileDesc, bool close_by_sink) {
      int oldfd = fd;
      fd = newFileDesc;
      sync();
      if(Close_fd && (oldfd >= 0)) close(oldfd);
      Close_fd = close_by_sink;
     }
  
  void ReceiveLogMessage(g3::LogMessageMover logEntry) {
     std::string data = logEntry.get().toString();
     write(fd, data.c_str(), strlen(data.c_str()) );
     write(fd, "\n", 2 );
   }
  
  void sync() {fsync(fd);};
  
private:
    int fd;
    bool Close_fd;
};
} // g3
