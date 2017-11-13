################ MAKEFILE TEMPLATE ################

# Author : Lucas Carpenter

# Usage : make target1

# What compiler are we using? (gcc, g++, nvcc, etc)
LINK = g++

# Name of our binary executable
OUT_FILE = T34Emulator

# Any weird flags ( -O2/-O3/-Wno-deprecated-gpu-targets/-fopenmp/etc)
FLAGS = -std=c++11 -O

all: T34Emulator

#Rules for compiling into *.o files
%.o: %.cpp
	$(LINK) $(FLAGS) -c -o $@ $<

#Rules for compiling the T34Emulator program
T34Emulator: T34Emulator.o MemIO.o Emulation.o
	$(LINK) -o $(OUT_FILE) $^

clean: 
	rm -f *.o *~ core $(OUT_FILE)
