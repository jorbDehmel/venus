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
extern bool safety;

struct var
{
    string type;
    short address;
    short size;
};

string handleScope(const string &Prefix, const string &VarName);

class Assembler
{
public:
    Assembler();

    short_assembly assemble(const string &What);
    short_assembly encode(const short &What);

    map<string, string (*)(Assembler &Caller, const string &Arg)> macros;
    map<string, short> types;

    set<string> noArgs;

    map<string, short> instructions;
    map<string, short> functions;
    map<string, var> variables;

    stack<short> memStack;
    stack<short> ifPositions;
    map<string, vector<var>> structs;

    short firstOpenAddress;
    string prefix;
};

#endif
