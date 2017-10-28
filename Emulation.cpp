#include "Emulation.h"

Registers::Registers() {
    // do nothin!
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


Emulator::Emulator(char* ic) {
    reg.IC = ic;
}

Emulator::~Emulator() {
    // do nothing!
}

/** 
 * A simple function to reassign the pointer of our memory array to the 
 * same pointer in our class so our class has access to memory.
 **/
void Emulator::setMemAddress(byte* mem) {
    memory = mem;
}

/** 
 * The beating heart of our emulator, it will handle continuous execution
 * of the program, fetching of the next instruction, interfacing between
 * memory and registers, and outputting to screen from this point on
 *
 * note to self: maybe take a note from python and do some kind of REPL?
 * Read - look at IC, decode and prepare
 * Eval - process the instruction and modify mem/regs as needed
 * Print - print the addr, value, instr, ac reg, and x0-3
 * Loop - increment IC if non-branch and loop back to R (Read)
 **/
void Emulator::run() {
    /** You see, it's simple... we fetch the instruction.. **/
    char* fetched = getWord(memory, reg.IC);
    printf("Old IC: %s | New IC: ",reg.IC);
    /** Increment the IC..**/\
    sprintf(reg.IC, "%0x", std::stoi(reg.IC, nullptr, 16)+1);
    printf("%s\n", reg.IC);
}

