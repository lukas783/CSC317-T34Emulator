#ifndef __MEMIO_H_
#define __MEMIO_H_

#define MAXMEM 12288

#include <bitset>
#include <string>
#include <vector>

typedef unsigned char byte;

void putWord(byte *memarray, char *memaddr, char *value, int base = 16);
char *getWord(byte *memarray, char *memaddr, int base = 16, int outbase = 10);
void memdump(byte *memarray);
void parse(byte *memarray, std::string addrs);
void putMemory(byte *memarray, std::bitset<12> mar, std::bitset<24> mdr);
void getMemory(byte* memory, std::bitset<12> mar, std::bitset<24> &mdr);
#endif