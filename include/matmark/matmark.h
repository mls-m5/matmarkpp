#pragma once

//#include "index.h"
#include <filesystem>
#include <functional>
#include <string>

struct MarkdownSettings {
    // Special specify if there should be some special way to find certain files
    std::function<std::filesystem::path(std::filesystem::path)> fileLookup =
        [](auto &&path) { return path; };

    std::function<std::filesystem::path(std::filesystem::path)> linkLookup =
        [](auto &&path) { return path; };
};

void md2html(std::istream &in,
             std::ostream &out,
             const MarkdownSettings &settings = MarkdownSettings{});
