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
#include "tags.hpp"
using namespace std;

typedef string short_assembly;
extern bool safety;

struct var
{
    string type;
    unsigned short address;
    unsigned short size;
};

string handleScope(const string &Prefix, const string &VarName);

class Assembler
{
public:
    Assembler();

    set<string> options;

    short_assembly assemble(const string &What);
    short_assembly encode(const short &What);
    string variablePass(const string &Source);

    map<string, string (*)(Assembler &Caller, const string &Arg)> macros;
    map<string, unsigned short> types;

    set<string> noArgs;

    map<string, unsigned short> instructions;
    map<string, unsigned short> functions;
    map<string, var> variables;

    stack<unsigned short> memStack;
    map<string, vector<pair<string, var>>> structs;

    unsigned short firstOpenAddress;
    string prefix;
};

#endif
