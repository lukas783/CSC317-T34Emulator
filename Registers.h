#ifndef __REGISTERS_H_
#define __REGISTERS_H_

#include <stdio.h>
#include <vector>

typedef unsigned char byte;

class Registers {
public:
    Registers(char* ic);
    ~Registers();
    void setMemAddress(byte *mem);
    void run();
    
private:
    byte* memory;
    char* MAR;
    char* IC;
    std::vector<char*> X = {0, 0, 0, 0};
    char* ABUS;
    char* MDR;
    char* AC;
    char* ALU;
    char* IR;
    char* DBUS;
};

#endif