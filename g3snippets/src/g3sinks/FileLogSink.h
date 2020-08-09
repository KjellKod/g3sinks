#pragma once
// seemingly this is not needed
// #define _GNU_SOURCE

#include <g3log/logmessage.hpp>
#include <string>
#include <unistd.h>
#include <cstring>

/**
* A simple file logger, with the specificity that it logs to an open 
file descriptor passed by the user. This may solve specific corner cases 
where only file descriptors are available. Ex: log to shared memory 
(fd returned by memfd_create()). Or when file opening may be tricky, 
for example in a [setuid process](http://www.cis.syr.edu/~wedu/Teaching/cis643/LectureNotes_New/Race_Condition.pdf).
*/
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
     auto ignored = write(fd, data.c_str(), strlen(data.c_str()) );
     ignored = write(fd, "\n", 2 );
   }
  
  void sync() {fsync(fd);};
  
private:
    int fd;
    bool Close_fd;
};
