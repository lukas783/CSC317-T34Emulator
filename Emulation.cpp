#include "Emulation.h"

Registers::Registers() {
    // do nothin!
}
Registers::~Registers() {
    if(IC != nullptr) {
        delete[] IC;
    }
}


Emulator::Emulator(char* ic) {
    reg.IC = ic;
    halted = false;
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
    printf("%d | ", opType);
    if(opType == 3) {
        printf("JUMP at %s\n", reg.IC);
    } else if(opType == 2) {
        printf("ADDOP at %s\n", reg.IC);
    } else if(opType == 1) {
        printf("LR/SR at %s\n", reg.IC);
    } else {
        printf("HALT AT %s\n", reg.IC);
        halted = true;
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
    while(!halted && strcmp(reg.IC, "fff")) {
        /** You see, it's simple... we fetch the instruction.. **/
        reg.IR = std::stoi(getWord(memory, reg.IC), nullptr, 10);
        /** Increment the IC..**/
        sprintf(reg.IC, "%0x", std::stoi(reg.IC, nullptr, 16)+1);
        printf("%d | %d \n", reg.IR.test(10), reg.IR.test(11));//reg.IR, (reg.IR & std::bitset<24>(3<<10)));
        decode( (reg.IR.test(11) << 1) | reg.IR.test(10) );

    }   

}

