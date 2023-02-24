/*
Jordan Dehmel
jdehmel@outlook.com
github.com/jorbDehmel
2023 - present
MIT licence via mit-license.org held by author
*/

#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <map>
#include <stack>
#include <sstream>
#include <set>

#include "cpu2.hpp"
#include "stdMacros.hpp"
using namespace std;

typedef string short_assembly;
typedef pair<short, short> var;

class Assembler
{
public:
    Assembler();

    short_assembly assemble(const string &What);
    short_assembly encode(const short &What);

    map<string, string (*)(Assembler &Caller, const string &Arg)> macros;

    set<string> noArgs;

    map<string, short> instructions;
    map<string, short> functions;
    map<string, var> variables;

    stack<short> memStack;
    stack<short> ifPositions;

    short firstOpenAddress;
};

#endif
