#include "Emulation.h"

/**
 * Registers()
 * The constructor, instatiates an object usable by a class/program. In
 * our case it is just to create the register file we will be using,
 * nothing new needs to be created or assigned initially
 **/
Registers::Registers() {
    // do nothin!
}
/**
 * ~Registers()
 * The destructor for this struct, this handles cleaning up dynamic data.
 * In our case it is used to free up the IC register as that does include
 * dynamic data created with new outside of our control
 **/
Registers::~Registers() {
    if(IC != nullptr) {
        delete[] IC;
    }
}


/**
 * Emulator(char*)
 * The constructor of this class. Takes in a starting instruction counter (PC)
 * and sets the local IC register to the given value, then sets halted to false
 * so the emulator can start without any issues.
 **/
Emulator::Emulator(char* ic) {
    reg.IC = ic;
    halted = false;
}

/**
 * ~Emulator()
 * The class destructor, no dynamic information is created so this isn't
 * used other than by the underlying OS to destroy stuff that I have no
 * control over.
 **/
Emulator::~Emulator() {
    // new isnt used, no need to clean anything up
}

/** 
 * void setMemAddress(byte*)
 * A function used in setting up the emulator by setting the class memory
 * address pointer to an external one. This is done so the memory can be
 * populated outside the emulator and control of the memory and its contents
 * can be passed to the emulator when ready.
 **/
void Emulator::setMemAddress(byte* mem) {
    memory = mem;
}

/**
 * void halt(int&, int&, std::string, std::string)
 * This is a helper function for changing operations to a NOP instruction
 * and printing addressing information and setting the error message for 
 * invalid instructions. If an instruction uses an undefined/unimplemented
 * opcode or an unimplemented/illegal addressing mode or the emulation is
 * halted, then this function will change the current operation to a NOP,
 * output the effective address if possible and set the emulator to halt
 * with a specific reason
 * 
 * Inputs - int& - the instruction type to be changed to a NOP
 *          int& - the instruction operation to be changed to a NOP
 *          std::string - the effective address to be output if possible
 *          std::string - the message to display after "Machine Halted - "
 * Outputs - no value output, physical output to terminal for the effective
 *           address
 **/
void Emulator::halt(int &type, int &op, std::string adr, std::string reason) {
    if(adr == "000") 
        printf("%03x  ", getBits(reg.IR, 12, 23));
    else 
        printf("%s  ", adr.c_str());
    type = 0;           //NOP is of instruction type 00
    op = halted = 1;    // NOP is of instruction operation 01
    errmsg = reason;    // Set the error message 
}

/**
 * void IDandEXE()
 * The instruction decoder and instruction executor, the function starts by
 * extracting certain bits from the IR register to decode what type of
 * instruction it is, what operation it is performing, what addressing mode
 * it is operating under, and using that what the effective address is. This
 * function also detects illegal operations, such as invalid instructions, or
 * illegal/unimplemented/undefined opcodes/addressing modes. Once the instruction
 * has been declared legal and the flags have been set for the ALU accordingly,
 * the EA and ALU flags are passed to the ALU to perform the instruction and
 * control is handed back to the main loop to execute the next instruction.
 * 
 * Inputs - none
 * 
 * Outputs - no value output, physical output to terminal as the mnemonic for
 *           the current instruction and the effective address for the instruction
 **/
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
    //else if(reg.IR.test(9)) 
    //    halt(indicator, op, (am == 1) ? "IMM" : "000", "unimplemented opcode");
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
        getMemory(memory, reg.MAR, reg.MDR);
        ea = int(reg.MDR.to_ulong());
        printf("%03x  ", int(reg.MAR.to_ulong()));
    }
    /** Set our output flags, all except LD follow a similar output for their instr type**/
    ALUFlags |= (indicator << 4);
    if(op >= 8) 
        ALUFlags = (1<<7);
    ALUFlags |= op;

    /** Override the effective address (op2) for jump instructions**/
    if(indicator == 3) {
        ALUFlags |= (op<<8);
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
        if(op == 8) {
            
        }
    }
    ALUFlags |= (getBits(reg.IR, 0, 1) << 10);
    /** Perform final ALUOp **/
    ALUOp(ALUFlags, ea);
    /** If it's a store, we need to make sure it stores **/
    if(indicator == 1 && op == 1) 
        putMemory(memory, reg.MAR, reg.MDR);
}

