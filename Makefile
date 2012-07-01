CC = g++
CFLAGS = -O2

ifdef OPENMP
PCC = $(CC) 
CFLAGS = -fopenmp -O2 -DOPENMP

else ifdef CILK
PCC = $(CC)
CFLAGS = -O2 -lcilkrts -DCILK -Wno-cilk-for

else ifdef IPPROOT
PCC = icpc
CFLAGS = -O2 -DCILKP

else 
PCC = $(CC)
CFLAGS = -O2 
endif

all: ansv 

.PHONY: clean

clean: 
	rm -rf ansv *~ *.o

ANSV.o: ANSV.cpp ANSV.h
	$(PCC) $(CFLAGS) -c $<

main.o: main.cpp ANSV.o
	$(PCC) $(CFLAGS) -c main.cpp

ansv: main.o ANSV.o
	$(PCC) $(CFLAGS) -o $@ $^
