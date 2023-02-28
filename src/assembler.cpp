/*
Jordan Dehmel
jdehmel@outlook.com
github.com/jorbDehmel
2023 - present
MIT licence via mit-license.org held by author
*/

#include "assembler.hpp"

bool safety = true;

Assembler::Assembler()
{
    firstOpenAddress = 32;

    // Build instruction table
    instructions["kill"] = kill;
    instructions["put"] = put;
    instructions["cpy"] = cpy;

    instructions["incr"] = incr;
    instructions["decr"] = decr;
    instructions["jump"] = jump;

    instructions["jumpBack"] = jumpBack;
    instructions["ifControl"] = ifControl;
    instructions["endif"] = endif;

    instructions["andEqV"] = andEqV;
    instructions["andNeqV"] = andNeqV;
    instructions["andLessV"] = andLessV;

    instructions["andGreaterV"] = andGreaterV;
    instructions["orEqV"] = orEqV;
    instructions["orNeqV"] = orNeqV;

    instructions["orLessV"] = orLessV;
    instructions["orGreaterV"] = orGreaterV;
    instructions["out"] = out;

    instructions["inp"] = inp;
    instructions["sector"] = sector;
    instructions["addV"] = addV;

    instructions["subtractV"] = subtractV;
    instructions["multV"] = multV;
    instructions["divV"] = divV;

    instructions["modV"] = modV;
    instructions["ifNever"] = ifNever;
    instructions["outChar"] = outChar;

    // Buffer address variables (non-stack)
    variables["INSTR"] = var{"u16", 0, 1};
    variables["CONT"] = var{"u16", 1, 1};
    variables["RET"] = var{"u16", 2, 1};

    // Convenience variables
    variables["ENDL"] = var{"i8", '\n', 1};
    variables["SPACE"] = var{"i8", ' ', 1};
    variables["TAB"] = var{"i8", '\t', 1};

    // No argument instructions
    noArgs.insert("kill");
    noArgs.insert("ifControl");
    noArgs.insert("ifNever");
    noArgs.insert("endif");

    // Include standard macros
    macros["print"] = print;
    macros["println"] = println;
    macros["req"] = req;

    macros["cpy"] = copyVars;
    macros["zero"] = zeroOut;

    // Include standard types
    types["u16"] = 1;
    types["u32"] = 2;
    types["u64"] = 4;
    types["u128"] = 8;

    types["f32"] = 2;
    types["f64"] = 4;

    return;
}

short_assembly Assembler::encode(const short &What)
{
    short_assembly out;

    out += ((unsigned char *)&What)[0];
    out += ((unsigned char *)&What)[1];

    return out;
}

string handleScope(const string &Prefix, const string &VarName)
{
    if (VarName[0] == '+')
    {
        string tempname = Prefix + VarName;
        for (int i = 0; i < (int)VarName.size() && VarName[i] == '+'; i++)
        {
            if (tempname[0] != '+')
            {
                throw runtime_error("Base scope has no superscope");
            }

            tempname = tempname.substr(2);
        }

        return tempname;
    }
    else if (VarName[0] == '*')
    {
        return VarName.substr(1);
    }
    else
    {
        return Prefix + VarName;
    }
}