/**
 * void ALUOp(int, int)
 * This is the ALU, it performs operations on two operands at a time, if the
 * bit 6 flag is set then operand 1 is the accumulator, otherwise it is 0, 
 * operand 2 is always the EA which is calculated/sign-extended before entering
 * the ALU. Once the input has been decided the operation and conditions are
 * pulled from bits 0-3 and 7-8 respectively. If conditions are met then the
 * selected operation is ran between the two operands. Valid operations include
 * addition, subtraction, clearing, two's-complementing, and, or, and exclusive or.
 * Once the operation has ran output is determined by bits 4-5 and can route the
 * output to the AC register, MDR register, or the IC register.
 * 
 * Inputs - int - the flags for input/operation/output of the ALU
 *          int - the second operand (effective address) for ALU operations.
 **/
void Emulator::ALUOp(int flags, int ea) {
    /** Flags breakdown
     * bits [10,11]- index reg being used | the value directly correlates to the register
     * bits [8,9] - 00 for unconditional, 01 for OPZ, 10 for OPN, 11 for OPP
     * bits [6,7] - op1 setter, 0 is 0, 1 is AC reg,  2 is IDX reg in
     * bits [3,5] - output to idx reg on 1, mdr on 2, ac on 4, ic on 6
     * bits [0,2] - op type (add/sub/clr/com/etc)
     **/

    /** Select our op1, decode the bit and sign extend as needed **/
    int op1 = (flags & (0b1<<6)) ? reg.AC.to_ulong() : 0;
    if(op1 & 0x800000)
        op1 |= (255<<24); // sign extend if needed for proper mathz
    op1 = (flags & (0b1<<7)) ? reg.X[(flags & (0b11<<10)) >> 10].to_ulong() : op1;
    if((flags & (0b1<<7)) && (op1 & 0x800))
        op1 |= (0xFFFFF<<12); // sign extension for index register math
    /** Get ALU output, ALU operation type, and conditional arguments from the flags **/
    int output = (flags & (0b111<<3)) >> 3;      // get output type flags
    int op = (flags & (0b111));                // get op type flags
    int conditional = (flags & (0b11<<8)) >> 8; // get conditional flags

    /** Evaluate conditionals, if condition is met, continue, else short-circuit exit **/
    switch(conditional) {
        case 1: // conditional zero, if not zero, exit ALU
            if(reg.AC != 0) return;
        break;
        case 2:
            if( reg.AC.to_ulong() == 0 || !(reg.AC.to_ulong() & (0b1<<23)) ) return; // if not < 0, exit ALU
        break;
        case 3:
            if( reg.AC.to_ulong() == 0 || (reg.AC.to_ulong() & (0b1<<23)) ) return; // if not > 0, exit ALU
        break;
    }
    /** Evaluate expression; performs a specific operation on the op1 and ea, stores result in the ALU reg **/
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

    /** Handle the directed output; funnel the result in the ALU reg to it's chosen destination **/
    switch(output) {
        case 0: // IGNORE
        break;
        case 1: // IDX reg output
            //printf("%d | ", (flags & (0b11<<10))>>10);
            reg.X[(flags & (0b11<<10))>>10] = reg.ALU.to_ulong(); // TODO: change this to verify it is 12-bits
        break;
        case 2: // MemOp
            reg.MDR = reg.ALU;
        break;
        case 4: // AC Op
            reg.AC = reg.ALU;
        break;
        case 6: // JOp
            sprintf(reg.IC, "%0x", ea);
        break;
    }
}

/**
 * int getBits(std::bitset<24>, int, int)
 * A helper function to extract specific bits from a 24 bit bitset
 * 
 * Inputs:  std::bitset<24> - the bitset to extract from
 *          int - the bit index to start extracting
 *          int - the bit index to stop extracting (inclusive)
 * Outputs: The value of bits from start to end
 **/
int Emulator::getBits(std::bitset<24> bits, int start, int end) {
    int n = end-start + 1;
    int extractedValue = 0;
    for(int i = end; i >= start; i--) {
        extractedValue |= (bits.test(i) << --n);
    }
    return extractedValue;
}

/**
 * void printAccumulator()
 * A helper function to output the current state of the accumulator and
 * the index registers.
 * 
 * Inputs: None
 * 
 * Outputs: no value output, but physical output to terminal containing
 *          the value of the accumulator register and the 4 index registers
 **/
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
 * void run()
 * The beating heart of this emulator. This will take the current instruction
 * and store it's value in the instruction register, then increment itself by 1.
 * Then the function will hand off to the decode and execute instruction and
 * output the contents of the accumulator and index registers. The loop only stops
 * when a halt instruction is executed or the end of memory is hit. If the emulator
 * is halted then a error msg will display stating what caused the halt.
 * 
 * Inputs - none
 * 
 * Outputs - no value output, physical output to the screen containing the current
 *           instruction address and contents as well as error messages.
 **/
void Emulator::run() {
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
        /** If we have an error msg, display it **/
        if(errmsg != "") 
            printf("Machine Halted - %s\n", errmsg.c_str());
    }   

}
