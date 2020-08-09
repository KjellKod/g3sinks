# g3sinks
Logging sinks to be used with [G3log](https://github.com/KjellKod/g3log)

#  Sinks
Each sink will have its own LICENSE file. Make sure to understand what the sink provides
before you use it. 

## Log Rotate + Log Rotate with Filter
Adapted from vrecan/g2log-dev https://bitbucket.org/vrecan/g2log-dev
Rotation of logs. 
The license is public domain, a.k.a the  UNLICENSE.
See details at the sink [location](https://github.com/KjellKod/g3sinks/tree/master/logrotate).

## Syslog
This sink directs g3log messages into the unix syslog system. Settings with syslog-ng/rsyslog 
can then be used to sort these to separate files, forward them to a remote machine, rotate log 
files when they get large, etc.
 
The mapping between G3 and syslog concepts is as follows:

* The log level is mapped to a syslog level by default as

        1. G3LOG_DEBUG -> LOG_DEBUG
        2. INFO -> LOG_INFO
        3. (INFO.value + WARNING.value )/2 -> LOG_NOTICE [there's no notice level in G3)
        4. WARNING -> LOG_WARNING
        5. FATAL -> LOG_CRIT
        
* If you log to a custom level between these, the level will be "rounded down". For example, a log record
    with a level of (FATAL.value-1) will get a syslog level of WARNING
* You can adjust this behavior through the setLevel() function.
* The "identity" of your process can be changed via the constructor's argument or the setIdentity() 
    method.  This is usually the primary value for filtering.
* The syslog "facility code" of the messages is set to LOG_USER by default, but use setFacility() to change.
* By default, nothing is written to the console when you log.  You can have messages also echoed to stderr
    by calling echoToStderr() 
* On the first log (only) of your program, a special banner header can be written. Use setLogHeader() to
    enable this. Use this to distinguish between runs in the logs.

A word of caution: syslog will timestamp each record itself, but with the time that the syslog
daemon recieved the message, not the time it was created.  You can include the creation time
in your record via the formatter. The setFormatter() function accepts a function pointer of the 
type `std::string (*) (const LogMessage&)` to handle the formatting.

An example program is at `syslog/example/main.cpp`.

The syslog sink should work out of the box on Linux systems without additional dependencies.  It has not 
been tested on Cygwin.

This component license is public domain, a.k.a the  UNLICENSE.
See details at the sink [location](https://github.com/KjellKod/g3sinks/tree/master/logrotate).

## Windows tracelogging
sink to allow logging through Windows TraceLogging
For build instructions please see [tracelogging/README](tracelogging/README.md)

# Snippets
[Code snippet examples and a short description](snippets/README.markdown). These are not installed but can 
be used as helpful examples on how to create your custom sinks

# More ...
For more information please look at the Readme and API documentation i.e. https://github.com/KjellKod/g3log
For detailed sink usage please see the header files and tests  in his repository. They are written so that
The sink usage should be clear.

# Building g3sinks
## Requirements
**g3log**<br>
G3sinks requires [g3log](https://github.com/KjellKod/g3log)  to be installed. 
The g3log install location has to be specified during the cmake build step. In the example below the g3log installation location is `/usr/local` with libraries in '/usr/local/lib' and 'headers in '/usr/local/include'
The given argument for the location would then be : 'cmake -DCMAKE_PREFIX_PATH=/usr/local/'

**boost**<br>
G3sinks requires [boost](http://www.boost.org/doc/libs/1_63_0/index.html)  to be installed. 
For installing boost please follow their [instructions](http://www.boost.org/doc/libs/1_63_0/more/getting_started/unix-variants.html). 

The boost install location `BOOST_ROOT` or `Boost_INCLUDE_DIR` has to be specified during the cmake build step.
In the example below the boost  installation 
location is `/usr/local`

**ZLIB**<br>
The ZLIB library must be installed for the logrotate to be able to compress the old log files
in Ubuntu it can be installed with `sudo apt-get install zlib1g-dev`. Please see your specific platform for details or go to the [zlib page](http://www.zlib.net/)


### Building with unit tests added using CMake option "-DBUILD_TEST=ON"
```bash
cd g3sinks
cd logrotate
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/usr/local/ -DBOOST_ROOT=/usr/local -DBUILD_TEST=ON ..
make -j
```

### Executing the unit tests
```
./UnitTestRunneer
```

### Executing all the unit tests and examples
From the `build` directory after a successful make

```
cd .. // out to g3sinks root directory

./scripts/buildAndRunTests.sh
```

### Installing
```
sudo make install
```

Alternative on Debian systems
```
make package
sudo dpkg -i g3LogRotate-<package_version>Linux.deb
```


# G3log and Sink Usage Code Example
Example usage where a logrotate sink is added. The logrotate limit is changed from the default to instead be 10MB. The limit is changed by calling the sink handler which passes the function call through to the actual logrotate sink object.
```cpp

// main.cpp
#include <g3log/g3log.hpp>
#include <g3log/logworker.h>
#include <g3sinks/LogRotate.h>
#include <memory>

int main(int argc, char**argv) {
   using namespace g3;
   std::unique_ptr<LogWorker> logworker{ LogWorker::createLogWorker() };
   auto sinkHandle = logworker->addSink(std::make_unique<LogRotate>(),
                                          &LogRotate::save);
   
   // initialize the logger before it can receive LOG calls
   initializeLogging(logworker.get());            
            
   // You can call in a thread safe manner public functions on the logrotate sink
   // The call is asynchronously executed on your custom sink.
   const int k10MBInBytes = 10 * 1024 * 1024;
   std::future<void> received = sinkHandle->call(&LogRotate::setMaxLogSize, k10MBInBytes);
   // You can manually trigger a rotate of the log
   std::future<bool> rotate = sinkHandle->call(&LogRotate::rotateLog);
   // Run the main part of the application. This can be anything of course, in this example
   // we'll call it "RunApplication". Once this call exits we are in shutdown mode
   RunApplication();

   // If the LogWorker is initialized then at scope exit the g3::shutDownLogging() will be 
   // called automatically. 
   //  
   // This is important since it protects from LOG calls from static or other entities that will go out of
   // scope at a later time. 
   //
   // It can also be called manually if for some reason your setup is different then the one highlighted in
   // this example
   g3::shutDownLogging();
}
```



# Say Thanks
These sinks for g3log are available for free and all of its source code is public domain.  A great way of saying thanks is to send a donation. It would go a long way not only to show your support but also to boost continued development.

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/g3log/25)

* $5 for a cup of coffee
* $10 for pizza 
* $25 for a lunch or two
* $100 for a date night with my wife (which buys family credit for evening coding)
* $$$ for upgrading my development environment
* $$$$ :)

Cheers

Kjell
