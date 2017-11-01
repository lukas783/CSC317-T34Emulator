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
    void decode(int opType);
    bool memOp();
    bool ALUOp();
    bool BranchOp();
};

#endif