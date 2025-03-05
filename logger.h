#ifndef Logger_Base
#include <fstream>
#include <io.h>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <string>
#include <time.h>
// #include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif

using namespace std;

template <class T>
string type_name()
{
    typedef typename remove_reference<T>::type TR;
    unique_ptr<char, void (*)(void *)> own(
#ifndef _MSC_VER
        abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr),
#else
        nullptr,
#endif
        free);
    string r = own != nullptr ? own.get() : typeid(TR).name();
    if (is_const<TR>::value)
        r += " const";
    if (is_volatile<TR>::value)
        r += " volatile";
    if (is_lvalue_reference<T>::value)
        r += "&";
    else if (is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}
class Logger_Base
{
private:
    time_t raw_time;
    struct tm *time_info;
    std::string log_name;
    std::string __CALLER;
    std::string __FILE;
    unsigned int __COLOR;
    unsigned int __BG_COLOR;
    char log_filename[100];
    char __TIME[40];
    char __DATE[40];
    unsigned int __LINE;
    std::ofstream log_file;

public:
    Logger_Base(const std::string &log_name,
                const std::string &__CALLER,
                const std::string &__FILE,
                unsigned int __LINE) : log_name(log_name),
                                       __CALLER(__CALLER),
                                       __FILE(__FILE),
                                       __LINE(__LINE)
    {
        if (_access("./logs", 0) == -1)
            system("mkdir ./logs");
    }
    ~Logger_Base() {};
    template <typename __TYPE>
    void operator()(__TYPE _message, bool no_print = false, bool save = true)
    {
        time(&raw_time);
        time_info = localtime(&raw_time);
        strftime(__DATE, 40, "%Y-%m-%d", time_info);
        strftime(__TIME, 40, "%H:%M:%S", time_info);
        if (!no_print)
        {
            std::cout << __TIME << " [" << log_name << "] <" << __CALLER << "> in " << __FILE << ", line " << __LINE << ": type:" << type_name<decltype(_message)>() << ", value:" << _message << std::endl;
        }
        if (save)
        {
            sprintf(log_filename, "%s%s%s", "./logs/", __DATE, ".log");
            log_file.open(log_filename, std::ios_base::app);
            log_file << __TIME << " [" << log_name << "] <" << __CALLER << "> in " << __FILE << ", line " << __LINE << ": type:" << type_name<decltype(_message)>() << ", value:" << _message << std::endl;
            log_file.close();
        }
    }
};
#endif
namespace logger
{
#ifdef DEBUG
#define debug Logger_Base("DEBUG", __FUNCTION__, __FILE__, __LINE__)
#else
#define debug NULL
#endif
#define info Logger_Base("INFO", __FUNCTION__, __FILE__, __LINE__)
#define warning Logger_Base("WARNING", __FUNCTION__, __FILE__, __LINE__)
#define error Logger_Base("ERROR", __FUNCTION__, __FILE__, __LINE__)
#define fatal Logger_Base("FATAL", __FUNCTION__, __FILE__, __LINE__)
};