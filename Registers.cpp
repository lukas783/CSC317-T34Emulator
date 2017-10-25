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

/** 
 * A simple function to reassign the pointer of our memory array to the 
 * same pointer in our class so our class has access to memory.
 **/
void Registers::setMemAddress(byte* mem) {
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
void Registers::run() {

}