#include <iostream>
#include <fstream>
#include <string.h>
#include <string>

#define PROGNAME "T34Emulator.exe"
#define MAXMEM 12288
typedef unsigned char byte;

void putWord(byte *memarray, char *memaddr, char *value);

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("%s: fatal error: No input .obj file given.\nUsage: %s [.obj file]\n", PROGNAME, PROGNAME);
        return 1;
    }
    std::ifstream in;
    byte memarray[MAXMEM];
    memset(memarray, 0, sizeof(memarray));
    memarray[8] = 253;
    for(int i = 0; i < MAXMEM/2; i+=3)
        printf("[%d] - %d %d %d\n", i/3, memarray[i], memarray[i+1], memarray[i+2]);
    
    return 0;
}

void putWord(byte *memarray, char *memaddr, char *value) {
    int decAddr = std::stoi(memaddr, nullptr, 16)*3;
    int val = std::stoi(value, nullptr, 16);
    memarray[decAddr] = (val >> 16) & 0xFF;
    memarray[decAddr+1] = (val >> 8) & 0xFF;
    memarray[decAddr+2] = (val >> 0) & 0xFF;
    printf("%u | %u | %u\n", memarray[decAddr], memarray[decAddr+1], memarray[decAddr+2]);
}