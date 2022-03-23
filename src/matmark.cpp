#include "matmark/matmark.h"
#include "matmark/html.h"
#include "stringutitls.h"
#include <sstream>
#include <vector>

// More info here: https://www.markdownguide.org/basic-syntax/

namespace {

using Lines = std::vector<std::string>;

Lines splitStream(std::istream &in) {
    auto lines = std::vector<std::string>{};
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

void tables(std::vector<std::string> &lines) {
    if (lines.empty()) {
        return;
    }

    auto countBars = [](std::string_view line) {
        if (line.empty()) {
            return 0l;
        }
        if (line.front() != '|' || line.back() != '|') {
            return 0l;
        }
        return std::count(line.begin(), line.end(), '|');
    };

    auto splitContent = [](std::string_view line) {
        auto ret = std::vector<std::string>{};
        for (size_t prev = 0, f = 0;
             (f = line.find('|', prev + 1)) != std::string::npos;
             prev = f) {
            ret.emplace_back(line.substr(prev + 1, f - prev - 1));
        }
        return ret;
    };

    auto createHeader = [splitContent](std::string &line1, std::string &line2) {
        auto content = splitContent(line1);
        line1 = "<table><tr>";
        line2 = "";
        for (auto &c : content) {
            line1 += "<td>" + c + "</td>";
        }
        line1 += "</tr>";
    };

    auto isHeadingUnderline = [](std::string_view line) {
        // Todo: Do real check to verify pattern |--|--| etc
        return line.find('-') != std::string::npos;
    };

    auto createRow = [&](std::string &line) {
        auto content = splitContent(line);
        line = "<tr>";
        for (auto &c : content) {
            line += "<td>" + c + "</td>";
        }
        line += "</tr>";
    };

    auto createTable = [&lines, createHeader, createRow](size_t begin,
                                                         size_t end) {
        createHeader(lines.at(begin - 2), lines.at(begin - 1));
        for (size_t i = begin; i < end; ++i) {
            auto &line = lines.at(i);
            createRow(line);
        }
        lines.at(end - 1) += "</table>";
    };

    auto *previous = &lines.front();
    auto *previous2 = previous;
    bool isTable = false;

    int tableStart = 0;

    for (size_t i = 0; i < lines.size();
         previous2 = previous, previous = &lines.at(i), ++i) {
        auto &line = lines.at(i);
        auto bars = countBars(line);

        if (!bars) {
            if (isTable) {
                createTable(tableStart, i + 1);
                isTable = false;
            }
            continue;
        }

        if (bars != countBars(*previous) || bars != countBars(*previous2)) {
            continue;
        }

        isTable = true;

        if (isHeadingUnderline(*previous)) {
            tableStart = i;
        }
    }

    if (isTable) {
        createTable(tableStart, lines.size());
    }
}

std::string escape(std::string str) {
    str = replaceAll(std::move(str), "<", "&lt;");
    str = replaceAll(std::move(str), ">", "&gt;");
    return str;
}

void codeBlocks(std::vector<std::string> &lines) {
    for (size_t i = 0; i < lines.size(); ++i) {
        auto &line = lines.at(i);
        if (!startsWith(line, "```")) {
            continue;
        }

        ++i;
        auto begin = i;

        auto end = [&i, &lines] {
            for (; i < lines.size(); ++i) {
                auto &line = lines.at(i);
                line = escape(line);
                if (startsWith(line, "```")) {
                    return i;
                    break;
                }
            }
            return 0ul;
        }();

        if (end) {
            line = "";
            lines.at(begin).insert(0, "<code>");
            lines.at(i) = "</code>";
        }
    }
}

} // namespace

void md2html(std::istream &in,
             std::ostream &out,
             const MarkdownSettings &settings) {
    auto lines = splitStream(in);

    codeBlocks(lines);
    replaceHeaders(lines);
    horizontalLines(lines);
    checkBoxes(lines);
    rawUrls(lines);
    images(lines, settings);
    links(lines, settings);
    lists(lines);
    tables(lines);

    bool disableP = false;
    for (auto &line : lines) {
        if (!disableP && line.find("<code>") != std::string::npos) {
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
