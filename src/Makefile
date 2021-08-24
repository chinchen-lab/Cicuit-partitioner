#
# Makefile for FM_Partitioner
#
CC=g++
# if you want to use debugger, add -g to CFLAGS and LDFLAGS
CFLAGS=-Wall -std=c++11 -Ofast
INCLUDES=-I../src/
# INCLUDES=-I../include/
HEADERS=part_graph.h
LFLAGS=
# LFLAGS=-L../lib/
#LIBS=-lm -lsystemc
# LIBS=-lm -lgurobi_c++ -lgurobi70 -fopenmp
SOURCES=main.cpp part_graph.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=../bin/FM_Partitioner

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJECTS) -o $@ $(LFLAGS) $(LIBS)

%.o:  %.cpp  $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ 

clean:
	rm -rf *.o $(EXECUTABLE)
