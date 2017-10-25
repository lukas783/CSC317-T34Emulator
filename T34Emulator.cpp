/********************************************************************
 * T34Emulator.cpp
 * 
 * Author:   Lucas Carpenter
 * Date:     09/25/2017
 * Class:    CSC317 (Computer Org & Architecture)
 * Professor:Dr. Karlsson
 * 
 * Description: T34Emulator is a program that creates a memory array of 
 * 4096 (212) words where each word contains 24 bits. Memory can be read
 * from and written to using the putWord and getWord functions, a memory 
 * dump can be executed using the memdump function and you can parse words 
 * of memory into their instructions using the parse function. The program 
 * also must read in an object file that contains memory addresses and their 
 * data to store, the last item in the object file will be the address the 
 * program counter will start at, this value is stored in a char* as a 
 * variable called ‘pc’. To perform a memory dump, the function 
 * memdump(byte*) can be used, or to parse a string the function 
 * parse(byte*, std::string) can be used
 * 
 * Compiling:
 * $ g++ -std=c++11 -o T34Emulator T34Emulator.cpp
 * 
 * Usage:
 * $ T34Emulator [objfile.obj]
 * Where [objfile.obj] is replaced with the file path for your .obj file
 * 
 ********************************************************************/

#include <bitset>
#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <vector>
#include "Registers.h"

#define PROGNAME "T34Emulator.exe"
#define MAXMEM 12288


char *readObj(char *filename, byte *memarray);
void putWord(byte *memarray, char *memaddr, char *value, int base = 16);
char *getWord(byte *memarray, char *memaddr, int base = 16, int outbase = 10);
void memdump(byte *memarray);
void parse(byte *memarray, std::string addrs);

/**
 * main(int, char*[])
 * - Function is the starting point of our program, the program
 * - begins and ends here. 
 * 
 * - Inputs: int     - the number of arguments from command line
 *           char*[] - a 2D array of chars returning input from command line
 * 
 * - Ouputs: returns 0 on successful program finish, physical
 *           output to terminal if invalid num of arguments.
 **/
int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("%s: fatal error: No input .obj file given.\nUsage: %s [.obj file]\n", PROGNAME, PROGNAME);
        return 1;
    }
    /** create an array of memory of length MAXWORDS*3 **/
    byte memarray[MAXMEM];
    memset(memarray, 0, sizeof(memarray));
    /** create our registers/fill our memory/assign our memory pointer/set the ic **/
    Registers regfile(readObj(argv[1], memarray));
    regfile.setMemAddress(memarray);

    /** start the emulation! **/
    regfile.run();
    
    /** dump it all, parse a few **/
    memdump(memarray);
    parse(memarray, "0c4,0c5 0c6,0c7 0c8 050");

    return 0;
}

/**
 * readObj(char*, byte*)
 * - Function takes in an array of characters representing an object
 * - filename to be opened and an array of bytes representing memory.
 * - The function reads through the object file and uses a specific
 * - object file format to store values into continuous memory.
 * 
 * - Inputs: char* - an array of characters representing an obj filename
 *           byte* - an array of bytes representing memory
 * 
 * - Ouputs: no data output, physical output only if file can't be opened
 **/
char *readObj(char *filename, byte *memarray) {
    std::ifstream in(filename);
    if(!in.is_open()) {
        printf("%s: fatal error: Unable to open object file %s. Exiting program.\n\n", PROGNAME, filename);
        exit(2);
    }
    char *addr = new char[4];
    int count = 0;
    char buffer[7] = "";
    while( in >> addr && in >> count) {
        for(int i = 0; i < count; i++) {
            in >> buffer;
            putWord(memarray, addr, buffer);
            sprintf(addr, "%0x", (std::stoi(addr, nullptr, 16) + 1));
        }
    }
    in.close();
    return addr; // returns out the address for the prog counter
}

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
