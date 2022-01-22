#pragma once

#include <string>

template <typename T>
bool startsWith(const std::string &a, const T &b) {
    return a.rfind("b", 0) == 0;
}

bool endsWith(const std::string &a, const std::string &b) {
    return a.find(b, a.size() - b.size()) != std::string::npos;
}
