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

void Emulator::halt(int &type, int &op, std::string adr, std::string reason) {
    printf("%s  ", adr.c_str());
    type = 0;
    op = halted = 1;
    errmsg = reason;
}

void Emulator::decode() {
    int indicator = getBits(reg.IR, 10, 11);// what type of instruction is it
    int op = getBits(reg.IR, 6, 9);         // what op in that type is it
    int am = getBits(reg.IR, 2, 5);         // what is the addressing mode
    /** Get and print our mnemonic **/
    std::string instr = mnemonic[indicator][getBits(reg.IR, 6, 9)]; 
    printf("%s  ", instr.c_str());
    /** Handle HALT instruction **/
    if(indicator == 0 && op == 0) 
        halt(indicator, op, "   ", "HALT instruction executed");
    /** Handle undefined opcodes  **/
    else if(instr == "????")  
        halt(indicator, op, "   ", "undefined opcode");
    /** Handle unimplemented opcodes **/
    else if(reg.IR.test(9)) 
        halt(indicator, op, "   ", "unimplemented opcode");
    /** Handle illegal addressing modes **/
    else if(amodes[indicator][op] != "i" && amodes[indicator][op].find(std::to_string(am)) == std::string::npos)  
        halt(indicator, op, "???", "illegal addressing mode");
    /** Handle unimplemented addressing modes  **/
    else if(am == 2 || am == 4 || am == 6) 
        halt(indicator, op, "???", "unimplemented addressing mode");
    /** If it's here then it's a valid instruction and we will execute
     *  set up flags (ALU op and EA)
     **/
    else {
        if(am == 0 && indicator != 0) {
            reg.MAR = getBits(reg.IR, 12, 23);
            reg.ALU = getMemory();
            printf("%03x  ", int(reg.MAR.to_ulong()));
        } else if(am == 1 && indicator != 0) {
            reg.ALU = getBits(reg.IR, 12, 23);
            if(reg.IR.test(23) == 1) {
                reg.ALU |= (0b111111111111<<12);
            }
            printf("IMM  ");
        } else {
            printf("     ");
        }  
        if(indicator == 3) {
            ALUOp(17, true, false);
            if(op == 0 || reg.MDR == op)
                sprintf(reg.IC, "%0x", reg.MAR);
            printAccumulator();
        } else if(indicator == 2) {
            ALUOp(getBits(reg.IR, 6, 9), true, true);
            printAccumulator();
        } else if(indicator == 1) {
            if(getBits(reg.IR, 6, 9) == 0) {
                ALUOp(0, false, true);
            }
            if(getBits(reg.IR, 6, 9) == 1) {
                ALUOp(16, true, false);
                putMemory(memory, reg.MAR, reg.MDR);
            }
            printAccumulator();
        } else {
            printAccumulator();
            if(op == 0) {
                halted = true;
                printf("Machine Halted - HALT instruction executed\n");
            }
        }
    }
}

bool Emulator::ALUOp(int op, bool useAC, bool toAC) {
    std::bitset<24> operand = useAC ? reg.AC : 0;
    switch(op) {
        /** ADD **/
        case 0:
            reg.ALU = useAC ? reg.ALU.to_ulong() + reg.AC.to_ulong() : reg.ALU; // cheating, but we only have 24 bits, not 64
        break;
        case 1:
            reg.ALU = reg.AC.to_ulong() - reg.ALU.to_ulong(); // cheating, but we only have 24 bits, not 64    
        break;
        case 2:
            reg.ALU = 0;
        break;
        case 3:
            reg.ALU = ~reg.AC;
        break;
        case 4:
            reg.ALU = reg.ALU & reg.AC;
        break;
        case 5:
            reg.ALU = reg.ALU | reg.AC;
        break;
        case 6:
            reg.ALU = reg.ALU ^ reg.AC;
        break;
        case 16:
            reg.ALU = reg.AC;
        break;
        case 17:
            if(reg.AC.test(23) == 1) 
                reg.ALU = 2;
            else if(reg.AC.to_ulong() > 0)
                reg.ALU = 3;
            else
                reg.ALU = 1;
        break;             
        default:
            halted = true;
        break;
    }
    
    if(toAC) 
        reg.AC = reg.ALU;
    else if(!toAC) 
        reg.MDR = reg.ALU;
    return true;
}
int Emulator::getBits(std::bitset<24> bits, int start, int end) {
    int n = end-start + 1;
    int extractedValue = 0;
    for(int i = end; i >= start; i--) {
        extractedValue |= (bits.test(i) << --n);
    }
    return extractedValue;
}

void Emulator::printAccumulator() {
    printf("AC[%06x]  X0[%03x]  X1[%03x]  X2[%03x]  X3[%03x]\n",
    int(reg.AC.to_ulong()),
    int(reg.X[0].to_ulong()),
    int(reg.X[1].to_ulong()),
    int(reg.X[2].to_ulong()),
    int(reg.X[3].to_ulong())    
    );
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
    }   

}
