
CXX=g++
INCLUDES=-I include/

INCLUDEDIR=./include/
VPATH=$(INCLUDEDIR)

SRCDIR=./src/
BINDIR=./bin/

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
FLAGS=-c -g -L/opt/lib
LIBS=-lm
endif
ifeq ($(UNAME), Darwin)
FLAGS=-c -g
LIBS=-llo
endif

BEACONOBJS = beacon.o

all: beacon

beacon: $(BEACONOBJS)
	$(CXX) -o $(BINDIR)beacon $(BEACONOBJS) $(LIBS) $(INCLUDE)
	
beacon.o: $(SRCDIR)beacon.cpp
	$(CXX) $(FLAGS) $(INCLUDES) $(SRCDIR)beacon.cpp

clean:
	rm -f *~ *# *.o beacon	
	rm -f *~ *# *.o $(BINDIR)beacon	
	