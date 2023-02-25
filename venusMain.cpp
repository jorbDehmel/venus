/*
Jordan Dehmel
jdehmel@outlook.com
github.com/jorbDehmel
2023 - present
MIT licence via mit-license.org held by author
*/

#include <iostream>
#include <fstream>
#include <chrono>
#include "src/cpu2.hpp"
#include "src/tags.hpp"
using namespace std;

struct options
{
    string inFile;
};

options parseArgs(const int argc, const char *argv[])
{
    options out;
    out.inFile = "";

    for (int i = 1; i < argc; i++)
    {
        string cur(argv[i]);

        if (cur[0] != '-')
        {
            out.inFile = cur;
        }
    }

    if (out.inFile == "")
    {
        throw runtime_error("No input file provided");
    }

    return out;
}

int main(const int argc, const char *argv[])
{
    options opts;
    try
    {
        opts = parseArgs(argc, argv);
    }
    catch (runtime_error &e)
    {
        cout << tags::red_bold
             << "Error parsing arguments.\n"
             << e.what() << '\n'
             << tags::reset;

        return 7;
    }

    string inputFileName = opts.inFile;

    // Look for .ter suffix on output
    if (inputFileName.size() < 4 || inputFileName.substr(inputFileName.size() - 4) != ".ven")
    {
        // Ensure it's not a venus-shebang file
        if (inputFileName.size() < 6 || inputFileName.substr(inputFileName.size() - 6) != ".vensh")
        {
            cout << tags::yellow_bold
                 << "Warning! The proper file suffix for ternary compiled files is .ven\n"
                 << tags::reset;
        }
    }

    ifstream in(inputFileName);

    if (!in.is_open())
    {
        cout << tags::red_bold
             << "Error: Could not open input file.\n"
             << tags::reset;
        return 2;
    }

    // Load source code to string (raw bytes)
    string code, line;
    while (getline(in, line))
    {
        // Allows shebangs
        if (line[0] != '#')
        {
            code += line + '\n';
        }
    }
    in.close();

    cpu2 c;
    c.loadProgram(code);

#ifdef TIMER
    auto begin = chrono::high_resolution_clock::now();
    while (c.doInstr() == 0)
    {
        // This comment to make this loop less ugly
    }
    auto end = chrono::high_resolution_clock::now();
    long int ellapsed = chrono::duration_cast<chrono::nanoseconds>(end - begin).count();
    cout << "Nanoseconds: " << ellapsed << '\n';
#else
    while (c.doInstr() == 0)
    {
        // This comment to make this loop less ugly
    }
#endif
    return 0;
}
