/*
Jordan Dehmel
jdehmel@outlook.com
github.com/jorbDehmel
2023 - present
MIT licence via mit-license.org held by author
*/

#include "stdMacros.hpp"

string print(Assembler &Caller, const string &Arg)
{
    string out;

    // Determine size to print
    int size = Arg.size();

    // Allocate appropriately sized variable
    out += ".__PRINT_TEMP " + to_string(size) + "\n";

    // Write to variable tryte by tryte
    for (int i = 0; i < size; i++)
    {
        out += "put ^__PRINT_TEMP." + to_string(i) + " ";

        if (Arg[i] == ' ')
        {
            out += "*SPACE\n";
        }
        else if (Arg[i] == '\t')
        {
            out += "*TAB\n";
        }
        else
        {
            out += string("-") + Arg[i] + "\n";
        }
    }

    // Out by the size of the variable
    out += "out __PRINT_TEMP " + to_string(size) + "\n";

    // Destruct the variable
    out += "~__PRINT_TEMP\n";

    return out;
}

string println(Assembler &Caller, const string &Arg)
{
    string out;

    // Determine size to print
    int size = Arg.size() + 1;

    // Allocate appropriately sized variable
    out += ".__PRINT_TEMP " + to_string(size) + "\n";

    // Write to variable tryte by tryte
    for (int i = 0; i + 1 < size; i++)
    {
        out += "put ^__PRINT_TEMP." + to_string(i) + " ";

        if (Arg[i] == ' ')
        {
            out += "*SPACE\n";
        }
        else if (Arg[i] == '\t')
        {
            out += "*TAB\n";
        }
        else
        {
            out += string("-") + Arg[i] + "\n";
        }
    }
    out += "put ^__PRINT_TEMP." + to_string(size - 1) + " *ENDL\n";

    // Out by the size of the variable
    out += "out __PRINT_TEMP " + to_string(size) + "\n";

    // Destruct the variable
    out += "~__PRINT_TEMP\n";

    return out;
}

string req(Assembler &Caller, const string &Arg)
{
    string out;

    vector<string> vars;
    string temp;
    for (int i = 0; i < (int)Arg.size(); i++)
    {
        if (Arg[i] == ' ')
        {
            vars.push_back(temp);
            temp = "";
        }
        else
        {
            temp += Arg[i];
        }
    }
    vars.push_back(temp);

    for (auto v : vars)
    {
        out += "incr " + v + " 0\n";
    }

    return out;
}

string copyVars(Assembler &Caller, const string &Arg)
{
    // Parse args
    string ARG_1, ARG_2;
    int ind = 0;

    while (Arg[ind] != ' ')
    {
        ARG_1 += Arg[ind];
        ind++;
    }
    ARG_2 = Arg.substr(ind + 1);

    // Assure safety
    if (Caller.variables[ARG_1].size != Caller.variables[ARG_2].size)
    {
        throw macro_error("Cannot copy: Unequal variable sizes");
    }

    string out;

    int size = (int)Caller.variables[ARG_1].size;
    for (int i = 0; i < size; i++)
    {
        out += "cpy ^" + ARG_1 + "." + to_string(i) + " ^" + ARG_2 + "." + to_string(i) + "\n";
    }

    return out;
}

string zeroOut(Assembler &Caller, const string &Arg)
{
    string out;
    string name = handleScope(Caller.prefix, Arg);

    if (Caller.variables.count(name) == 0)
    {
        cout << "Var dump:\n";
        for (auto v : Caller.variables)
        {
            cout << '\t' << v.first << '\n';
        }

        throw macro_error("Invalid variable '" + name + "' could not be zeroed");
    }

    int size = (int)Caller.variables[name].size;
    for (int i = 0; i < size; i++)
    {
        out += "put ^" + name + "." + to_string(i) + " 0\n";
    }

    return out;
}
