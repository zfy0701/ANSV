CC = /usr/bin/g++
CFLAGS = -O2

ifdef OPENMP
PCC = /usr/bin/g++
CFLAGS = -fopenmp -O2 -DOPENMP

else ifdef CILK
PCC = cilk++
CFLAGS = -O2 -DCILK -Wno-cilk-for

else ifdef IPPROOT
PCC = icpc
CFLAGS = -O2 -DCILKP

else 
PCC = /usr/bin/g++
CFLAGS = -fopenmp -O2 -DOPENMP
endif


all: ansv 
#ansv2 sa

.PHONY: clean

clean: 
	rm -rf ansv *~ *.o

ANSV.o: ANSV.cpp ANSV.h
	$(PCC) $(CFLAGS) -c $<

main.o: main.cpp ANSV.o
	$(PCC) $(CFLAGS) -c main.cpp

ansv: main.o ANSV.o
	$(PCC) $(CFLAGS) -o $@ $^
