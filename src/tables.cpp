#include "tables.h"
#include <algorithm>

namespace matmark {

namespace {

auto countBars(std::string_view line) {
    if (line.empty()) {
        return 0l;
    }
    if (line.front() != '|' || line.back() != '|') {
        return 0l;
    }
    return std::count(line.begin(), line.end(), '|');
};

auto splitContent(std::string_view line) {
    auto ret = std::vector<std::string>{};
    for (size_t prev = 0, f = 0;
         (f = line.find('|', prev + 1)) != std::string::npos;
         prev = f) {
        ret.emplace_back(line.substr(prev + 1, f - prev - 1));
    }
    return ret;
};

auto createHeader(std::string &line1, std::string &line2) {
    auto content = splitContent(line1);
    line1 = "<table><tr>";
    line2 = "";
    for (auto &c : content) {
        line1 += "<td>" + c + "</td>";
    }
    line1 += "</tr>";
};

auto isHeadingUnderline(std::string_view line) {
    // Todo: Do real check to verify pattern |--|--| etc
    return line.find('-') != std::string::npos;
};

auto createRow(std::string &line) {
    auto content = splitContent(line);
    line = "<tr>";
    for (auto &c : content) {
        line += "<td>" + c + "</td>";
    }
    line += "</tr>";
};

auto createTable =
    [](std::vector<std::string> &lines, size_t begin, size_t end) {
        createHeader(lines.at(begin - 2), lines.at(begin - 1));
        for (size_t i = begin; i < end; ++i) {
            auto &line = lines.at(i);
            createRow(line);
        }
        lines.at(end - 1) += "</table>";
    };

} // namespace

void tables(std::vector<std::string> &lines) {
    if (lines.empty()) {
        return;
    }

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
                createTable(lines, tableStart, i + 1);
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
        createTable(lines, tableStart, lines.size());
    }
}

} // namespace matmark
