#pragma once

#include <sstream>
#include <string>

inline std::string createLink(std::string file) {
    return "<p><a href=\"commonplace/" + file + "\">" + file + "</a></p>";
}

template <typename... Args>
std::string t(std::string tagName, const Args &...args) {
    auto ss = std::ostringstream{};
    ss << "<" << tagName << ">\n";
    ((ss << args << "\n"), ...);
    ss << "</" << tagName << ">\n";
    return ss.str();
}

template <typename... Args>
std::string tp(std::string tagName,
               std::string properties,
               const Args &...args) {
    auto ss = std::ostringstream{};
    ss << "<" << tagName << " " << properties << ">\n";
    ((ss << args << "\n"), ...);
    ss << "</" << tagName << ">\n";
    return ss.str();
}
