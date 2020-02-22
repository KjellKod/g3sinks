/** ==========================================================================
* 2020 Joel Stienlet
* this file is part of g3log by KjellKod.cc 
*
* This code is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================*/


#pragma once

/*
 
 These deep copy types have been introduced to make it easier to
 call sink methods with dynamically allocated data, without 
 having to wait for the worker thread to finish.
 They work together with the "NoJn"(No thread Join)-suffixed variants of
 the corresponding sink methods.
 
 Alternatively, you can:
  - use static data
  - wait for the thread (join it) with std::future .get()

 */
#include <cstring>
#include <functional>
#include <memory>

namespace g3
{

// RAII string type with its deleter (Dltr) included to cross library boundaries 
class DltrStr
{
public:
    DltrStr() = delete;
    // the deleter accompanies the memory block to enable it beeing freed in the library (using a different heap, as on Windows):
    DltrStr(const char * content) {
        size_t len = strlen(content);
        _content = std::shared_ptr<char>(new char[len + 1], [](char *b){ delete[](b);} );
        memcpy(_content.get(), content, len+1); };
        
    DltrStr(const std::string& content) {
        size_t len = content.length();
        _content = std::shared_ptr<char>(new char[len + 1], [](char *b){ delete[](b);} );
        memcpy(_content.get(), content.c_str(), len+1); };
        
    const char *c_str() {return _content.get();};
    
private:
    std::shared_ptr<char> _content;
};

}


