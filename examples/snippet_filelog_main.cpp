//
// g++ test_FileLog.cpp -lg3logger -lpthread
// Sink location: snippets



// the manpage of memfd_create says sys/mman.h, but the version of glibc
// installed on travis has it in sys/memfd.h
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "g3sinks/FileLogSink.h"
#include <g3log/std2_make_unique.hpp>
#include <iostream>

#include <string.h>
// for open(2):
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#ifdef HAVE_MMAN
#include <sys/mman.h>
#endif
#ifdef HAVE_MEMFD
#include <sys/memfd.h>
#endif

void content_to_stderr(int fd) {
   ssize_t dataSize;
   ssize_t usefulBufSize = 1234;
   char buf[usefulBufSize + 1];
   std::cerr << "buffer contents:" << std::endl;
   lseek(fd, 0, SEEK_SET);
   do {
      dataSize = read(fd, buf, usefulBufSize);
      if (dataSize > 0) {
         buf[dataSize] = 0;
         std::cerr << buf;
      }
   } while (dataSize > 0);
}

int new_mem_fd(const char* name) {
#ifdef MFD_CLOEXEC
   int fd = memfd_create(name, MFD_CLOEXEC);
#else
   // emulate memfd with a file that we unlink immediately
   std::cerr << "Emulating memfd_create() in " << __FILE__ << std::endl;
   int fd = open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
   if (unlink(name) != 0) {
      std::cerr << "ERROR: unable to unlink " << name << " : " << strerror(errno) << std::endl;
      exit(1);
   }
#endif
   if (fd == -1) {
      std::cerr << "ERROR: unable to create the file descriptor. " << strerror(errno) << std::endl;
      exit(1);
   }
   return fd;
}

int main(int argc, char** argv) {
   int fd1 = new_mem_fd("memfile_1");
   std::unique_ptr<g3::LogWorker> logworker{ g3::LogWorker::createLogWorker() };
   auto sinkHandle = logworker->addSink(std2::make_unique<FileLogSink>(fd1, false),
                                        &FileLogSink::ReceiveLogMessage);

   initializeLogging(logworker.get());
   LOG(INFO) << "This content is written to the first memory file descriptor";
   sleep(1); // TODO: wait for the logger to finish, in a cleaner way...
   int fd2 = new_mem_fd("memfile_2");
   std::future<void> to_wait = sinkHandle->call(&FileLogSink::Rotate, fd2, true);
   to_wait.wait();
   content_to_stderr(fd1);
   close(fd1);

   LOG(INFO) << "This content is written to the second memory file descriptor";
   sleep(1); // TODO: wait for the logger to finish, in a cleaner way...
   to_wait = sinkHandle->call(&FileLogSink::sync);
   to_wait.wait();
   content_to_stderr(fd2);
   close(fd2);
   return 0;
}