string Assembler::variablePass(const string &Source)
{
    cout << "Variable pass...\n";
    stringstream prePass, postPass;
    prePass << Source;

    string ignoreLines = "[/=";

    string line;
    while (getline(prePass, line))
    {
        while (line[0] == ' ' || line[0] == '\t')
        {
            line = line.substr(1);
        }

        if (ignoreLines.find(line[0]) == string::npos)
        {
            postPass << line << '\n';
        }

        if (line[0] == '.')
        {
            stringstream extract;
            extract << line.substr(1);

            string type;
            string name;
            extract >> name >> type;

            name = prefix + name;

            try
            {
                unsigned short sizeShort = stoi(type);
                variables[name] = var{"NULL_TYPE", 0, sizeShort};
            }
            catch (...)
            {
                if (types.count(type) != 0)
                {
                    variables[name] = var{type, 0, types[type]};

                    if (structs.count(type) != 0)
                    {
                        for (auto member : structs[type])
                        {
                            variables[prefix + name + "." + member.first] =
                                var{member.second.type, (unsigned short)(member.second.address + firstOpenAddress), member.second.size};
                        }
                    }
                }
                else
                {
                    throw runtime_error("Invalid type '" + type + "'");
                }
            }
        }
        else if (line[0] == '=')
        {
            // Alias
            string from, to;
            int index = 1;
            while (line[index] != ' ')
            {
                from += line[index];
                index++;
            }
            to = line.substr(index + 1);

            if (variables.count(to) == 0)
            {
                throw runtime_error("Cannot alias to a non-existant variable '" + to + "'");
            }

            variables[from] = var{variables[to].type, variables[to].address, variables[to].size};
        }
        else if (line[0] == '[')
        {
            // Struct definition

            // [foobar
            //      .a u16
            //      .b i32
            //      .c 16
            // ]

            // .VAR foobar
            // `VAR 19

            string structName = line.substr(1);

            structs[structName] = vector<pair<string, var>>();
            short runningSize = 0;

            string name, type;

            // Get entries and their types
            do
            {
                prePass >> name;
                if (name[0] == ']')
                {
                    break;
                }

                prePass >> type;

                short size;
                var varToAdd{"", 0, 0};
                try
                {
                    size = stoi(type);
                    varToAdd.address = runningSize;
                    varToAdd.size = size;
                    varToAdd.type = "NULL_TYPE";

                    runningSize += size;
                }
                catch (...)
                {
                    if (types.count(type) != 0)
                    {
                        size = types[type];
                        varToAdd.address = runningSize;
                        varToAdd.size = size;
                        varToAdd.type = type;

                        runningSize += size;
                    }
                    else
                    {
                        throw runtime_error("Invalid type '" + type + "'");
                    }
                }

                structs[structName].push_back(pair<string, var>(name, varToAdd));
            } while (name[0] != ']');

            types[structName] = runningSize;
        }
        else if (line[0] == ']')
        {
            // Struct ending
            throw runtime_error("Unmatched struct definition end bracket (']')");
        }
    }

    if (options.count("dump_vars") != 0)
    {
        cout << "Variable dump:\n";
        for (auto v : variables)
        {
            cout << "Name: " << v.first << " Size:" << v.second.size << " Address: " << v.second.address << " Type: " << v.second.type << '\n';
        }

        cout << "Struct definition dump:\n";
        for (auto s : structs)
        {
            cout << "Name: " << s.first << '\n';

            for (auto member : s.second)
            {
                cout << "\tMember name: " << member.first << '\n'
                     << "\tSize: " << member.second.size << '\n'
                     << "\tAddress: " << member.second.address << '\n'
                     << "\tType: " << member.second.type << "\n\n";
            }
        }
    }

    return postPass.str();
}

