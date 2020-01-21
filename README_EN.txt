=== .c files: ===

./src/
-Location of the most recent files and to compile

emulMips.c Contains the hand and all the embryo of modified code.

function_cmd.c Contains all the functions that manage the grammar of commands
and parsing.

function.c Contains all functions for executing shell commands

assfunction.c Contains all functions of assembly instructions

mem.c Commands on memory

is.c Command on identity type functions (except is_range)

elf.c, syms.c, section.c, bits.c, types.c
Commands for ELF files


=== .h files: ===

./include/common/

notify.h No changes made.

macro.h Contains all #define

function_cmd.h Contains all the prototypes of the functions of function_cmd.c
and the structure of the interpreter

function.h -Contains all the prototypes of the functions of function.c
-The structures "opc_t" and "type_t" for the disasm.
-The prototypes of the 3 tables of function pointers (R, I, J).

assfunction.h Contains all prototypes of assembly instructions

mem.h - Contains all prototypes of mem.c functions
-The structures of breakpoint, virtual machine, registers and segments

is.h Prototypes of identity type functions (except is_range)

./include/elf/ Contains the headers files for the proper functioning of the load

=== Additional files ===

./dico/
-Location of the dictionaries used during the disasm

type.txt Contains all opcodes of all functions, all types combined

R.txt Contains all function codes of type R

I.txt Contains all the opcodes of type I functions

J.txt Contains all opcodes of type J functions

./demo/
-Location of functional test scripts

./test/
-Location of all test files


=== === Makefile
To start the FAIL test writing program
$ make test

to execute: ./w

=== === simpleUnitTest2.sh
Reuse of the simpleUnitTest.sh script for folder-by-folder execution with reccurtion

syntax: ./simpleUnitTest2.sh test /

