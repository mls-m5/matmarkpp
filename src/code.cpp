#include "code.h"
#include "stringutitls.h"

namespace matmark {

void codeBlocks(Lines &lines) {
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
            return size_t{0};
        }();

        if (end) {
            line = "";
            lines.at(begin).insert(0, "<code class=\"block\">");
            lines.at(i) = "</code>";
        }
    }
}

namespace {

void inlineCode(std::string &line) {
    if (line.find('`') == std::string::npos) {
        return;
    }

    auto parts = split(line, '`');
    line.clear();

    bool isCode = false;
    for (auto &part : parts) {
        if (isCode) {
            line += "<code class=\"inline\">" + std::string{part} + "</code>";
        }
        else {
            line += std::string{part};
        }
        isCode = !isCode;
    }
}

} // namespace

void inlineCode(Lines &lines) {
    for (auto &line : lines) {
        inlineCode(line);
    }
}

} // namespace matmark
