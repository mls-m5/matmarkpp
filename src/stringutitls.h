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

// https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
inline std::string replaceAll(std::string str,
                              const std::string &from,
                              const std::string &to) {
    for (size_t start_pos = 0;
         (start_pos = str.find(from, start_pos)) != std::string::npos;) {
        str.replace(start_pos, from.length(), to);
        start_pos +=
            to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}
