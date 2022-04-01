#include "matmark/matmark.h"
#include "code.h"
#include "lines.h"
#include "matmark/html.h"
#include "stringutitls.h"
#include "tables.h"
#include <sstream>
#include <vector>

// More info here: https://www.markdownguide.org/basic-syntax/

using namespace matmark;

namespace {

Lines splitStream(std::istream &in) {
    auto lines = Lines{};
    for (std::string line; getline(in, line);) {
        lines.push_back(line);
    }

    return lines;
}

/// Check so that there is only one type of character on line
/// skips spaces, example for headings
bool onlyChar(const std::string &s, char expected) {
    bool res = false;
    for (auto c : s) {
        if (std::isspace(c)) {
            continue;
        }
        if (c != expected) {
            return false;
        }
        res = true;
    }
    return res;
}

/// For header of type `# heading`
/// @return 0 if not header, level of header otherwise
int isHeading(const std::string line) {
    if (line.empty()) {
        return 0;
    }

    if (line.front() != '#') {
        return 0;
    }

    for (size_t i = 0; i < line.size(); ++i) {
        auto c = line.at(i);
        if (c == '#') {
            continue;
        }
        else if (isspace(c)) {
            return i;
        }
        else {
            return 0;
        }
    }

    return 0;
}

void replaceHeaders(Lines &lines) {
    for (size_t i = 1; i < lines.size(); ++i) {
        if (lines.at(i - 1).empty()) {
            continue;
        }

        if (onlyChar(lines.at(i), '=')) {
            lines.at(i - 1) = t("h1", lines.at(i - 1));
            lines.erase(lines.begin() + i);
        }
        else if (onlyChar(lines.at(i), '-')) {
            lines.at(i - 1) = t("h2", lines.at(i - 1));
            lines.erase(lines.begin() + i);
        }
    }

    for (auto &line : lines) {
        if (auto h = isHeading(line)) {
            line = t("h" + std::to_string(h), line.substr(h));
        }
    }
}

void horizontalLines(Lines &lines) {
    for (auto &line : lines) {
        if (onlyChar(line, '-') || onlyChar(line, '*') || onlyChar(line, '_')) {
            line = "<hr>";
        }
    }
}

void checkBoxes(Lines &lines) {
#ifdef UTF_CHECKBOX_STYLE
    auto unchecked = "☐";
    auto checked = "☑";
#else
    auto unchecked = "<input type=\"checkbox\" disabled>";
    auto checked = "<input type=\"checkbox\" checked disabled>";
#endif

    for (auto &line : lines) {
        if (line.rfind("- [ ]", 0) == 0) {
            line.replace(0, 5, unchecked);
        }
        else if (line.rfind("- [x]", 0) == 0) {
            line.replace(0, 5, checked);
        }
    }
}

void rawUrls(Lines &lines) {
    for (auto &line : lines) {
        if (line.rfind("http://", 0) == 0 || line.rfind("https://", 0) == 0) {
            line = tp("a", p("href", line), line);
        }
    }
}

void images(Lines &lines, const MarkdownSettings &settings) {
    // Obisdian syntax ![[stuff.png]]
    for (auto &line : lines) {
        if (startsWith(line, "![[") && endsWith(line, "]]")) {
            endsWith(line, "]]");
            auto name = line.substr(3, line.size() - 5);
            line = tp("img", p("src", settings.fileLookup(name).string()));
        }
    }
}

void links(Lines &lines, const MarkdownSettings &settings) {
    // Obisdian syntax [[stuff.md]]
    // But only full line versions
    for (auto &line : lines) {
        if (startsWith(line, "[[") && endsWith(line, "]]")) {
            endsWith(line, "]]");
            auto name = line.substr(2, line.size() - 4);
            line = tp("a", p("href", settings.linkLookup(name).string()), name);
        }
    }
}

void lists(Lines &lines) {
    auto indentList = std::vector<int>{};

    auto countSpaces = [](std::string_view line) -> int {
        if (line.empty()) {
            return 0ul;
        }
        size_t len = 0;
        for (auto c : line) {
            if (!isspace(c)) {
                break;
            }
            ++len;
        }
        return len;
    };

    auto currentDepth = [&indentList]() -> int {
        return indentList.empty() ? -1 : indentList.back();
    };

    auto haltList = [&indentList](std::string &line) {
        for ([[maybe_unused]] auto _ : indentList) {
            line += "</ul>";
        }
        indentList.clear();
    };

    auto previous = -1;

    for (size_t i = 0; i < lines.size(); ++i) {
        auto &line = lines.at(i);
        auto &previousLine = i ? lines.at(i - 1) : line;
        auto spaces = countSpaces(line);
        if (spaces + 1 == line.size()) {
            continue;
        }
        auto str = line.substr(spaces);
        if (str.empty()) {
            continue;
        }

        auto isList = (str.front() == '-' || str.front() == '*');

        auto current = currentDepth();

        if (isList) {
            line = str.substr(1);
            line.insert(0, "<li>");
            line += "</li>";
        }
        else {
            haltList(previousLine);

            continue;
        }

        if (spaces > current) {
            indentList.push_back(spaces);
            line.insert(0, "<ul>");
        }
        if (spaces < current) {
            if (!indentList.empty()) {
                indentList.pop_back();
                indentList.back() = spaces;
            }
            previousLine += "</ul>";
        }
    }

    haltList(lines.back());
}

} // namespace

void md2html(std::istream &in,
             std::ostream &out,
             const MarkdownSettings &settings) {
    auto lines = splitStream(in);

    matmark::codeBlocks(lines);
    matmark::inlineCode(lines);
    replaceHeaders(lines);
    horizontalLines(lines);
    checkBoxes(lines);
    rawUrls(lines);
    images(lines, settings);
    links(lines, settings);
    lists(lines);
    matmark::tables(lines);

    bool disableP = false;
    for (auto &line : lines) {
        if (!disableP &&
            line.find("<code class=\"block\"") != std::string::npos) {
            disableP = true;
        }
        else if (disableP && line.find("</code>") != std::string::npos) {
            disableP = false;
        }

        if (!disableP && line.find("<li>") == std::string::npos &&
            line.find("<td>") == std::string::npos) {
            out << "<p>" << line << "</p>\n";
        }
        else {
            out << line << "\n";
        }
    }
}
