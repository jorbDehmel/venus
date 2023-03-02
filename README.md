# Venus
## An improved CPU simulator
Jordan "Jorb" Dehmel, 2023

jdehmel@outlook.com

github.com/jorbDehmel/venus.git

## Overview: Venus, Venue, and Cove

Venus is a CPU simulator. It runs .Venus files which are
compiled by Venue, the Venus assembler. Venue takes in .tasl
(ternary assembly language) files. Cove is the Venus decompiler,
which takes .Venus files and outputs a possible .tasl
source.

Venus is a port of Tera, a base-3 CPU simulation, to binary.
It uses the same .tasl language as Tera.

Deco was the decompiler for Tera, so deCO VEnus I guess?
I don't remember why I named it this.

## Use

To compile the Venus source code:

`make`

To make these executables callable from anywhere (Linux only):

`make install`

To remove Venus binaries from your system (Linux only):

`make uninstall`

To compile some .tasl file into .Venus:

`bin/venue.out /PATH/TO/TASL/FILE.tasl -o /PATH/TO/OUTPUT/Venus/FILE.ven`

To run some .Venus file:

`bin/venus.out /PATH/TO/Venus/FILE.ven`

To decompile some .Venus file:

`bin/cove.out /PATH/TO/Venus/FILE.ven`
(this prints the .tasl)

`bin/cove.out /PATH/TO/Venus/FILE.ven /PATH/TO/OUTPUT/FILE.tasl` (this writes the .tasl to a new file)

## Shebangs (Linux only)

Using a text editor, you can add the following as the first line
of a COMPILED .ven program to make it executable anywhere.

`#!/usr/bin/venus` (followed by a newline)

You may or may not have to use the following command on the file
(depending on your Linux distro).

`chmod +x /PATH/TO/FILE.ven`

It is good form to also call the following to change the file
extension, although this is not strictly neccessary.

`mv /PATH/TO/FILE.ven /PATH/TO/FILE.vensh`

From then on, you can simply use the following to call the program.

`/PATH/TO/FILE.vensh`

## .tasl symbol meanings

\* - Access base scope variable (*VARNAME)

\+ - Access superscope variable (+VARNAME)

\. - Variable declaration (.VAR 1)

\~ - Stack pop (~VAR)

\_ - Base 27 literal (_0qf)

\- - Char literal

\/ - Comment (/ foo or // foobar)

\{ - Begin function ({FNNAME ...)

\} - End function (... })

\! - Update call stack and call function (!FNNAME)

\^ - Pointer arithmatic (^VARNAME.-12)

\$ - Size in bytes of a variable ($VARNAME)

\# - Call a macro (#print yada yada yada)

\` - Variable size/type assertion (\`VAR 2 or \`VAR u16 or \`VAR $u16)

\[ - Struct definition ([CLASSNAME ...)

\] - End struct definition (... ])

\= - Alias (=WHAT TO)

\@ - Dealias (\@WHAT)

## Macros

Macros are pre-compilation CPP functions which take arguments
and expand into corrosponding .tasl code. Below is a list of
macros and examples of their use.

**#print ARG**

Prints the following string directly to the CPU's output stream.

> #print Hello, World!

**#println ARG**

Does the same as #print and prints a newline.

> #println Hello,
> #println  World!

**#req ARG_1 ARG_2 ... ARG_N**

Ensures that all passed arguments are the names of variables in
the current scope.

> #req VAR1 VAR2
> / some operation using VAR1 and VAR2

**#cpy FROM TO**

Copy each byte in one variable to its relative position in
another.

> .VAR 9
> put ^VAR.3 -Y
> .CLONE 9
> #cpy VAR CLONE

**#zero VAR**

> .VAR 9
> #zero VAR

Set every byte within VAR to zero. This is important as
memory is not reset when a variable is freed from the stack.

## Instruction layout

A full instruction consists of a 2 byte instruction, a 2
byte address, and a 2 byte literal.

**Example**: Put "456" into memory position 123

> put 123 456

## Buffers

0 - instrPointer

Pointer to the current instruction.

1 - controlBuffer

Holds either 0 or non-zero. Controls all if statements.

2 - returnPointer

Where to jump back to after completing a function call.

## Instruction set

> kill (unused unused)

End the current program.

> put ADDRESS literal

Put the given literal at the given memory address.

> cpy FROM_ADDRESS TO_ADDRESS

Copy from one memory address to another.

> incr ADDRESS literal

Increment a memory address by a literal.

> decr ADDRESS literal

Decrement a memory address by a literal

> jump ADDRESS literal

If literal is non-zero, jump forward by that many instructions.
Otherwise, go to ADDRESS.

> jumpBack ADDRESS literal

Same as jump, but jumps backwards.

> ifControl (unused unused)

If the control is zero, jump past the corrosponding endif.

> endif (unused unused)

Terminates an if statement.

> andEqV ADDRESS_1 ADDRESS_2

The control buffer is set to one if it is one and the thing at
the first address matches the thing at the second address.
Otherwise, it is set to zero.

> andNeqV ADDRESS_1 ADDRESS_2

The control buffer is set to one if it is one and the thing at
the first address does not match the thing at the second.
Otherwise, it is set to zero.

> andLessV ADDRESS_1 ADDRESS_2

The control buffer is set to one if it is one and the thing at
the first address is less than the thing at the second one.
Otherwise, it is set to zero.

> andGreaterV ADDRESS_1 ADDRESS_2

The control buffer is set to one if it is one and the thing at
the first address is greater than the second. Otherwise, it is set
to zero.

> orEqV ADDRESS_1 ADDRESS_2

The control buffer is set to one if the thing at the first address
is equal to the second.

> orNeqV ADDRESS_1 ADDRESS_2

The control buffer is set to one if the thing at the first address
is not equal to the second.

> orLessV ADDRESS_1 ADDRESS_2

The control buffer is set to one if the thing at the first address
is less than the second.

> orGreaterV ADDRESS_1 ADDRESS_2

The control buffer is set to one if the thing at the first address
is greater than the second.

> out ADDRESS literal

Outputs the values of literal bytes starting at ADDRESS.

> inp ADDRESS literal

Inputs to the values of literal bytes starting at ADDRESS.

> sector ADDRESS (unused)

Switch the current memory sector. If ADDRESS is zero, switch
to the default memory sector. Otherwise, switch to the specified
one.

> addV TO FROM

The value in TO is equal to the value in TO plus the value in FROM.

> subtractV TO FROM

The value in TO is equal to the value in TO minus the value in FROM.

> multV TO FROM

The value in TO is equal to the value in TO times the value in FROM.

> divV TO FROM

The value in TO is equal to the value in TO divided the value in FROM.

> modV TO FROM

The value in TO is equal to the value in TO modulo the value in FROM.

> ifNever (unused unused)

Skip to the next endif, no matter what.

## Memory sectors

These are not extensively tested, but they allow for memory size upgrades
from 1 kilobyte to 27. A wrapper program could also link them to files
or drives and the CPU could use them accordingly with little modification.
