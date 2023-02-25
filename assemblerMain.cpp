/*
Jordan Dehmel
jdehmel@outlook.com
github.com/jorbDehmel
2023 - present
MIT licence via mit-license.org held by author
*/

#include "src/assembler.hpp"
#include "src/tags.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
using namespace std;

struct options
{
    string inFile;
    string outFile;
    string shebang;
};

options parseArgs(const int argc, const char *argv[])
{
    options out;
    out.inFile = "";
    out.outFile = "";
    out.shebang = "";

    for (int i = 1; i < argc; i++)
    {
        string cur(argv[i]);

        if (cur == "-sh")
        {
            if (i + 1 >= argc)
            {
                throw runtime_error("Error: -sh must be followed by an argument");
            }

            i++;
            out.shebang = argv[i];
        }
        else if (cur == "-o")
        {
            if (i + 1 >= argc)
            {
                throw runtime_error("-o must be followed by a file name");
            }

            out.outFile = argv[i + 1];
            i++;
        }
        else if (cur[0] != '-')
        {
            out.inFile = cur;
        }
    }

    if (out.inFile == "")
    {
        throw runtime_error("No input file provided");
    }
    else if (out.outFile == "")
    {
        throw runtime_error("No output file provided");
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
    string outputFileName = opts.outFile;

    // Look for .tasl suffix on input
    if (inputFileName.size() < 5 || inputFileName.substr(inputFileName.size() - 5) != ".tasl")
    {
        cout << tags::yellow_bold
             << "Warning! This assembler takes only .tasl files\n"
             << tags::reset;
    }

    // Look for .ter suffix on output
    if (outputFileName.size() < 4 || outputFileName.substr(outputFileName.size() - 4) != ".ven")
    {
        // Ensure it's not a venus-shebang file
        if (outputFileName.size() < 6 || outputFileName.substr(outputFileName.size() - 6) != ".vensh")
        {
            cout << tags::yellow_bold
                 << "Warning! The proper file suffix for ternary compiled files is .ven\n"
                 << tags::reset;
        }
    }

    ifstream in(inputFileName);

    if (!in.is_open())
    {
        in.close();

        cout << tags::red_bold
             << "Error: Could not open input file.\n"
             << tags::reset;
        return 2;
    }

    // Get source code to assemble
    string toAssemble, line;
    while (getline(in, line))
    {
        toAssemble += line + '\n';
    }
    in.close();

    cout << tags::green_bold
         << "Assembling...\n"
         << tags::reset;

    // Assemble
    short_assembly assembled;
    try
    {
        Assembler a;
        assembled = a.assemble(toAssemble);
    }
    catch (macro_error &e)
    {
        cout << tags::red_bold
             << "Error: Macro failed.\n"
             << e.what() << '\n'
             << tags::reset;
        return 6;
    }
    catch (runtime_error &e)
    {
        cout << tags::red_bold
             << "Error: Assembly failed.\n"
             << e.what() << '\n'
             << tags::reset;
        return 4;
    }
    catch (...)
    {
        cout << tags::red_bold
             << "Error: Fatal error occured.\n"
             << tags::reset;
        return 5;
    }

    cout << tags::green_bold
         << "Assembled.\n"
         << tags::reset;

    // Write to file
    ofstream out(outputFileName);

    if (!out.is_open())
    {
        out.close();

        cout << tags::red_bold
             << "Error: Could not open output file.\n"
             << tags::reset;
        return 3;
    }

    if (opts.shebang != "")
    {
        out << "#!" << opts.shebang << '\n';
    }

    out << assembled;
    out.close();

    return 0;
}
