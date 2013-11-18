
CXX=g++
INCLUDES=-I include/

INCLUDEDIR=./include/
VPATH=$(INCLUDEDIR)

SRCDIR=./src/
BINDIR=./bin/

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
FLAGS=-D__UNIX_JACK__ -c -g -L/opt/lib
LIBS=-lasound -lpthread -ljack -lstdc++ -lm \
	-lGL -lGLU -lglut -llo 
endif
ifeq ($(UNAME), Darwin)
FLAGS=-D__MACOSX_CORE__ -c -g
LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
	-framework IOKit -framework Carbon  -framework OpenGL \
	-framework GLUT -framework Foundation \
	-framework AppKit -lstdc++ -lm -llo
endif

SERVEROBJS = osc-server.o 
CLIENTOBJS = osc-client.o

all: beacon osc-server osc-client

beacon: $(CLIENTOBJS)
	$(CXX) -o $(BINDIR)beacon $(CLIENTOBJS) $(LIBS) $(INCLUDE)
	
osc-server: $(SERVEROBJS)
	$(CXX) -o $(BINDIR)osc-server $(SERVEROBJS) $(LIBS) $(INCLUDE)

osc-client: $(CLIENTOBJS)
	$(CXX) -o $(BINDIR)osc-client $(CLIENTOBJS) $(LIBS) $(INCLUDE)


beacon.o: $(SRCDIR)beacon.cpp
	$(CXX) $(FLAGS) $(INCLUDES) $(SRCDIR)beacon.cpp

osc-server.o: $(SRCDIR)osc-server.cpp
	$(CXX) $(FLAGS) $(INCLUDES) $(SRCDIR)osc-server.cpp

osc-client.o: $(SRCDIR)osc-client.cpp
	$(CXX) $(FLAGS) $(INCLUDES) $(SRCDIR)osc-client.cpp

# OBJS=  RtAudio.o chuck_fft.o Thread.o Stk.o VoxeLib.o voxelMeter.o
# 
# voxelMeter: $(OBJS)
# 	$(CXX) -o voxelMeter $(OBJS) $(LIBS)
# 	
# voxelMeter.o: voxelMeter.cpp RtAudio.h
# 	$(CXX) $(FLAGS) voxelMeter.cpp
# 	
# VoxeLib.o: VoxeLib.h VoxeLib.cpp
# 	$(CXX) $(FLAGS) VoxeLib.cpp
# 
# chuck_fft.o: chuck_fft.h chuck_fft.c
# 	$(CXX) $(FLAGS) chuck_fft.c
# 	
# Thread.o: Thread.h Thread.cpp
# 	$(CXX) $(FLAGS) Thread.cpp
# 	
# Stk.o: Stk.h Stk.cpp
# 	$(CXX) $(FLAGS) Stk.cpp
# 	
# RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
# 	$(CXX) $(FLAGS) RtAudio.cpp

clean:
	rm -f *~ *# *.o osc-server
	rm -f *~ *# *.o $(BINDIR)osc-server
	rm -f *~ *# *.o osc-client
	rm -f *~ *# *.o $(BINDIR)osc-client
	rm -f *~ *# *.o beacon	
	rm -f *~ *# *.o $(BINDIR)beacon	
	