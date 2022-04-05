#pragma once

#include <filesystem>
#include <functional>
#include <string>

namespace matmark {

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

//! Utility functions
//! -----------------------

/// For header of type `# heading`
/// @return 0 if not header, level of header otherwise
int isHeading(std::string_view line);

//! Return 0 if no heading and number of heading otherwise
int isHeading(std::string_view lineBefore, std::string lineAfter);

} // namespace matmark
