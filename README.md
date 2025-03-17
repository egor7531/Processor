<h1 align="center">Processor</h1>

# Description
The Processor program is designed to execute bytecode instructions stored in a file. It consists of several main modules:
- Assembler: Converts human-readable assembler code into byte code;
- Disassembler:Does everything the other way around
- CPU: Executes bytecode instructions;
- Stack: Implements the stack data structure used in the CPU;

# Run
You need a C compiler (for example, g++).
```
cd Processor
```
To run the assembler:
```
g++ Assembler.cpp 
```
To run the cpu:
```
g++ CPU.cpp 
```
To run the disassembler:
```
g++ Disassembler.cpp 
```
