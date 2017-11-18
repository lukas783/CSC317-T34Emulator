#ifndef __REGISTERS_H_
#define __REGISTERS_H_

#include <string.h>
#include <stdio.h>
#include <vector>
#include "MemIO.h"

typedef unsigned char byte;

/**
 * struct Registers - A struct containing all the necessary registers
 * for this emulator. Some are there only in spirit where others are 
 * used in every operation of the emulator.
 **/
struct Registers {
    Registers();
    ~Registers();
    std::bitset<12> MAR; // Memory Address Register
    char* IC; // Instruction Counter (Program Counter)
    std::vector<std::bitset<12>> X= {0,0,0,0}; // 4 Index Registers
    std::bitset<12> ABUS; // Address Bus
    std::bitset<24> MDR; // Memory Data Register
    std::bitset<24> AC;  // Accumulator register
    std::bitset<24> ALU; // Arithmetic Logic Unit output register
    std::bitset<24> IR = 0; // Instruction Register
    std::bitset<24> DBUS; // Data Bus
};

/**
 * class Emulator - a class that controls the operation and variables of
 * our emulator, once constructed and the memory address is defined, run
 * will be called and the emulator will run itself based on the architecture
 * write-up
 **/
class Emulator {
public:
    Emulator(char* ic);
    ~Emulator();
    void setMemAddress(byte *mem);
    void run();
    
private:
    byte* memory;
    Registers reg;
    bool halted;
    std::string errmsg = "";
    int getBits(std::bitset<24> bits, int start, int end);
    void halt(int &type, int &op, std::string adr, std::string reason);
    void IDandEXE();
    void ALUOp(int flags, int ea);
    void printAccumulator();
};

/** A global, constant 2D vector of strings containing instruction mnemonics **/
const std::vector<std::vector<std::string>> mnemonic = {
    /** MISC INSTR **/
    {"HALT", "NOP ", "????", "????", "????", "????", "????", "????",
    "????", "????", "????", "????", "????", "????", "????", "????"},
    /** MEM INSTR **/
    {"LD  ", "ST  ", "EM  ", "????", "????", "????", "????", "????",
    "LDX ", "STX ", "EMX ", "????", "????", "????", "????", "????"},
    /** ALU INSTR **/
    {"ADD ", "SUB ", "CLR ", "COM ", "AND ", "OR ", "XOR ", "????",
    "ADDX", "SUBX", "CLRX", "????", "????", "????", "????", "????"},
    /** BRANCH INSTR **/
    {"J   ", "JZ  ", "JN  ", "JP  ", "????", "????", "????", "????",
    "????", "????", "????", "????", "????", "????", "????", "????"}
};

/** A global, constant 2D vector of legal addressing modes **/
const std::vector<std::vector<std::string>> amodes = { // i apologize if you are reading this
    /** Misc legal addressing modes **/
    {"i", "i"},
    /** MemOps legal addressing modes **/
    {"01246", "0246", "0246", "", "", "", "", "", "01", "0", "0"},
    /** Logic/Arithmetic legal addressing modes **/
    {"01246", "01246", "i", "i", "01246", "01246", "01246", "", "01", "01", "i"},
    /** Jump instruction legal addressing modes **/
    {"0246", "0246", "0246", "0246"},
};
#endif