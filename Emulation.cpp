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
    // new isnt used, no need to clean anything up
}

/** 
 * A simple function to reassign the pointer of our memory array to the 
 * same pointer in our class so our class has access to memory.
 **/
void Emulator::setMemAddress(byte* mem) {
    memory = mem;
}

void Emulator::getMemory() {
    int addr = int(reg.MAR.to_ulong())*3; 
    reg.MDR = ((memory[addr]<<16) | (memory[addr+1]<<8) | (memory[addr+2]));
}

void Emulator::halt(int &type, int &op, std::string adr, std::string reason) {
    if(adr == "000") 
        printf("%03x  ", getBits(reg.IR, 12, 23));
    else 
        printf("%s  ", adr.c_str());
    type = 0;
    op = halted = 1;
    errmsg = reason;
}

void Emulator::IDandEXE() {
    int indicator = getBits(reg.IR, 10, 11);// what type of instruction is it
    int op = getBits(reg.IR, 6, 9);         // what op in that type is it
    int am = getBits(reg.IR, 2, 5);         // what is the addressing mode
    int ea = 0;                             // the op2 (effective address)
    int ALUFlags = 0;                       // flags for the ALU  
    /** Get and print our mnemonic **/
    std::string instr = mnemonic[indicator][getBits(reg.IR, 6, 9)]; 
    printf("%s  ", instr.c_str());
    /** Handle HALT instruction **/
    if(indicator == 0 && op == 0) 
        halt(indicator, op, "   ", "HALT instruction executed");
    /** Handle undefined opcodes  **/
    else if(instr == "????")  
        halt(indicator, op, (am == 1) ? "IMM" : "000", "undefined opcode");
    /** Handle unimplemented opcodes **/
    else if(reg.IR.test(9)) 
        halt(indicator, op, (am == 1) ? "IMM" : "000", "unimplemented opcode");
    /** Handle illegal addressing modes **/
    else if(amodes[indicator][op] != "i" && amodes[indicator][op].find(std::to_string(am)) == std::string::npos)  
        halt(indicator, op, "???", "illegal addressing mode");
    /** Handle unimplemented addressing modes  **/
    else if(am == 2 || am == 4 || am == 6) 
        halt(indicator, op, "???", "unimplemented addressing mode");
    /** Set up addressing properly | immediate or direct; set EA accordingly **/
    else if(am == 1) {
        printf("IMM  ");
        ea = (reg.IR.test(23) == 1) ? (getBits(reg.IR, 12, 23) | (0xFFF<<12)) : getBits(reg.IR, 12, 23);
    } else if(am == 0) {
        reg.MAR = getBits(reg.IR, 12, 23);
        getMemory();
        ea = int(reg.MDR.to_ulong());
        printf("%03x  ", int(reg.MAR.to_ulong()));
    }
    /** Set our output flags, all except LD follow a similar output for their instr type**/
    ALUFlags |= (indicator << 4);
    ALUFlags |= op;
    /** Override the effective address (op2) for jump instructions**/
    if(indicator == 3) {
        ALUFlags |= (op<<7);
        ea = int(reg.MAR.to_ulong()); // if we are jumping, our jump to address is weird
    }
    /** Set the alu op type flag, if its a ld instr we need to override it to output to ac **/
    if(indicator == 2)
        ALUFlags |= (0b1<<6);
    if(indicator == 1) {
        if(op == 0) 
            ALUFlags = (0b10<<4); // add 0 to ea ; write to ac
        if(op == 1) {
            ea = 0;
            ALUFlags = (0b101<<4); // add ac to 0 and write to mdr
        }
        if(op == 2) {
            /** Store our loaded data temporarily, sign extend if necessary **/
            int temp = reg.MDR.to_ulong();
            if(temp & 0x800000)
                temp |= (255<<24);
            /** Set EA to 0 and flags to put AC into MDR, put MDR into memory at MAR **/
            ALUFlags = (0b101<<4); // set to put AC into MDR
            ea = 0;
            ALUOp(ALUFlags, ea);
            putMemory(memory, reg.MAR, reg.MDR);
            /** Set up a load op by putting temp storage into EA and setting flags **/
            ALUFlags = (0b10<<4);
            ea = temp;     
        }
    }
    /** Perform final ALUOp **/
    ALUOp(ALUFlags, ea);
    /** If it's a store, we need to make sure it stores **/
    if(indicator == 1 && op == 1) 
        putMemory(memory, reg.MAR, reg.MDR);
}

void Emulator::ALUOp(int flags, int ea) {
    /** Flags breakdown
     * bits [7,8] - 00 for unconditional, 01 for OPZ, 10 for OPN, 11 for OPP
     * bits [6,6] - op1 is accumulator if asserted, otherwise 0
     * bits [4,5] - output to mdr on 1, ac on 2, ic on 3
     * bits [0,3] - op type (add/sub/clr/com/etc)
     **/
    int op1 = (flags & (0b1<<6)) ? reg.AC.to_ulong() : 0; //pick our op1, op2 is always EA
    if(op1 & 0x800000)
        op1 |= (255<<24); // sign extend if needed for proper mathz
    int output = (flags & (0b11<<4)) >> 4;      // get output type flags
    int op = (flags & (0b1111));                // get op type flags
    int conditional = (flags & (0b11<<7)) >> 7; // get conditional flags
    switch(conditional) {
        case 1:
            if(reg.AC != 0) return;
        break;
        case 2:
            if( !(reg.AC.to_ulong() & (0b1<<23)) ) return;
        break;
        case 3:
            if( (reg.AC.to_ulong() & (0b1<<23)) ) return;
        break;
    }
    switch(op) {
        case 0: // ADD
            reg.ALU = op1 + ea;
        break;
        case 1: // SUB
            reg.ALU = op1 - ea;
        break;
        case 2: // CLR
            reg.ALU = 0;
        break;
        case 3: // COM
            reg.ALU = ~op1+1;
        break;
        case 4: // AND
            reg.ALU = op1 & ea;
        break;
        case 5: // ORR
            reg.ALU = op1 | ea;
        break;
        case 6: // XOR
            reg.ALU = op1 ^ ea;
        break;

    }
    switch(output) {
        case 0: // IGNORE
            break;
        case 1: // MemOp
            reg.MDR = reg.ALU;
        break;
        case 2: // AC Op
            reg.AC = reg.ALU;
        break;
        case 3: // JOp
            sprintf(reg.IC, "%0x", reg.ALU);
        break;
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
        IDandEXE();
        printAccumulator();
        if(errmsg != "")
            printf("Machine Halted - %s", errmsg.c_str());
    }   

}