short_assembly Assembler::assemble(const string &sourceIn)
{
    string What = variablePass(sourceIn);
    string postMacro = What;
    string instr;
    prefix = "";

    int macroPass = 0;
    do
    {
        stringstream preMacro;
        preMacro << postMacro;
        postMacro = "";

        // Macro replacement pass
        cout << "Handling macros (pass " << macroPass++ << ")\n";
        while (!preMacro.eof())
        {
            getline(preMacro, instr);

            if (instr[0] == '#')
            {
                string name;
                for (int i = 1; i < (int)instr.size() && instr[i] != ' '; i++)
                {
                    name += instr[i];
                }

                if (macros.count(name) != 0)
                {
                    string arg = instr.substr(name.size() + 2);
                    postMacro += macros[name](*this, arg);
                }
                else
                {
                    throw runtime_error("Unknown macro '" + instr.substr(1) + "'");
                }
            }

            else if (instr[0] == '{')
            {
                prefix = "+" + prefix;
                postMacro += instr + '\n';
            }
            else if (instr[0] == '}')
            {
                prefix = prefix.substr(1);
                postMacro += instr + '\n';
            }
            else
            {
                postMacro += instr + '\n';
            }
        }
    } while (postMacro.find('#') != string::npos);

    // Compilation pass
    stringstream code;
    code << postMacro;

    cout << "Assembling...\n";

    string out;
    prefix = "";

    while (!code.eof())
    {
        code >> instr;

        if (instructions.count(instr) != 0)
        {
            // Instruction
            out += encode(instructions[instr]);

            // Take care of no argument instructions
            if (noArgs.count(instr) != 0)
            {
                out += encode(short(0)) + encode(short(0));
            }
        }
        else if (functions.count(instr) != 0)
        {
            // Function
            out += encode(functions[instr]);
        }
        else if (variables.count(handleScope(prefix, instr)) != 0)
        {
            // Variable (current scope)
            out += encode(variables[handleScope(prefix, instr)].address);
        }
        else if (instr[0] == '^')
        {
            string arg = handleScope(prefix, instr.substr(1));

            string name;
            int by;

            int ind = 0;
            while (arg[ind] != '.')
            {
                name += arg[ind];
                ind++;

                if (ind >= (int)arg.size())
                {
                    throw runtime_error("Invalid usage of pointer arithmatic operator");
                }
            }

            by = stoi(arg.substr(ind + 1));

            // Pointer arithmatic
            if (variables.count(name) == 0)
            {
                throw runtime_error("Unknown variable '" + name + "'");
            }
            else
            {
                if (by >= 0)
                {
                    out += encode(variables[name].address + short(by));
                }
                else
                {
                    out += encode(variables[name].address - short(-by));
                }
            }
        }
        else if (instr[0] == '$')
        {
            string name = handleScope(prefix, instr.substr(1));
            if (variables.count(name) == 0)
            {
                throw runtime_error("No variable '" + name + "' found.");
            }

            out += encode(variables[name].size);
        }
        else if (instr[0] == '.')
        {
            // Variable declaration
            string type;
            code >> type;

            unsigned short size;
            try
            {
                size = stoi(type);
                variables[prefix + instr.substr(1)] = var{"NULL_TYPE", firstOpenAddress, size};
            }
            catch (...)
            {
                if (types.count(type) != 0)
                {
                    size = types[type];
                    variables[prefix + instr.substr(1)] = var{type, firstOpenAddress, size};

                    if (structs.count(type) != 0)
                    {
                        for (auto member : structs[type])
                        {
                            variables[prefix + instr.substr(1) + "." + member.first] =
                                var{member.second.type, (unsigned short)(member.second.address + firstOpenAddress), member.second.size};
                        }
                    }
                }
                else
                {
                    throw runtime_error("Invalid type '" + type + "'");
                }
            }

            if ((int)firstOpenAddress + size > MEMSIZE)
            {
                throw runtime_error("Creation of variable '" + instr.substr(1) + "' would exceed memory limit");
            }

            memStack.push(firstOpenAddress);
            firstOpenAddress += size;
        }
        else if (instr[0] == '~')
        {
            // Stack pop
            if (variables.count(prefix + instr.substr(1)) == 0 || memStack.top() != variables[prefix + instr.substr(1)].address)
            {
                throw runtime_error("Cannot pop a variable which is not on the top of the stack");
            }

            short addressToRemove = variables[prefix + instr.substr(1)].address;
            variables.erase(prefix + instr.substr(1));

            set<string> toRemove;
            for (auto v : variables)
            {
                // Remove any aliases
                if (v.second.address == addressToRemove)
                {
                    toRemove.insert(v.first);
                }
                // Remove any struct member aliases
                else if (v.first.substr(0, (prefix.size() + instr.size() - 1)) == prefix + instr.substr(1))
                {
                    toRemove.insert(v.first);
                }
            }
            for (auto r : toRemove)
            {
                variables.erase(r);
            }

            firstOpenAddress = memStack.top();
            memStack.pop();
        }
        else if (instr[0] == '_')
        {
            // Hex literal
            out += encode(stoi(instr.substr(1), 0, 16));
        }
        else if (instr[0] == '-')
        {
            short toInsert((short)instr[1]);
            out += encode(toInsert);
        }
        else if (instr[0] == '{')
        {
            // Call CPU's integrated function jumper
            out += encode(ifNever) + encode(0) + encode(0);

            string name = instr.substr(1);
            functions[name] = (out.size()) / 2;
            prefix = "+" + prefix;

            cout << "Started function " << name << " at " << functions[name] << '\n';
        }
        else if (instr[0] == '}')
        {
            // End scope
            if (prefix == "")
            {
                throw runtime_error("Error: Cannot end global scope");
            }

            cout << "Ended function\n";

            prefix = prefix.substr(1);

            // End CPU's integrated function jumper
            out += encode(endif) + encode(short(0)) + encode(short(0));
        }
        else if (instr[0] == '!')
        {
            // Call function
            string name = instr.substr(1);
            if (name == "return")
            {
                vector<short> toInsert = {
                    cpy, RET, INSTR};
                for (auto t : toInsert)
                {
                    out += encode(t);
                }
            }
            else if (functions.count(name) == 0)
            {
                throw runtime_error("Error: Undeclared function " + name);
            }
            else
            {
                // .OLD_RET 1 /unnamed compile-time var
                short oldRet = firstOpenAddress;
                memStack.push(firstOpenAddress);
                firstOpenAddress += 1; // since size is 1 short

                // cpy RET OLD_RET
                // cpy INSTR RET
                // incr RET 6
                vector<short> toInsert = {
                    cpy, RET, oldRet,
                    cpy, INSTR, RET,
                    incr, RET, 6};
                for (auto t : toInsert)
                {
                    out += encode(t);
                }

                // Jump to fn command
                out += encode(short(jump));
                out += encode(functions[name]);
                out += encode(short(0));

                // cpy OLD_RET RET
                toInsert = {
                    cpy, oldRet, short(2)};
                for (auto t : toInsert)
                {
                    out += encode(t);
                }

                // ~OLD_RET
                firstOpenAddress = memStack.top();
                memStack.pop();
            }
        }
        else if (instr[0] == '`')
        {
            // Variable type/size assertion

            // `VAR 2 asserts VAR is of size 2 bytes
            // `VAR int asserts VAR is of type int
            // `VAR $int asserts VAR is of the same size as an int (pointer castable)
            // Compilation fails if these conditions are not met

            string type;
            code >> type;

            string name = handleScope(prefix, instr.substr(1));

            if (type[0] == '$')
            {
                if (types.count(type.substr(1)) == 0)
                {
                    throw runtime_error("Type '" + type.substr(1) + "' does not exist");
                }
                else
                {
                    if (types[type.substr(1)] != variables[name].size)
                    {
                        throw runtime_error("Variable '" + name +
                                            "' is required to have a size of " + to_string(types[type.substr(1)]) +
                                            " but has a size of " + to_string(variables[name].size));
                    }
                }
            }
            else if (types.count(type) == 0)
            {
                throw runtime_error("Type '" + type + "' does not exist");
            }
            else if (variables.count(name) == 0)
            {
                throw runtime_error("Variable '" + name + "' does not exist");
            }
            else if (variables[name].type != type)
            {
                throw runtime_error("Variable '" + name +
                                    "' is required to be of type '" + type +
                                    "' but is of type '" + variables[name].type + "'");
            }
        }
        else if (instr[0] == '[')
        {
            string symb;
            do
            {
                code >> symb;
            } while (symb[0] != ']');
        }
        else if (instr[0] == '/')
        {
            // Comment
            string garbage;
            getline(code, garbage);
        }
        else
        {
            // Decimal literal
            try
            {
                out += encode(short(stoi(instr)));
            }
            catch (invalid_argument &e)
            {
                throw runtime_error("Invalid symbol '" + prefix + instr + "'");
            }
        }
    }

    return out;
}
