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
    
}

/** 
 * A simple function to reassign the pointer of our memory array to the 
 * same pointer in our class so our class has access to memory.
 **/
void Emulator::setMemAddress(byte* mem) {
    memory = mem;
}

void Emulator::decode(int opType) {
    int fetched = opType>>10;
    if(fetched == 3) {
        printf("JUMP at %s\n", reg.IC);
    } else if(fetched == 2) {
        printf("ADDOP at %s\n", reg.IC);
    } else if(fetched == 1) {
        printf("LR/SR at %s\n", reg.IC);
    } else {
//        printf("HALT AT %s\n", reg.IC);
    }
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
    //char* newIC = new char[4];
    while(strcmp(reg.IC, "fff") != 0) {
        /** You see, it's simple... we fetch the instruction.. **/
        reg.IR = std::stoi(getWord(memory, reg.IC), nullptr, 10);
        /** Increment the IC..**/\
        sprintf(reg.IC, "%0x", std::stoi(reg.IC, nullptr, 16)+1);
        decode(reg.IR-(0b11<<10));

        /** How these instructions are decoded.. 
        if((reg.IR & (0b11<<10)) == (0b11<<10)) {
            printf("JUMP at %s\n", reg.IC);
        } else if((fetched & (0b1<<11)) == (0b1<<11)) {
            printf("ADDOP at %s\n", reg.IC);
        } else if((fetched & (0b01<<10)) == (0b01<<10)) {
            printf("LR/SR at %s\n", reg.IC);
        } else {
            printf("HALT AT %s\n", reg.IC);
            break;
        }*/

//        strcpy(reg.IC, newIC);
    }   

}

