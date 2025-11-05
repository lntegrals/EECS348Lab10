// EECS 348: Software Engineering I – Fall 2025
// Lab 10 – String-based calculator
//
// Program entry point.  This program prompts the user for the name
// of a text file containing test cases.  Each line of the file must
// consist of two whitespace‑separated strings representing numeric
// values in the format described in the lab specification.  For
// each pair of strings the program validates the inputs, performs
// addition if both are valid, and reports the result.  Any
// invalid numbers are reported and skipped.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "string_calculator.h"

int main() {
    std::cout << "Enter the path to the test case file: ";
    std::string filename;
    std::getline(std::cin, filename);
    if (filename.empty()) {
        std::cerr << "No file name provided.\n";
        return 1;
    }
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return 1;
    }
    std::string line;
    int lineNumber = 1;
    while (std::getline(infile, line)) {
        if (line.empty()) {
            // Skip blank lines to keep numbering consistent
            ++lineNumber;
            continue;
        }
        std::istringstream iss(line);
        std::string a, b;
        if (!(iss >> a >> b)) {
            std::cout << "Line " << lineNumber << ": Unable to parse two numbers\n";
            ++lineNumber;
            continue;
        }
        bool aValid = isValidDouble(a);
        bool bValid = isValidDouble(b);
        if (!aValid || !bValid) {
            // Report which value(s) are invalid
            if (!aValid && !bValid) {
                std::cout << "Line " << lineNumber << ": both inputs are invalid: " << a << ", " << b << "\n";
            } else if (!aValid) {
                std::cout << "Line " << lineNumber << ": invalid input: " << a << "\n";
            } else {
                std::cout << "Line " << lineNumber << ": invalid input: " << b << "\n";
            }
            ++lineNumber;
            continue;
        }
        // Perform addition and print result
        std::string result = add(a, b);
        std::cout << "Line " << lineNumber << ": " << a << " + " << b << " = " << result << "\n";
        ++lineNumber;
    }
    return 0;
}