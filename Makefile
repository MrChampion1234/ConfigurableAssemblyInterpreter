
CC=g++
CFLAGS=-Og -g3
ROOTDIR="/home/tye/code Folders/C++ programs/assemblyInterpreter"
OBJDIR=$(ROOTDIR)/objectfiles

compile:
	#cd ./objectfiles
	#pwd
	g++ $(CFLAGS) -o objectfiles/stringops.o -c stringops.cpp
	g++ $(CFLAGS) -o objectfiles/mainLib.o -c mainLib.cpp
	g++ $(CFLAGS) -o objectfiles/instructions.o -c instructions.cpp
	g++ $(CFLAGS) -o objectfiles/main.o -c main.cpp
	#cd ..

link:
	#ld -o main -I $(OBJDIR)/*.o
	g++ -o main $(OBJDIR)/*.o

all: compile link
