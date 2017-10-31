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

#include <fstream>
#include <string.h>
#include "Emulation.h"
#include "MemIO.h"

#define PROGNAME "T34Emulator.exe"
#define MAXMEM 12288

char *readObj(char *filename, byte *memarray);

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
    Emulator emulator(readObj(argv[1], memarray));
    emulator.setMemAddress(memarray);

    /** start the emulation! **/
    emulator.run();
    
    /** dump it all, parse a few **/
//    memdump(memarray);
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

