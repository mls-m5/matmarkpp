#include "matmark/matmark.h"
#include <fstream>
#include <iostream>

void open(std::filesystem::path path);

auto header = R"_(

<html>
<head>
<style>
code {
   white-space: pre;
   background: rgba(0,0,100,.3);
}

code.block {
   display: block;
}

th {
   background: rgba(0,0,100,.3);
   padding: 4px;
}

td {
   padding: 4px;
}

table {
   border-spacing:0;
   border: 1px solid black;
}


</style>
</head>
</body>
)_";

int main(int argc, char *argv[]) {
    auto file = std::ifstream{"README.md"};

    if (!file.is_open()) {
        std::cerr << "could not open file\n";
        std::terminate();
    }

    {
        auto ofile = std::ofstream{"testoutput.html"};

        ofile << header;
        md2html(file, ofile);
    }

    open("testoutput.html");
}

#ifdef __unix__
void open(std::filesystem::path path) {
    std::system(("xdg-open " + path.string()).c_str());
}

#else
void open(std::filesystem::path path) {
    std::system(
        ("rundll32 url.dll,FileProtocolHandler " + path.string()).c_str());
}

#endif
