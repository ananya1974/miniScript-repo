#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <sstream>

std::string read_input(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

#endif
