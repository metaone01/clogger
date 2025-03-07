#pragma once
#include <cstdlib>
#include <ctime>
#include <direct.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
// #include <cstddef>
// #include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif

namespace logger_base
{
    template <class T>
    std::string type_name()
    {
        typedef std::remove_reference_t<T> TR;
        const std::unique_ptr<char, void (*)(void *)> own(
#ifndef _MSC_VER
            abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr),
#else
            nullptr,
#endif
            free);
        std::string r = own != nullptr ? own.get() : typeid(TR).name();
        if (std::is_const_v<TR>)
            r += " const";
        if (std::is_volatile_v<TR>)
            r += " volatile";
        if (std::is_lvalue_reference_v<T>)
            r += "&";
        else if (std::is_rvalue_reference_v<T>)
            r += "&&";
        return r;
    }

    static std::string date;
    static std::string log_filename;
    static std::ofstream log_file;
    static int today;
    static bool no_print = false;
    static bool save = true;

    template <typename T>
    static void log(const std::string &log_name,
                    const std::string &CALLER,
                    const std::string &FILE,
                    const unsigned int LINE,
                    T _message)
    {
        time_t raw_time;
        tm time_info;
        (void)time(&raw_time);
        (void)localtime_s(&time_info, &raw_time);
        if (time_info.tm_mday != today)
            date = std::to_string(time_info.tm_year + 1900) + "-" +
                   std::to_string(time_info.tm_mon + 1) + "-" +
                   std::to_string(time_info.tm_mday);
        std::string _time = std::to_string(time_info.tm_hour) + ":" +
                            std::to_string(time_info.tm_min) + ":" +
                            std::to_string(time_info.tm_sec);
        if (!no_print)
        {
            std::cout << time << " [" << log_name << "] <" << CALLER << "> in " << FILE << ", line " << LINE << ": type:" << type_name<decltype(_message)>() << ", value:" << _message << std::endl;
        }
        if (save)
        {
            log_file.open(log_filename, std::ios_base::app);
            log_file << _time << " [" << log_name << "] <" << CALLER << "> in " << FILE << ", line " << LINE << ": type:" << type_name<decltype(_message)>() << ", value:" << _message << std::endl;
            log_file.close();
        }
    }

    template <typename T>
    static void ignore(T _message) {}
}
namespace logger
{

    static void init(bool no_print = false, bool save = true)
    {
        time_t raw_time;
        tm time_info;
        (void)time(&raw_time);
        (void)localtime_s(&time_info, &raw_time);
        logger_base::date = std::to_string(time_info.tm_year + 1900) + "-" +
                            std::to_string(time_info.tm_mon + 1) + "-" +
                            std::to_string(time_info.tm_mday);
        logger_base::log_filename = "./log/" + logger_base::date + ".log";
        logger_base::today = time_info.tm_mday;
        _mkdir("./log");
    }
#ifdef DEBUG
#define debug(_message) logger_base::log<decltype(_message)>("DEBUG", __FUNCTION__, __FILE__, __LINE__, _message)
#else
#define debug(_message) logger_base::ignore<decltype(_message)>(_message)
#endif
#define info(_message) logger_base::log<decltype(_message)>("INFO", __FUNCTION__, __FILE__, __LINE__, _message)
#define warning(_message) logger_base::log<decltype(_message)>("WARNING", __FUNCTION__, __FILE__, __LINE__, _message)
#define error(_message) logger_base::log<decltype(_message)>("ERROR", __FUNCTION__, __FILE__, __LINE__, _message)
#define fatal(_message) logger_base::log<decltype(_message)>("FATAL", __FUNCTION__, __FILE__, __LINE__, _message)
}