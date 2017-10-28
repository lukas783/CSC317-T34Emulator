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
    char* MAR;
    char* IC;
    std::vector<char*> X= {0,0,0,0};
    char* ABUS;
    char* MDR;
    char* AC;
    char* ALU;
    char* IR;
    char* DBUS;
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
    
};

#endif