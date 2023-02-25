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
    *instrPointer = 2 * MEMSIZE / 3;

    sectors[0] = mem;
    for (int i = 1; i < 27; i++)
    {
        sectors[i] = nullptr;
    }
    return;
}

cpu2::cpu2(short *Sectors[31])
{
    *instrPointer = 2 * MEMSIZE / 3;

    sectors[0] = mem;
    for (int i = 1; i < 27; i++)
    {
        sectors[i] = Sectors[i - 1];
    }
    return;
}

void cpu2::loadProgram(string &Program)
{
    for (int i = 0; i < (int)Program.size(); i++)
    {
        ((char *)(mem + *instrPointer))[i] = Program[i];
    }
    return;
}

void cpu2::printInstr(const short &From, const int &HowMany) const
{
    for (int i = 0; i < HowMany; i++)
    {
        if (i % 3 == 0 && i != 0)
        {
            cout << '\n';
        }
        cout << mem[From + short(i)] << '\t';
    }
    cout << '\n';

    return;
}

int cpu2::doInstr()
{
#ifdef TIMER
    auto start = chrono::high_resolution_clock::now();
#endif

    short *instruc, *addr, *lit;

    instruc = mem + *instrPointer;
    addr = mem + *instrPointer + 1;
    lit = mem + *instrPointer + 2;

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
        if (*lit != short(0))
        {
            *instrPointer += (*lit - short(1)) * short(3);
        }
        else
        {
            *instrPointer = *addr - short(3);
        }
        break;
    case jumpBack:
        if (*lit != short(0))
        {
            *instrPointer -= (*lit + short(1)) * short(3);
        }
        else
        {
            *instrPointer = *addr - short(3);
        }
        break;
    case ifControl:
        if (*controlBuffer == short(0))
        {
            jumpIf();
        }
        break;
    case endif:
        break;
    case andEqV:
        if (*controlBuffer != short(0))
        {
            if (curSector[*addr] == curSector[*lit])
            {
                *controlBuffer = short(19'682);
            }
            else
            {
                *controlBuffer = short(0);
            }
        }
        break;
    case andNeqV:
        if (*controlBuffer != short(0))
        {
            if (curSector[*addr] != curSector[*lit])
            {
                *controlBuffer = short(19'682);
            }
            else
            {
                *controlBuffer = short(0);
            }
        }
        break;
    case andLessV:
        if (*controlBuffer != short(0))
        {
            if (curSector[*addr] < curSector[*lit])
            {
                *controlBuffer = short(19'682);
            }
            else
            {
                *controlBuffer = short(0);
            }
        }
        break;
    case andGreaterV:
        if (*controlBuffer != short(0))
        {
            if (curSector[*addr] > curSector[*lit])
            {
                *controlBuffer = short(19'682);
            }
            else
            {
                *controlBuffer = short(0);
            }
        }
        break;
    case orEqV:
        if (*controlBuffer == short(0))
        {
            if (curSector[*addr] == curSector[*lit])
            {
                *controlBuffer = short(19'682);
            }
            else
            {
                *controlBuffer = short(0);
            }
        }
        break;
    case orNeqV:
        if (*controlBuffer == short(0))
        {
            if (curSector[*addr] != curSector[*lit])
            {
                *controlBuffer = short(19'682);
            }
            else
            {
                *controlBuffer = short(0);
            }
        }
        break;
    case orLessV:
        if (*controlBuffer == short(0))
        {
            if (curSector[*addr] < curSector[*lit])
            {
                *controlBuffer = short(19'682);
            }
            else
            {
                *controlBuffer = short(0);
            }
        }
        break;
    case orGreaterV:
        if (*controlBuffer == short(0))
        {
            if (curSector[*addr] > curSector[*lit])
            {
                *controlBuffer = short(19'682);
            }
            else
            {
                *controlBuffer = short(0);
            }
        }
        break;
    case out:
        for (int i = 0; i < *lit; i++)
        {
            if (curSector[*addr + i] < 256)
            {
                cout << (char)curSector[*addr + i];
            }
            else
            {
                cout << curSector[*addr + i] << '\n';
            }
        }
        break;
    case inp:
        for (short i = 0; i < *lit; i++)
        {
            cin >> temp;
            curSector[*addr + i] = temp;
        }

        break;
    case sector:
        if (*addr != short(0))
        {
            if (sectors[*addr % short(27)] == nullptr)
            {
                throw runtime_error("Cannot access invalid memory sector");
            }
            curSector = sectors[*addr % short(27)];
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

    default:
        cout << "Error during processing of instruction " << *instruc << '\n';
        throw runtime_error("Could not process invalid command");
        break;
    }

    *instrPointer += short(3);

#ifdef TIMER
    auto end = chrono::high_resolution_clock::now();
    int ellapsed = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    if (instrTimes.count(*instruc) == 0)
    {
        instrTimes[*instruc] = pair<double, int>(ellapsed, 1);
    }
    else
    {
        instrTimes[*instruc].first += ellapsed;
        instrTimes[*instruc].second += 1;
    }
#endif

    return 0;
}

void cpu2::jumpIf()
{
    short numIfs(1);

    while (numIfs != short(0))
    {
        *instrPointer += short(3);

        if (mem[*instrPointer] == ifControl)
        {
            numIfs++;
        }
        else if (mem[*instrPointer] == endif)
        {
            numIfs--;
        }
    }

    return;
}
