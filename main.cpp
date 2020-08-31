/*	
 *	This file is a part of ConfigurableAssemblyIntepreter.
 *	
 *	ConfigurableAssemblyIntepreter is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ConfigurableAssemblyIntepreter is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
//#include "environment.h"
//#include "instructions.h"
#include <cassert>
#include <cstdio>

#include "instructions.h"
#include "mainLib.h"
#include "stringops.h"

void printArray(int arr[], int size) {
	for (int i = 0; i < size; ++i) {
		if ((i+1) == size) {
			printf("%i]\n", arr[i]);
		} else {
			printf("%i, ", arr[i]);
		}
	}
}

void printVector(std::vector<int> vec) {
	printf("[");
	for (std::vector<int>::iterator it = vec.begin() ; it != vec.end(); ++it){
		printf(" %i", *it);
	}
	printf("]\n");
}



// Prints the parameters of a given line struct 
// For debugging purposes
void printLine(Line line) {
	printf("%i %i [", static_cast<int>(line.operation), line.lineNum);
	Arg carg;
	for (int i = 0; i < line.numArgs; ++i) {
		carg = line.arguments[i];
		if ((i+1) == line.numArgs) {
			printf("(%i,%i)]\n", carg.value, carg.derefLevel);
		} else {
			printf("(%i,%i), ", carg.value, carg.derefLevel);
		}
	}
}

/*int testinstructions() {
	std::vector<int> init {1, 2, 3};
	Env env = setupEnvironment(5, init);
	printState(env);
	
	std::vector<Arg> args {
		{ .value = 0, .derefLevel = 0 },
		{ .value = 1, .derefLevel = 0 }
	};
	
	mov(env, args);  // Move memblock 0 to memblock 1
	printState(env);
	// Assert that memblock 0 must be equal to memblock 1
	assert((env.memory[0] == env.memory[1]) && "mov instruction failed to copy 0 to 1");
	
	args[0] = { .value = 0, .derefLevel = 0 };
	cpf(env, args);
	printState(env);
	assert((env.reg == env.memory[0]) && "cpf instruction failed to copy 0 to register");
	
	//delete[] init;
	printf("Finished instructions test\n");
	return 0;
}*/

int testInterpreter() {
	printf("Testing interpreter\n");
	Labelmap_t temp;
	Line l1 = interpretLine(static_cast<std::string>("add 0 1"), 0, temp);
	printLine(l1);
	Line l2 = interpretLine(static_cast<std::string>("mov *0 5"), 1, temp);
	printLine(l2);
	Line l3 = interpretLine(static_cast<std::string>("sub **0 120"), 2, temp);
	printLine(l3);
	
	/*Program p1 = interpretFile("test.asm");
	printf("p1 lines\n");
	for (int i = 0; i < (int)p1.lines.size(); i++) {
		printLine(p1.lines[i]);
	}*/
	temp = makeLabelMap("tests/jmptest.asm");
	printLabelMap(temp);	
	
	Env env = createEnvironmentFromFile("test.asm");
	printf("Starting program\n");
	printState(env);
	iterateOnce(env);
	printState(env);
	iterateOnce(env);
	printState(env);
	
	//env = createEnvironmentFromFile("test.asm");
	
	//env = runProgram("test.asm");
	
	int i=0;
	while (!env.endProgram) {
		iterateOnce(env);
		printState(env);
		if (i > 10) {
			printf("Too many iterations, exiting\n");
			throw 'i';
		}
	}
	printf("\nStarting jmptest.asm\n");
	env = runProgram("tests/jmptest.asm");
	printState(env);
	
	printf("Finished interpreter test\n");
	return 0;
}

int testStringops() {
	std::string str1 = std::string{"  test  ", 8};
	printf("str1 orig: '%s'\n", str1.c_str());
	str1 = stripends(str1, ' ');
	printf("str1 new : '%s'\n", str1.c_str());
	
	std::string str2{ "0 1" };
	stringPair_t p1 = splitOnFirstChar(str2, ' ');
	printf("div1: ('%s' . '%s')\n", p1.first.c_str(), p1.second.c_str());
	stringPair_t p2 = splitOnFirstChar(p1.second, ' ');
	printf("div2: ('%s' . ('%s' . '%s'))\n", p1.first.c_str(), p2.first.c_str(), p2.second.c_str());
	
	// Test the array string interpreter 
	std::string str3 {"[0, 1,2,1]"};
	std::vector<int> v = processArrayString(str3);
	printVector(v);
	
	printf("Finished stringops test\n");
	return 0;
}

// The GNU GPL v3.0 license
std::string license = 
R"LICENSE(ConfigurableAssemblyInterpreter is exactly what you'd expect, a configurable assembly intepreter
Copyright (C) 2020 Tye Paquette

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
)LICENSE";

int main(int argc, char const *argv[], char *envp[]) {
	// Test the tellg function to see if any information can be gained from it 
	/*
	std::ifstream ifs;
	ifs.open("test.asm");
	int i = ifs.tellg();
	ifs.seekg(0, std::ifstream::end);
	int e = ifs.tellg();
	ifs.close();
	printf("%i : %i\n", i, e);
	
	//testinstructions();
	//testStringops();
	testInterpreter();
	*/
	if (argc < 2) {
		printf("Please input a file name\n");
	} else {
		// Print GNU GPL v3.0 license
		if (argv[1] == "-v") {
			printf("%s\n", license.c_str());
		}
		std::string filename = argv[1];
		printState(runProgram(filename));
	}
	
	
	return 0;
	
}
