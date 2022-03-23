#pragma once

#include <string>
#include <vector>

template <typename T>
inline bool startsWith(const std::string &a, const T &b) {
    return a.rfind(b, 0) != std::string::npos;
}

inline bool endsWith(const std::string &stack, const std::string &needle) {
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

inline std::string escape(std::string str) {
    str = replaceAll(std::move(str), "<", "&lt;");
    str = replaceAll(std::move(str), ">", "&gt;");
    return str;
}

inline std::vector<std::string> split(std::string_view str, char delim) {
    auto ret = std::vector<std::string>{};

    for (size_t f; (f = str.find(delim)) != std::string::npos;
         str = str.substr(f + 1)) {
        ret.emplace_back(str.substr(0, f));
    }

    if (!str.empty()) {
        ret.emplace_back(str);
    }

    return ret;
}
