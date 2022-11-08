#pragma once
#include <string>
#include <vector>

inline std::vector<std::string>* split_string(const std::string& string, const std::string& delimiter) {
    auto* result = new std::vector<std::string>();
    size_t from = 0, to = 0;
    while (std::string::npos != (to = string.find(delimiter, from))) {
        result->push_back(string.substr(from, to - from));
        from = to + delimiter.length();
    }
    result->push_back(string.substr(from, to));
    return result;
}
