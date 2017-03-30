# g3sinks
sinks to g3logger

#  Sinks
Each sink will have its own LICENSE file. Make sure to understand what the sink provides
before you use it. 

## Log Rotate + Log Rotate with Filter
Adapted from vrecan/g2log-dev https://bitbucket.org/vrecan/g2log-dev
Rotation of logs. 
The license is public domain, a.k.a the  UNLICENSE.
See details at the sink [location](https://github.com/KjellKod/g3sinks/tree/master/logrotate).

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


### Building with unit tests added
```
cd g3sinks
cd 3rdparty
unzip gtest-1.7.0.zip
cd ..
cd logrotate
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/usr/local/ -DBOOST_ROOT=/usr/local -DADD_LOGROTATE_UNIT_TEST=ON ..
make -j
```

### Executing the unit tests
```
./UnitTestRunneer
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
```

// main.cpp
#include <g3log/g3log.hpp>
#include <g3log/logworker.h>
#include <g3sinks/logrotate.hpp>
#include <g3log/std2_make_unique.hpp>

int main(int argc, char**argv) {
   using namespace g3;
   std::unique_ptr<LogWorker> logworker{ LogWorker::createLogWorker() };
   auto sinkHandle = logworker->addSink(std2::make_unique<LogRotate>(),
                                          &LogRotate::save);
   
   // initialize the logger before it can receive LOG calls
   initializeLogging(logworker.get());            
            
   // You can call in a thread safe manner public functions on the logrotate sink
   // The call is asynchronously executed on your custom sink.
   const int k10MBInBytes = 10 * 1024 * 1024;
   std::future<void> received = sinkHandle->call(&LogRotate::setMaxLogSize, k10MBInBytes);
   
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
These sinks for the g3logger are available for free and all of its source code is public domain.  A great way of saying thanks is to send a donation. It would go a long way not only to show your support but also to boost continued development.

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/g3log/25)

* $5 for a cup of coffee
* $10 for pizza 
* $25 for a lunch or two
* $100 for a date night with my wife (which buys family credit for evening coding)
* $$$ for upgrading my development environment
* $$$$ :)

Cheers

Kjell
