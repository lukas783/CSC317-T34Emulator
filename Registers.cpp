#include "Registers.h"

Registers::Registers(char* ic) {
    IC = ic;
}

Registers::~Registers() {
    if(MAR != nullptr) 
        delete[] MAR;
    if(IC != nullptr) 
        delete[] IC;
    if(ABUS != nullptr) 
        delete[] ABUS;
    if(MDR != nullptr) 
        delete[] MDR;
    if(AC != nullptr) 
        delete[] AC;
    if(ALU != nullptr) 
        delete[] ALU;
    if(IR != nullptr) 
        delete[] IR;
    if(DBUS != nullptr) 
        delete[] DBUS;
    for(int i = 0; i < X.size(); i++) {
        delete[] X[i];
    }
}