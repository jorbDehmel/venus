/*
Jordan "Jorb" Dehmel
jdehmel@outlook.com
github.com/jorbDehmel
2023 - present
MIT licence via mit-license.org held by author
*/

#include "cpu2.hpp"

cpu2::cpu2()
{
    *instrPointer = 0;

    sectors[0] = mem;
    for (int i = 1; i < 32; i++)
    {
        sectors[i] = nullptr;
    }
    return;
}

cpu2::cpu2(unsigned short *Sectors[31])
{
    *instrPointer = 0;

    sectors[0] = mem;
    for (int i = 1; i < 32; i++)
    {
        sectors[i] = Sectors[i - 1];
    }
    return;
}

void cpu2::loadProgram(string &Program)
{
    for (int i = 0; i < (int)Program.size(); i++)
    {
        ((char *)(instructions + *instrPointer))[i] = Program[i];
    }

    return;
}

void cpu2::printInstr(const unsigned short &From, const unsigned short &HowMany) const
{
    for (int i = 0; i < HowMany; i++)
    {
        if (i % 3 == 0 && i != 0)
        {
            cout << '\n';
        }
        cout << instructions[From + i] << '\t';
    }
    cout << '\n';

    return;
}

int cpu2::doInstr()
{
    unsigned short *instruc, *addr, *lit;

    instruc = instructions + *instrPointer;
    addr = instructions + *instrPointer + 1;
    lit = instructions + *instrPointer + 2;

    char temp;
    switch (*instruc)
    {
    case kill:
        return -1;
        break;
    case put:
        curSector[*addr] = *lit;
        break;
    case cpy:
        curSector[*lit] = curSector[*addr];
        break;
    case incr:
        curSector[*addr] += *lit;
        break;
    case decr:
        curSector[*addr] -= *lit;
        break;
    case jump:
        if (*lit != 0)
        {
            *instrPointer += (*lit - 1) * 3;
        }
        else
        {
            *instrPointer = *addr - 3;
        }
        break;
    case jumpBack:
        if (*lit != 0)
        {
            *instrPointer -= (*lit + 1) * 3;
        }
        else
        {
            *instrPointer = *addr - 3;
        }
        break;
    case ifControl:
        if (*controlBuffer == 0)
        {
            jumpIf();
        }
        break;
    case endif:
        break;
    case andEqV:
        if (*controlBuffer != 0)
        {
            if (curSector[*addr] == curSector[*lit])
            {
                *controlBuffer = 128;
            }
            else
            {
                *controlBuffer = 0;
            }
        }
        break;
    case andNeqV:
        if (*controlBuffer != 0)
        {
            if (curSector[*addr] != curSector[*lit])
            {
                *controlBuffer = 128;
            }
            else
            {
                *controlBuffer = 0;
            }
        }
        break;
    case andLessV:
        if (*controlBuffer != 0)
        {
            if (curSector[*addr] < curSector[*lit])
            {
                *controlBuffer = 128;
            }
            else
            {
                *controlBuffer = 0;
            }
        }
        break;
    case andGreaterV:
        if (*controlBuffer != 0)
        {
            if (curSector[*addr] > curSector[*lit])
            {
                *controlBuffer = 128;
            }
            else
            {
                *controlBuffer = 0;
            }
        }
        break;
    case orEqV:
        if (*controlBuffer == 0)
        {
            if (curSector[*addr] == curSector[*lit])
            {
                *controlBuffer = 128;
            }
            else
            {
                *controlBuffer = 0;
            }
        }
        break;
    case orNeqV:
        if (*controlBuffer == 0)
        {
            if (curSector[*addr] != curSector[*lit])
            {
                *controlBuffer = 128;
            }
            else
            {
                *controlBuffer = 0;
            }
        }
        break;
    case orLessV:
        if (*controlBuffer == 0)
        {
            if (curSector[*addr] < curSector[*lit])
            {
                *controlBuffer = 128;
            }
            else
            {
                *controlBuffer = 0;
            }
        }
        break;
    case orGreaterV:
        if (*controlBuffer == 0)
        {
            if (curSector[*addr] > curSector[*lit])
            {
                *controlBuffer = 128;
            }
            else
            {
                *controlBuffer = 0;
            }
        }
        break;
    case out:
        for (int i = 0; i < *lit; i++)
        {
            *outStream << curSector[*addr + i] << '\n';
        }
        break;
    case inp:
        for (short i = 0; i < *lit; i++)
        {
            *inStream >> temp;
            curSector[*addr + i] = temp;
        }

        break;
    case sector:
        if (*addr != 0)
        {
            if (sectors[*addr % 32] == nullptr)
            {
                throw runtime_error("Cannot access invalid memory sector");
            }
            curSector = sectors[*addr % 32];
        }
        else
        {
            curSector = mem;
        }
        break;
    case addV:
        curSector[*addr] = curSector[*addr] + curSector[*lit];
        break;
    case subtractV:
        curSector[*addr] = curSector[*addr] - curSector[*lit];
        break;
    case multV:
        curSector[*addr] = curSector[*addr] * curSector[*lit];
        break;
    case divV:
        curSector[*addr] = curSector[*addr] / curSector[*lit];
        break;
    case modV:
        curSector[*addr] = curSector[*addr] % curSector[*lit];
        break;
    case ifNever:
        jumpIf();
        break;
    case outChar:
        for (int i = 0; i < *lit; i++)
        {
            *outStream << ((char *)(curSector + *addr + i))[0]
                       << ((char *)(curSector + *addr + i))[1];
        }
        break;

    default:
        *outStream << "Error during processing of instruction " << *instruc << '\n';
        throw runtime_error("Could not process invalid command");
        break;
    }

    *instrPointer += 3;

    return *instruc;
}

void cpu2::jumpIf()
{
    short numIfs = 1;

    while (numIfs != 0)
    {
        *instrPointer += 3;

        if (instructions[*instrPointer] == ifControl || instructions[*instrPointer] == ifNever)
        {
            numIfs++;
        }
        else if (instructions[*instrPointer] == endif)
        {
            numIfs--;
        }
    }

    return;
}
