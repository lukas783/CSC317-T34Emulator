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

int Emulator::getMemory() {
    int addr = int(reg.MAR.to_ulong())*3; 
    return ((memory[addr]<<16) | (memory[addr+1]<<8) | (memory[addr+2]));
}
void Emulator::decode() {
    int indicator = getBits(reg.IR, 10, 11);
    int op = getBits(reg.IR, 6, 9);
    int am = getBits(reg.IR, 2, 5);
    printf("%s  ", mnemonic[indicator][getBits(reg.IR, 6, 9)].c_str());    
    if(am == 0 && indicator != 0) {
        reg.MAR = getBits(reg.IR, 12, 23);
        EA = getMemory();
        printf("%03x  ", int(reg.MAR.to_ulong()));
    } else if(am == 1 && indicator != 0) {
        EA = getBits(reg.IR, 12, 23);
        if(reg.IR.test(23) == 1) {
            EA |= (0b111111111111<<12);
        }
        printf("IMM  ");
    } else {
        halted = true;
    }
    printf("\n");
    if(indicator == 3) {
        //printf("JUMP at %s", reg.IC);
    } else if(indicator == 2) {
        //printf("ADDOP at %s", reg.IC);
    } else if(indicator == 1) {
        
    } else {
        halted = true;
        printf("Machine Halted - HALT instruction executed\n");
    }
}

int Emulator::getBits(std::bitset<24> bits, int start, int end) {
    int n = end-start + 1;
    int extractedValue = 0;
    for(int i = end; i >= start; i--) {
        extractedValue |= (bits.test(i) << --n);
    }
    return extractedValue;
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
        /** Print out our current IC **/
        printf("%03x:  %06x  ", std::stoi(reg.IC,nullptr,16), reg.IR);
        /** Increment the IC..**/ // <-- re-do later by sending off to the ALU to add 1 instead of using sprintf
        sprintf(reg.IC, "%0x", std::stoi(reg.IC, nullptr, 16)+1);
        /** decode the instruction in reg.IR and set regs/flags as needed **/
        decode( );
        /** execute what needs to be executed **/
        //execute();
    }   

}
