#include "MemIO.h"

/**
 * putWord(byte*, char*, char*, int=16)
 * - Function takes in an array of bytes acting as memory, an
 * - array of characters representing in hex a memory address, 
 * - another array of characters representing a value in a base
 * - to store in the hex memory address, and an int defaulted to
 * - 16 to represent the base the value being put into the hex 
 * - address is needed to be converted from.
 * 
 * - Inputs: byte* - an array of bytes acting as memory
 *           char* - an array of chars representing a hex mem address
 *           char* - an array of chars representing a value in an unknown base
 *           int   - an int, defaulted to 16, representing the char* value's base
 * 
 * - Ouputs: no data output, no physical output
 **/
void putWord(byte *memarray, char *memaddr, char *value, int base) {
    int decAddr = std::stoi(memaddr, nullptr, 16)*3;
    int val = std::stoi(value, nullptr, base);
    memarray[decAddr] = (val >> 16) & 0xFF;
    memarray[decAddr+1] = (val >> 8) & 0xFF;
    memarray[decAddr+2] = (val >> 0) & 0xFF;
}

/**
 * getWord(byte*, char*, int=16, int=10)
 * - Function takes in an array of bytes acting as memory, an
 * - array of characters representing in hex a memory address
 * - and two ints representing input base and output base both
 * - defaulting to 16 and 10 respectively. The code converts
 * - the input base memory address to a decimal integer and
 * - navigates to the correct offset in the byte array. Then
 * - the current byte and the next two bytes are OR'd together
 * - and shifted appropriately to create a 24bit word, this word
 * - is converted to the proper output base as a char* and returned
 * 
 * - Inputs: byte* - an array of bytes acting as memory
 *           char* - an array of chars representing a mem address
 *           int   - an input base to convert char* to decimal correctly
 *           int   - an output base for the returned char* array
 * 
 * - Ouputs: char* - a character array in a specific base containing the
 *                   24 bit value of a memory address
 **/
char* getWord(byte *memarray, char *memaddr, int base, int outbase) {
    /** Convert our memaddr to a base10 number and find the array offset **/
    int decAddr = std::stoi(memaddr, nullptr, base)*3;
    int retVal = 0;
    /** OR and LSL offset plus next 2 bytes to create a 24 bit word **/
    retVal += (memarray[decAddr] << 16) | (memarray[decAddr+1] << 8) | (memarray[decAddr+2] << 0);
    char *retStr = new char[26];
    /** Convert our new value to a char* string in the proper base **/
    if(outbase == 16) 
        sprintf(retStr, "%0x", retVal);
    else if(outbase == 10)
        sprintf(retStr, "%d", retVal);
    else
        printf("Invalid output base value, returning null.\n");
    return retStr;
}

/**
 * memdump(byte*)
 * - Function takes in an array of bytes functioning as memory,
 * - the code loops through each memory address and outputs any
 * - addresses with a value not equal to 0
 * 
 * - Inputs: byte* - an array of bytes acting as memory
 * 
 * - Ouputs: no data output, physical output to terminal in form
 *           [MEMADDR] - HEXVAL(6 hex digits)
 **/
void memdump(byte *memarray) {
    /** loop through 3 bytes of memory at a time (24bit memory addresses), if not 0, output value **/
    for(int i = 0; i < MAXMEM; i+=3) {
        int val = 0;
        val = (memarray[i] << 16) | (memarray[i+1] << 8) | (memarray[i+2] << 0);
        if(val != 0) {
            printf("[%03x] - %06x\n", (i/3), val);
        }   
    } 
}

/** 
 * parse(byte*, std::string)
 * - Function takes in an array of bytes functioning as static memory
 * - and a string of hex addresses separated by ','s or ' 's and splits
 * - the string into a list of strings, then each address is parsed
 * - and the output of each byte in memory is displayed as 3 bitstrings
 * 
 * - Inputs: byte* - an array of bytes posing as memory
 *           std::string - a string of memory addresses to parse
 * - Ouputs: no data output, physical output to terminal in form
 *           [MEMADDR]:    ADDR(12bits) OP(6bits) AM(6bits)
 **/
