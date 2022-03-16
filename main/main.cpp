#include "matmark/matmark.h"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
    auto file = std::ifstream{"README.md"};

    if (!file.is_open()) {
        std::cerr << "could not open file\n";
        std::terminate();
    }

    md2html(file, std::cout);
}
