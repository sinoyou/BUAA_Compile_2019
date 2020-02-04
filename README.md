# BUAA_Compile_2019
Codes for course design of BUAA Compiler 2019 (the highest challenge). Implemented with C++.

Author: Zinuo You

Course Final Grade: 96

## Run
- Compiler: Clang
- Recommended Development Enviroment: Visual Studio 2019+.
- !!! Due to auto judge enviroment, there'is not practical directories and all codes are in the same directory. If you want to view codes more carefully, I strongly recommend you to clone the codes and use Visual Studio open .vcxproj file (virtual directories will show).
- Input file: testfile.txt
- Output files: 
  1. output.txt: objects codes (can run on MARS)
  2. log files: log information when running in different steps.(errors, warnings, data flow graph, reference count and etc.)



## Features
- Source Code: Persuade C (similar to C, but simplified for education.)
- Object Code: MIPS-32
- Pluggable Compiling Optimization
  1. inline optimizing.
  2. assign spread optimizing.
  3. dead code optimizing.
  4. register allocation (global registers: reference count, local registers: FIFO with active analysis on data flow graph).
  5. seek optimization for object codes generation. (branch, mod and etc.)



## Branches
- unit2 : Implement lexical parser.
- unit3 : Implement grammer parser with recursive descent method (from top to bottom). Use peeking (next one/two) to make recursive no recalls under codes with accurate format.
- unit4 : Add common error analysis of grammer error and semantic error.
- unit5 : Generating object codes. Codes can run on MARS but do not get non optimization. Only three registers are used, so you can almost regard it as a stack symbol machine.
- unit6 : Current Version.
