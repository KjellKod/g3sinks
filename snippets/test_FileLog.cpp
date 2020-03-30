//
// g++ test_FileLog.cpp -lg3logger -lpthread
//
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "FileLogSink.hpp"
#include <iostream>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/mman.h>

void content_to_stderr(int fd)
{
ssize_t dataSize;
ssize_t usefulBufSize = 1234;
char buf[usefulBufSize+1];
std::cerr << "buffer contents:" << std::endl;
lseek(fd, 0, SEEK_SET);
do{
    dataSize = read(fd, buf, usefulBufSize);
    if(dataSize > 0) {
        buf[dataSize] = 0;
        std::cerr << buf;
       }
  }while(dataSize > 0);
}

int new_mem_fd(const char *name)
{
int fd = memfd_create(name, MFD_CLOEXEC);
if(fd == -1) {
    std::cerr << "ERROR: unable to create a memory mapped file." << std::endl;
    exit(1);
   }
return fd;
}

int main(int argc, char**argv) {   
int fd1 = new_mem_fd("memfile_1");
std::unique_ptr<g3::LogWorker> logworker{ g3::LogWorker::createLogWorker() };
auto sinkHandle = logworker->addSink(std::make_unique<g3::FileLogSink>(fd1, false),
                                    &g3::FileLogSink::ReceiveLogMessage);

initializeLogging(logworker.get());
LOG(INFO) << "This content is written to the first memory file descriptor";
sleep(1); // TODO: wait for the logger to finish, in a cleaner way...
int fd2 = new_mem_fd("memfile_2");
std::future<void> to_wait = sinkHandle->call(&g3::FileLogSink::Rotate, fd2, true);
to_wait.wait();
content_to_stderr(fd1);
close(fd1);

LOG(INFO) << "This content is written to the second memory file descriptor";
sleep(1); // TODO: wait for the logger to finish, in a cleaner way...
to_wait = sinkHandle->call(&g3::FileLogSink::sync);
to_wait.wait();
content_to_stderr(fd2);
close(fd2);
return 0;
}
