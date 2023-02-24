/*
Jordan Dehmel
jdehmel@outlook.com
github.com/jorbDehmel
2023 - present
MIT licence via mit-license.org held by author
*/

#ifndef CPU2_HPP
#define CPU2_HPP

#include <vector>
#include <chrono>
#include <map>
#include <iostream>
using namespace std;

#define MEMSIZE (int)19'683
#define INSTRSIZE 19'683

// Note: trytes internally store their data as 16-bit binary integers.
// Thus, the following enumeration is valid for direct casting to trytes.
enum instr
{
    kill = 0,
    put,
    cpy,
    incr,
    decr,
    jump,
    jumpBack,
    ifControl,
    endif,
    andEqV,
    andNeqV,
    andLessV,
    andGreaterV,
    orEqV,
    orNeqV,
    orLessV,
    orGreaterV,
    out,
    inp,
    sector,
    addV,
    subtractV,
    multV,
    divV,
    modV,
    ifNever,
};

enum buffers
{
    INSTR = 0,
    CONT = 1,
    RET = 2
};

class cpu2
{
public:
    cpu2();
    cpu2(short *Sectors[31]);

    void loadProgram(string &Program);
    int doInstr();

    void printInstr(const short &From, const int &HowMany) const;

    void useStream(ostream &Stream)
    {
        outStream = &Stream;
        return;
    }

    void useStream(istream &Stream)
    {
        inStream = &Stream;
        return;
    }

protected:
    void jumpIf();

    ostream *outStream = &cout;
    istream *inStream = &cin;

    short mem[MEMSIZE + INSTRSIZE];
    short *sectors[27];

    short *curSector = mem;
    short *instrPointer = mem;
    short *controlBuffer = mem + 1;
    short *sectorBuffer = mem + 2;
};

#endif
