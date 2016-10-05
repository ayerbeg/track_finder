
ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs) 

CFLAGS = -Wall $(ROOTFLAGS) 
LIBS = $(ROOTLIBS) 

PROG = TrackFinder 
CXX = chain_finder_main.C 

#PROG = MirrorFinder 
#CXX = mirror_finder.C 


all: $(PROG) 

$(PROG): $(CXX) 
	g++ -mcmodel=medium $(CFLAGS) $(CXX) $(LIBS) -o $(PROG) -std=gnu++11