void parse(byte *memarray, std::string addrs) {
    printf("%*s%*s%*s\n", 18, "ADDR", 9, "OP", 7, "AM");
    
    std::vector<std::string> addrList;
    int startIdx = 0;
    /** Loop through our input string of addresses, if we find a ',' or a ' ', then we substring and put in list **/
    for(int i = 0; i < addrs.length(); i++) {
        if(addrs[i] == ',' || addrs[i] == ' ') {
            addrList.push_back(addrs.substr(startIdx, i-startIdx));          
            startIdx = i+1;
        }
    }
    addrList.push_back(addrs.substr(startIdx, addrs.length()-startIdx));

    /** Loop through our list of addresses, outputting bitstrings for each one from memory**/
    for(int i = 0; i < addrList.size(); i++) {
        char trimmedAddr[10] = "";
        /** This piece is used just to trim our list in case something weird happened**/
        sprintf(trimmedAddr, "%03x", std::stoi(addrList[i], nullptr, 16));
        char *addrValue = getWord(memarray, trimmedAddr);
        printf("%03s:", trimmedAddr);
        /** %#s for formatting purposes, convert our base10 char* to an int, shift and/or AND with 6bit string and convert from bitset to std::string to c_str() **/
        printf("%18s", std::bitset<12>(std::stoi(addrValue, nullptr, 10)>>12).to_string<char, std::string::traits_type,std::string::allocator_type>().c_str());
        printf("%7s", std::bitset<6>((std::stoi(addrValue, nullptr, 10)>>6)&0x3F).to_string<char, std::string::traits_type,std::string::allocator_type>().c_str());
        printf("%7s\n", std::bitset<6>(std::stoi(addrValue, nullptr, 10)&0x3F).to_string<char, std::string::traits_type,std::string::allocator_type>().c_str());
        /** Since we use 'new' in getWord, we have to delete addrValue... memory management, woo **/
        delete addrValue;
    }
}

/**
 * void putMemory(byte*, std::bitset<12>, std::bitset<24>)
 * Puts the value in std::bitset<24> into the byte* array at 24-bit word offset of
 * std::bitset<12>, using bitwise operations to store one byte at a time;
 * 
 * Inputs - byte* - the static memory
 *          std::bitset<12> - the 24-bit word offset to store in memory
 *          std::bitset<24> - the value to store at the 24-bit word offset in memory
 * Outputs - none
 **/
void putMemory(byte *memarray, std::bitset<12> mar, std::bitset<24> mdr) {
    int addr = int(mar.to_ulong())*3;
    memarray[addr] = (mdr.to_ulong() >> 16) & 0xFF;
    memarray[addr+1] = (mdr.to_ulong() >> 8) & 0xFF;
    memarray[addr+2] = (mdr.to_ulong() >> 0) & 0xFF;
}

/**
 * void putHalfMemory(byte*, std::bitset<12>, std::bitset<24>)
 * Puts the value in std::bitset<24> into the byte* array at 24-bit word offset of
 * std::bitset<12>, using bitwise operations to store into only the upper half of 
 * the memory word.
 * 
 * Inputs - byte* - the static memory
 *          std::bitset<12> - the 24-bit word offset to store in memory
 *          std::bitset<24> - the value to store at the 24-bit word offset in memory
 * Outputs - none
 **/
void putHalfMemory(byte *memarray, std::bitset<12>mar, std::bitset<24> mdr) {
    int addr = int(mar.to_ulong())*3;
    mdr <<= 12; // position the mdr to clear unneccessary bits
    memarray[addr] = (mdr.to_ulong() >> 16) & 0xFF; // set the upper byte
    memarray[addr+1] &= 0xF; // clear the upper nyble of the middle byte
    memarray[addr+1] |= (mdr.to_ulong() >> 8) & 0xF0; // set the upper nyble of the middle byte
}

/**
 * void getMemory(byte*, std::bitset<12>, std::bitset<24>&)
 * A function to retrieve the contents at a specific offset in a memory array
 * and store it's found value into a std::bitset<24>
 * 
 * Inputs - byte* - the static memory
 *          std::bitset<12> - the 24-bit word offset to go to in memory
 *          std::bitset<24> - the bitset to store the calculated value
 * Outputs - none
 **/
void getMemory(byte* memory, std::bitset<12> mar, std::bitset<24> &mdr) {
    int addr = int(mar.to_ulong())*3; 
    mdr = ((memory[addr]<<16) | (memory[addr+1]<<8) | (memory[addr+2]));
}