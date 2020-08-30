// -*- grammar-ext: .cpp -*-
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <queue>
#include <functional>

#include "instructionsEnum.h"

#ifndef MAINLIB_H
#define MAINLIB_H

using Labelmap_t = std::map<std::string, int>;

struct Env;

struct Arg {
	int value;
	int derefLevel;
};

// Struct to store a line of the file
struct Line {
	Op operation;
	void (*func)(struct Env &env, std::vector<Arg> args);
	int lineNum;
	int numArgs;
	std::vector<Arg> arguments;
};

// Struct to store all the lines of a program
// I know making a struct just to store an array is inefficient, but
// the point of it is to allow for extensibility in case I add more features
struct Program {
	std::vector<Line> lines;
};

// For reading and storing the environment configuration
struct EnvConfig {
	int reg;
	int line;
	int memSize;
	std::vector<int> initialMemory;
	std::queue<int> input;
	std::queue<int> output;
};

// This is a struct that will contain the current state of the program
// This is so it can be passed into functions and still work.
struct Env {
	int reg;          // The acc register
	int line;         // The line number it's on
	int memSize;      // This will be to do boundary checks
	//int *memory;      // This will be a dynamically allocated region of memory for "cpt" and "cpf" operations
	std::vector<int> memory; // Switching to a vector object
	Program program;
	int steps { 0 };           // To keep track of how many steps the program is taking
	std::vector<bool> states;  // This will allow for a general set of states to be set for whatever reason
	bool endProgram{false};  // The end instruction will make this true
	std::queue<int> input;
	std::queue<int> output;
};

void doInstruction(Line line, Env &env);

int  getReg(Env &env, bool remove=false);
int* getRegp(Env &env);
void setReg(Env &env, int value);

int  getDeref(Env &env, Arg arg1);
int* getDerefp(Env &env, Arg arg1);
Env setDeref(Env &env, Arg arg1, int newValue);

Op getOpFromString(std::string op);
Line interpretLine(std::string line, int lineNum, Labelmap_t labelmap);

void printLabelMap(Labelmap_t labelmap);
Labelmap_t makeLabelMap(std::string filename);
Program interpretFile(std::string filename, std::streampos offset);

std::pair<EnvConfig,std::streampos> makeEnvConf(std::string filename);
Env setupEnvironment(int memSize, std::vector<int> init);
Env createEnvironmentFromFile(std::string filename);
Env iterateOnce(Env &env);

void printState(Env env);
Env runProgram(std::string filename);

#endif
