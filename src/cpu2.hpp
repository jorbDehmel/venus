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

constexpr int power(int a, int b)
{
    int out = 1;
    for (int i = 0; i < b; i++)
    {
        out *= a;
    }

    return out;
}

#define MEMSIZE power(2, 16)
#define INSTRSIZE MEMSIZE

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
    outChar,
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
    cpu2(unsigned short *Sectors[31]);

    void loadProgram(string &Program);
    int doInstr();

    void printInstr(const unsigned short &From, const unsigned short &HowMany) const;

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

    unsigned short mem[MEMSIZE + INSTRSIZE];
    unsigned short *instructions = mem + MEMSIZE;
    unsigned short *sectors[32];

    unsigned short *curSector = mem;
    unsigned short *instrPointer = mem;
    unsigned short *controlBuffer = mem + 1;
    unsigned short *sectorBuffer = mem + 2;
};

#endif
