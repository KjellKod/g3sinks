## Snippets
The snippets directory contain examples that will not be installed
but can guide you in some simple use cases that can help you when 
creating your own custom sinks. 


## Colored Cout
A simple example where the log is colored differently depending on what
logging level it comes with. The color example is for Linux xterm
.Code snippet: [colored cout](ColorCoutSink.hpp)

## File Log
A simple file logger, with the specificity that it logs to an open file descriptor passed by the user. This may solve specific corner cases where only file descriptors are available. Ex: log to shared memory (fd returned by memfd_create()). Or when file opening may be tricky, for example in a [setuid process](http://www.cis.syr.edu/~wedu/Teaching/cis643/LectureNotes_New/Race_Condition.pdf).
