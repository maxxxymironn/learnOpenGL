#pragma once

class Logger {
    Logger() = default;
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void print(const char* msg);
    
    void printErr(const char* msg);
};
