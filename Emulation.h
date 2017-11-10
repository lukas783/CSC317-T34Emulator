#ifndef __REGISTERS_H_
#define __REGISTERS_H_

#include <string.h>
#include <stdio.h>
#include <vector>
#include "MemIO.h"

typedef unsigned char byte;

struct Registers {
    Registers();
    ~Registers();
    std::bitset<12> MAR;
    char* IC;
    std::vector<std::bitset<12>> X= {0,0,0,0};
    std::bitset<12> ABUS;
    std::bitset<24> MDR;
    std::bitset<24> AC;
    std::bitset<24> ALU;
    std::bitset<24> IR = 0;
    std::bitset<24> DBUS;
};

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
    int getMemory();
    int getBits(std::bitset<24> bits, int start, int end);
    void halt(int &type, int &op, std::string adr, std::string reason);
    void decode();
    bool memOp();
    bool ALUOp(int op, bool useAC, bool toAC);
    bool BranchOp();
    void printAccumulator();
};

const std::vector<std::vector<std::string>> mnemonic = {
    /** MISC INSTR **/
    {"HALT", "NOP ", "????", "????", "????", "????", "????", "????",
    "????", "????", "????", "????", "????", "????", "????", "????"},
    /** MEM INSTR **/
    {"LD  ", "ST  ", "EM  ", "????", "????", "????", "????", "????",
    "LDX  ", "STX ", "EMX ", "????", "????", "????", "????", "????"},
    /** ALU INSTR **/
    {"ADD ", "SUB ", "CLR ", "COM ", "AND ", "OR ", "XOR ", "????",
    "ADDX", "SUBX", "CLRX", "????", "????", "????", "????", "????"},
    /** BRANCH INSTR **/
    {"J   ", "JZ  ", "JN  ", "JP  ", "????", "????", "????", "????",
    "????", "????", "????", "????", "????", "????", "????", "????"}
};

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