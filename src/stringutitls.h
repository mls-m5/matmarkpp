#pragma once

#include <string>

template <typename T>
bool startsWith(const std::string &a, const T &b) {
    return a.rfind(b, 0) != std::string::npos;
}

bool endsWith(const std::string &stack, const std::string &needle) {
    return stack.find(needle, stack.size() - needle.size()) !=
           std::string::npos;
}
