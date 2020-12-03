# BUAA_Compile_2019
Codes for course design of BUAA Compiler 2019 (the highest challenge). Implemented with C++.

**Author**: Zinuo You

**Course Final Grade**: 96

**Guide Book**: [Guide Book (Chinese)](Compiler_GuideBook.pdf)

## Run
- **Compiler**: Clang
- **Recommended Development Enviroment**: Visual Studio 2019+.
- **Notice**: Due to automatic judgement enviroment, there's not practical directories and all codes are in the same directory. If you want to view codes more carefully, I strongly recommend you to clone the codes and use Visual Studio open .vcxproj file (virtual directories will show).
- **Input file**: testfile.txt
- **Output files**: 
  1. output.txt: objects codes (can run on MARS)
  2. log files: log information when running in different steps.(errors, warnings, data flow graph, reference count and etc.)



## Features
- **Source Code**: Persuade C (similar to C, but simplified for teaching purpose.)
- **Object Code**: MIPS-32
- **Pluggable Compiling Optimization**
  1. inline optimization.
  2. assignment spread optimizing.
  3. dead code optimization.
  4. register allocation (global registers: reference count, local registers: FIFO with active analysis on data flow graph).
  5. seek optimization for object codes generation. (branch, mod and etc.)
  6. Final Optimization Race: 6/306, please see [Optimization Report(Chinese)](Optimization_Report.pdf) for details. 



## Branches
- **unit2** : Implement lexical parser on FSM. 
- **unit3** : Implement grammer parser with recursive descent method (from top to bottom). Use peeking (next one/two) to make sure no recall.
- **unit4** : Implement error analysis of grammer errors and semantic errors.
- **unit5** : Implement object codes generation of generating middle codes and generating MIPS codes. Object codes can run on MARS but do not have any optimization with only three registers being used, so you can almost regard it as a stack symbol machine.
- **unit6** : Current version, see "features" for details. 
