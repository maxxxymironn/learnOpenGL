#include "logger.hpp"

#include <iostream>

void Logger::print(const char* msg) {
    std::cout << "INFO: " << msg << "\n";
}

void Logger::printErr(const char* msg) {
    std::cerr << "ERROR: " << msg << "\n";
}
