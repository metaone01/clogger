#pragma once
#include <cstdlib>
#include <ctime>
#include <direct.h>
#include <fstream>
#include <iostream>
#include <string>
namespace logger_base
{

    static std::string date;
    static std::string log_filename;
    static std::ofstream log_file;
    static int today;
    static bool no_print = false;
    static bool save = true;

    static void log(const std::string &log_name,
                    const std::string &CALLER,
                    const std::string &FILE,
                    const unsigned int LINE,
                    std::string _message)
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
            std::cout << time << " [" << log_name << "] <" << CALLER << "> in " << FILE << ", line " << LINE << ": " << _message << std::endl;
        }
        if (save)
        {
            log_file.open(log_filename, std::ios_base::app);
            log_file << _time << " [" << log_name << "] <" << CALLER << "> in " << FILE << ", line " << LINE << ": " << _message << std::endl;
            log_file.close();
        }
    }
}
namespace logger
{
    static bool DEBUG = false;

    static void init(bool debug = false, bool no_print = false, bool save = true)
    {
        logger::DEBUG = debug;
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
#define debug(_message) \
    if (logger::DEBUG)  \
    logger_base::log("DEBUG", __FUNCTION__, __FILE__, __LINE__, _message)
#define info(_message) logger_base::log("INFO", __FUNCTION__, __FILE__, __LINE__, _message)
#define warning(_message) logger_base::log("WARNING", __FUNCTION__, __FILE__, __LINE__, _message)
#define error(_message) logger_base::log("ERROR", __FUNCTION__, __FILE__, __LINE__, _message)
#define fatal(_message) logger_base::log("FATAL", __FUNCTION__, __FILE__, __LINE__, _message)
}