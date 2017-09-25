#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <stdio.h>

#define PROGNAME "T34Emulator.exe"
#define MAXMEM 12288
typedef unsigned char byte;

void putWord(byte *memarray, char *memaddr, char *value, int base = 16);
char *getWord(byte *memarray, char *memaddr, int base = 16, int outbase = 10);
void memdump(byte *memarray);

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("%s: fatal error: No input .obj file given.\nUsage: %s [.obj file]\n", PROGNAME, PROGNAME);
        return 1;
    }
    std::ifstream in;
    byte memarray[MAXMEM];
    char mem[4] = "0c0";
    char memval[8] = "200800";
    memset(memarray, 0, sizeof(memarray));
    putWord(memarray, mem, memval);
    memdump(memarray);
    return 0;
}

void readObj(char *filename, byte *memarray) {
    
}

void putWord(byte *memarray, char *memaddr, char *value, int base) {
    int decAddr = std::stoi(memaddr, nullptr, 16)*3;
    int val = std::stoi(value, nullptr, 16);
    memarray[decAddr] = (val >> 16) & 0xFF;
    memarray[decAddr+1] = (val >> 8) & 0xFF;
    memarray[decAddr+2] = (val >> 0) & 0xFF;
}

char* getWord(byte *memarray, char *memaddr, int base, int outbase) {
    int decAddr = std::stoi(memaddr, nullptr, base)*3;
    int retVal = 0;
    retVal += (memarray[decAddr] << 16) | (memarray[decAddr+1] << 8) | (memarray[decAddr+2] << 0);
    char *retStr = new char[26];
    if(outbase == 16) 
        sprintf(retStr, "%0x", retVal);
    else if(outbase == 10)
        sprintf(retStr, "%d", retVal);
    else
        printf("Invalid output base value, returning null.\n");
    return retStr;
}

void memdump(byte *memarray) {
    for(int i = 0; i < MAXMEM; i+=3) {
        int val = 0;
        val = (memarray[i] << 16) | (memarray[i+1] << 8) | (memarray[i+2] << 0);
        if(val != 0) {
            printf("[%03x] - %06x\n", (i/3), val);
        }   
    } 
}