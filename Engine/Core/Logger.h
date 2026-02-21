#pragma once
#include <string>
#include <iostream>
#include <format>
#include <chrono>

namespace rex {

enum class LogLevel {
    Trace, Info, Warn, Error, Fatal
};

class Logger {
public:
    template<typename... Args>
    static void log(LogLevel level, std::string_view fmt, Args&&... args) {
        auto now = std::chrono::system_clock::now();
        std::string level_str = "";
        switch(level) {
            case LogLevel::Trace: level_str = "\033[90m[TRACE]\033[0m"; break;
            case LogLevel::Info:  level_str = "\033[32m[INFO ]\033[0m"; break;
            case LogLevel::Warn:  level_str = "\033[33m[WARN ]\033[0m"; break;
            case LogLevel::Error: level_str = "\033[31m[ERROR]\033[0m"; break;
            case LogLevel::Fatal: level_str = "\033[41m[FATAL]\033[0m"; break;
        }
        
        std::cout << std::format("{} {}\n", level_str, std::vformat(fmt, std::make_format_args(args...)));
    }

    template<typename... Args> static void trace(std::string_view f, Args&&... a) { log(LogLevel::Trace, f, std::forward<Args>(a)...); }
    template<typename... Args> static void info(std::string_view f, Args&&... a)  { log(LogLevel::Info, f, std::forward<Args>(a)...); }
    template<typename... Args> static void warn(std::string_view f, Args&&... a)  { log(LogLevel::Warn, f, std::forward<Args>(a)...); }
    template<typename... Args> static void error(std::string_view f, Args&&... a) { log(LogLevel::Error, f, std::forward<Args>(a)...); }
};

}
