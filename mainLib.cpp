#include <string>
#include <cassert>
#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>
#include <algorithm>

#include "stringops.h"
#include "instructions.h"
#include "mainLib.h"

#ifndef MAINLIB_CPP
#define MAINLIB_CPP

// Function to handle getting a dereferenced value
int getDeref(Env &env, Arg arg1) {
	// Stores the argument's value if derefLevel == 0,
	// but will store the value found at each dereference level if derefLevel >= 1
	int lastval = env.memory.at(arg1.derefLevel);
	
	// Repeatedly dereference the value lastval while derefLevel >= 1 
	while (arg1.derefLevel >= 1) {
		lastval = env.memory.at(lastval); // Set lastval to the value the address lastval
		arg1.derefLevel--; // Decrement derefLevel
	}
	
	// lastval is now the n-th dereferenced value.
	return lastval;
}

// Same as above, but returns a pointer to the value.
// Used for setting a value with +=, ++ and other operators
int* getDerefp(Env &env, Arg arg1) {
	int* lastval = &env.memory.at(arg1.value);
	
	// Same as getDeref, but modified since lastval is int* and not int.
	while (arg1.derefLevel >= 1) {
		lastval = &env.memory.at(*lastval);
		arg1.derefLevel--;
	}
	
	return lastval;
}

// Function to handle setting a dereferenced value
Env setDeref(Env &env, Arg arg1, int newValue) {
	// Use getDerefp to get a pointer to the value
	int* val = getDerefp(env, arg1);
	*val = newValue;
	
	return env;
}

// Wrapper to get the value of the register 
// remove specifies whether the value in the register should 
// be removed after getting it. This means that the NULL_REGISTER
// flag gets set to true
int getReg(Env &env, bool remove) {
	if (remove) {
		env.states[NULL_REGISTER] = true;
	}
	return env.reg;
}

// Returns pointer to current register
int* getRegp(Env &env) {
	return &env.reg;
}

void setReg(Env &env, int value) {
	env.states[NULL_REGISTER] = true;
	
}

// Call the instruction func given the line struct and the current environment
void doInstruction(Line line, Env &env) {
	Op inst = line.operation;
	std::vector<Arg> args = line.arguments;
	
	(*line.func)(env, line.arguments);
}

// Returns enum given string of operation
Op getOpFromString(std::string op) {
	// Can't use a string in a switch-case, so I have to use
	// an ugly series of if-elses
	const char *cop = op.c_str();
	if (strcmp(cop, "nop") == 0) {
		return Op::NOP;
	} else if (strcmp(cop, "mov") == 0) {
		return Op::MOV;
	} else if (strcmp(cop, "cpf") == 0) {
		return Op::COPY_FROM;
	} else if (strcmp(cop, "cpt") == 0) {
		return Op::COPY_TO;
	} else if (strcmp(cop, "add") == 0) {
		return Op::ADD;
	} else if (strcmp(cop, "sub") == 0) {
		return Op::SUB;
	} else if (strcmp(cop, "inc") == 0) {
		return Op::INC;
	} else if (strcmp(cop, "dec") == 0) {
		return Op::DEC;
	} else if (strcmp(cop, "jmp") == 0) {
		return Op::JUMP;
	} else if (strcmp(cop, "jiz") == 0) {
		return Op::JUMP_IF_ZERO;
	} else if (strcmp(cop, "jlz") == 0) {
		return Op::JUMP_IF_NEGATIVE;
	} else if (strcmp(cop, "inp") == 0) {
		return Op::INP;
	} else if (strcmp(cop, "out") == 0) {
		return Op::OUT;
	} else if (strcmp(cop, "end") == 0) {
		return Op::END;
	} else {
		printf("Error, instruction %s not found\n", op.c_str());
		throw 'i';
	}
}

// Interprets a given argument as the default address argument
// Returns the Arg struct of the argument
Arg interpretArg(std::string argString) {
	//printf("\nEntering interpretArg\n");
	assert(argString.size() > 0);
	const char *argcstring = argString.c_str();
	// Count dereference level, using 0 if not dereferencing
	int count = 0;
	std::string snumber{ "", 0 };
	for (int i = 0; i < (int)argString.length(); ++i) {
		if (argString[i] == '*') {  // If char is the dereference operator
			count++;  // Increment count
		} else {
			//printf("adding char '%c' to snumber\n", argcstring[i]);
			//printf("%i\n", (int)snumber.length());
			// Add character to snumber to extract argument part
			snumber += argcstring[i];
			//printf("%i\n", (int)snumber.length());
		}
	}
	//printf("snumber is '%s'\n", snumber.c_str());
	// Convert snumber to integer
	int address = std::stoi(snumber);

	// Now, create and return the Arg struct
	Arg retval{
		.value = address,
		.derefLevel = count
	};
	//printf("Exiting interpretArg\n\n");
	return retval;
}

// For processing the operation and returning a Line from it
Line processOperation(Op operation, int lineNum, std::vector<std::string> stringArgs, Labelmap_t labelmap) {
	OpFunc opfunc = optofunc.at(operation);
	switch (operation) {
		case Op::JUMP: 
		case Op::JUMP_IF_ZERO:
		case Op::JUMP_IF_NEGATIVE: { // Using brackets because Atom doesn't autoindent after case statements
			// Get the label to jump to 
			std::string label = stringArgs[0];
			
			// Print the labelmap
			//printLabelMap(labelmap);
			
			// Check if label is in labelmap 
			//printf("Jump op references label '%s'\n", label.c_str());
			goto SKIP_CHECK;
			if (labelmap.find(label) == labelmap.end()) {
				printf("Error, label '%s' not found in labelmap\n", label.c_str());
				throw '0';
			}
			SKIP_CHECK:
			// Get the line that label is on 
			int label_lineNum = labelmap.at(label);
			Arg arg1 {
				label_lineNum,
				0
			};
			return Line {
				operation,
				opfunc,
				lineNum,
				1,
				std::vector<Arg>{ arg1 }
			};
			
		} case Op::LABEL: {
			// This is handled by interpretLine, so this case will never happen
			printf("Error, Op::LABEL should never be found in processOperation\n");
			throw 's';
		}
		
		
		default: {
			// Process it as if each arg was a possibly dereferenced address 
			std::vector<Arg> args;
			args.reserve(2);
			
			for (std::string s : stringArgs) {
				args.push_back(interpretArg(s));
			}
			
			return Line {
				operation,
				opfunc,
				lineNum,
				static_cast<int>(args.size()),
				args
			};
		}
	}
}

// Interpret line of file and return a Line struct
Line interpretLine(std::string line, int lineNum, Labelmap_t labelmap) {
	//printf("Entering interpretLine\n");
	// First, get rid of single line comments, which should start with "//"
	// This is done first since whitespace may come before a comment, and hence won't be 
	// removed by stripends
	if (line.empty()) {
		// Nothing here, so ignore the instruction
		return Line {
			Op::NO_INSTRUCTION,
			optofunc.at(Op::LABEL), // No instruction should act as a label
			lineNum,
			0,
			std::vector<Arg> {}
		};
	}
	// Find the first occurence of "//"
	size_t ind = line.find("//");
	if (ind != line.npos) {  // If "//" was found
		line.erase(ind);     // Erase from "ind" to end, 
	}
	line = stripends(line, ' ');
	line = stripends(line, '\t');
	
	// Check if the current line ends in colon, to see if it's a label 
	if (line.back() == ':') {
		// A label should be the same as a nop instruction
		// Since a label shouldn't do anything.
		return Line {
			Op::LABEL,
			optofunc.at(Op::NOP),
			lineNum,
			0, // No arguments
			std::vector<Arg>{ }
		};
	}
	
	stringPair_t tempPair;
	std::string current_arg;
	Op operation;
	std::vector<std::string> stringArgs{};
	stringArgs.reserve(2);
	int argCount = 0;
	int i=0;
	while (!line.empty()) {
		//printf("current string is '%s'\n", line.c_str());
		if (i > 10) {
			printf("interpretLine loop has gone on for more \
			than 10 iterations, throwing exception and exiting\n");
			throw 'l';
		}
		// Split string into two parts by space
		tempPair = splitOnFirstChar(line, ' ');
		
		//printf("('%s' . '%s')\n", tempPair.first.c_str(), tempPair.second.c_str());
		// Current argument should be the car of the pair
		current_arg = stripends(tempPair.first, ' ');
		
		// Find out what current_arg should be
		if (argCount == 0) { // current_arg is the operation
			operation = getOpFromString(current_arg);
		} else { // current_arg is an argument to the operation
			stringArgs.push_back(current_arg);
		}
		argCount++;
		if (tempPair.second.empty()) {
			break;
		} 
		line = stripends(tempPair.second, ' ');
		
		i++;
	}
	//printLabelMap(labelmap);
	//printf("Exiting interpretLine\n");
	return processOperation(operation, lineNum, stringArgs, labelmap);
}

void printLabelMap(Labelmap_t labelmap) {
	for (Labelmap_t::iterator it=labelmap.begin(); it!=labelmap.end(); ++it) {
		//std::cout << it->first << " => " << it->second << '\n';
		printf("'%s' => '%i'\n", it->first.c_str(), it->second);
	}
}

// Make a labelmap, which has type map<string, int>
Labelmap_t makeLabelMap(std::string filename) {
	std::ifstream ifs;
	ifs.open(filename);
	std::string line;
	std::getline(ifs,line);
	
	// This section is to skip past the configuration if it exists
	if (line.compare("ENVDEF") == 0) {
		while (line.compare("ENDENVDEF") != 0) {
			std::getline(ifs,line);
		}
	}
	
	// Create an empty map 
	Labelmap_t labelmap;
	int lineNum = 0;
	while (static_cast<int>(ifs.tellg()) != -1) {
		//printf("tellg() = %i, lineNum = %i\n", (int)inputfile.tellg(), lineNum);
		
		std::getline(ifs,line);
		// Strip line of whitespace characters
		line = stripends(line, ' ');
		line = stripends(line, '\t');
		
		// Check if line is a label 
		if (line.back() == ':') {
			//printf("Found label '%s'\n", line.c_str());
			line.pop_back();           // Get rid of colon at end
			//printf("Adding label '%s'\n", line.c_str());
			labelmap[line] = lineNum;  // Add label to map
		}
		lineNum++;
	}
	ifs.close();
	//printf("Printing labelmap in makeLabelMap\n");
	//printLabelMap(labelmap);
	return labelmap;
}

// Interprets a given filename as a program, and returns a program struct from it
// "offset" is so that the environment config can be skipped
Program interpretFile(std::string filename, std::streampos offset) {
	printf("Entering interpretFile\n");
	
	// First, make a labelmap
	Labelmap_t labelmap;
	labelmap = makeLabelMap(filename);
	// try {
	// 	Labelmap_t labelmap = makeLabelMap(filename);
	// } catch (const std::exception& e) {
	// 	printf("An error has occured in makeLabelMap\n");
	// 	throw e;
	// }
	//printf("printing labelmap in interpretFile\n");
	//printLabelMap(labelmap);
	// Open file
	std::ifstream ifs;
	ifs.open(filename, std::ifstream::in);
	
	ifs.seekg(0, ifs.end);
	int length = ifs.tellg();
	ifs.seekg(0, ifs.beg);
	
	// Ignore the first offset number of chars	
	ifs.ignore(offset);
	
	// Create a vector of Lines to make a Program struct with 
	std::vector<Line> lines;
	// Estimate number of lines needed
	// Code block to keep the variables scope limited
	{
		int linesNeeded = ceil(length / 5);
		if (linesNeeded < 4) { // Guarantee at least 4 lines are reserved
			linesNeeded = 4;
		}
		lines.reserve(linesNeeded);
	}
	// Now, for each line in the file, interpret it, and add it to the vector 
	std::string cline = {""};
	Labelmap_t prev = labelmap;
	Line temp;
	int i = 0;
	if (static_cast<int>(ifs.tellg()) == -1) {
		printf("Error, already at end of file in interpretFile, exiting\n");
		throw 'e';
	}
	while (static_cast<int>(ifs.tellg()) < length &&
		   static_cast<int>(ifs.tellg()) != -1) {
		if (i > length) {
			printf("Error, i is larger than the file's size, meaning something is wrong, exiting\n");
			throw 's';
		}
		std::getline(ifs,cline);
		cline = stripends(cline, ' ');
		cline = stripends(cline, '\n');
		if (cline.compare("\n") == 0) {  // Skip over any empty lines
			continue;
		}
		if (strcmp(cline.c_str(), "ENDPROGRAM") == 0) { // Now at end of the program
			//printf("Breaking\n");
			break;
		}
		prev = labelmap;
		temp = interpretLine(cline, i, labelmap);
		//assert(prev == labelmap);
		if (temp.operation == Op::NO_INSTRUCTION) {
			// Ignore the current line
			i++;
			continue;
		} else {
			lines.push_back(temp);
		}
		
		i++;
	}
	ifs.sync();
	ifs.close();
	printf("Exiting interpretFile\n");
	return Program {
		lines
	};
}

// Setup the environment
Env setupEnvironment(EnvConfig config, Program prog) {
	printf("Entering setupEnvironment\n");
	std::vector<int> mem;
	assert(config.memSize > 0);
	mem.reserve(config.memSize);
	// Initialize mem with zeros
	for (int i = 0; i < config.memSize; i++) {
		mem.push_back(0);
	}
	
	// Put initial memory in mem
	for (int i = 0; i < (int)config.initialMemory.size(); ++i) {
		mem[i] = config.initialMemory[i];
	}
	Env env {config.reg, config.line, config.memSize, mem, prog};
	
	// Reserve 16 flags for the states vector, since it's ultra space efficient
	// the 16 flags should be nothing
	env.states.reserve(16);
	
	//printf("size of memory is %i\n", mem.size());
	printf("Exiting setupEnvironment\n");
	return env;
}

std::pair<EnvConfig,std::streampos> makeEnvConf(std::string filename) {
	printf("Entering makeEnvConf\n");
	std::ifstream ifs;
	ifs.open(filename, std::ifstream::in);
	// Get length of file 
	//ifs.seekg(0, ifs.end);
	//int length = ifs.tellg();
	//ifs.seekg(0, ifs.beg);
	
	
	
	// Check for start of environment definition 
	std::string cline;
	std::getline(ifs, cline);
	stringPair_t varValPair;
	std::string var,val; 
	bool inDef = false;
	
	// Vector of bools to keep track of which values were set by the file
	std::vector<bool> setVals;
	
	enum setValsInds { 
		MEMSIZE,
		STARTLINE,
		STARTREG,
		INIT_MEM,
		INPUT,
		OUTPUT,
		END_OF_ENUM
	};
	setVals.reserve(END_OF_ENUM);
	// Initialize values of vector to false
	for (int i=0; i<END_OF_ENUM; i++) {setVals[i]=false;} 
	
	std::vector<int> memInit;
	EnvConfig envconf;
	
	// Read configuration
	while (cline.compare("ENDENVDEF") != 0) {
		cline = stripends(cline, ' ');
		cline = stripends(cline, '\t');
		if (cline.compare("ENVDEF") == 0) {
			inDef = true;
		} else if (cline.compare("ENDENVDEF") == 0) { // At end of configuration
			break; 
		} else if (inDef && !cline.empty()) {
			// Now, search for some var=value pairs
			if (cline.find('=') != cline.npos) {
				varValPair = splitOnFirstChar(cline, '=');
				var = varValPair.first;
				val = varValPair.second;
				
				// Strip whitespace from ends of var and val
				var = stripends(var, ' ');
				val = stripends(val, ' ');
				
				// Now, "switch" with the var to see what val actually is
				if (var.compare("msize") == 0 ||
					var.compare("size") == 0) { // Specifies size of memory
					envconf.memSize = stoi(val);
					setVals[MEMSIZE] = true;
					// I could initialize the array now, but I'll wait until 
					// after processing all the config vars. 
					// This is because if I initialize the array here, then ideally 
					// I should only initialize the array here. Which means that 
					// I can't do it later. But, if msize isn't passed yet init is,
					// Then I have to initialize the array then. But, I would have already initialized
					// the array here, thus making it a waste. 
					// So, I'll do the initialization after since it's much easier to do it then
					
				} else if (var.compare("startline") == 0) { // Specifies starting line 
					envconf.line = stoi(val);
					setVals[STARTLINE] = true;
					
				} else if (var.compare("startreg") == 0) {  // Specifies starting register value
					envconf.reg = stoi(val);
					setVals[STARTREG] = true;
					
				} else if (var.compare("init") == 0) { // Specifies starting memory
					envconf.initialMemory = processArrayString(val);
					setVals[INIT_MEM] = true;
				
				} else if (var.compare("input") == 0) {
					printf("Taking input '%s'\n", val.c_str());
					std::vector<int> temp = processArrayString(val);
					for (int i : temp) {
						envconf.input.push(i);
					}
				
				} else {
					printf("Error, config var '%s' is not a configuration variable, exiting\n", var.c_str());
					throw 's';
				}
			}
		}
		
		// Update current line
		std::getline(ifs, cline);
	}
	assert(strcmp(cline.c_str(),"ENDENVDEF") == 0);
	assert((int)ifs.tellg() != -1);
	// Save the offset for later when interpreting the program
	std::streampos offset = ifs.tellg();
	ifs.sync();
	ifs.close(); // Close file since it's no longer needed
	
	// Figure out what the memory size should be given what values were set
	if (setVals[MEMSIZE]) {
		// Memsize was set, so now check if initmem's size is greater than the memsize 
		if ((int)envconf.initialMemory.size() > envconf.memSize) {
			printf("Error, initial memory of size %i cannot fit in \
				memory of size %i, exiting\n", (int)envconf.initialMemory.size(), envconf.memSize);
			throw 's';
		}
	} else if (setVals[INIT_MEM]) {
		// Memsize was not set, but initmem was. So, set the memory size to be exactly 
		// equal to initmem's size
		envconf.memSize = (int)envconf.initialMemory.size();
	} else {
		// None were passed, so set it to a default of 100
		envconf.memSize = 100;
	}
	assert(envconf.memSize > 0);
	
	// Check if things like reg and line number aren't set. If not, set them 
	// to the default of 0
	if (!setVals[STARTLINE]) {
		envconf.line = 0;
	}
	if (!setVals[STARTREG]) {
		envconf.reg = 0;
	}
	
	printf("Exiting makeEnvConf\n");
	return std::make_pair(envconf, offset);
}

Env createEnvironmentFromFile(std::string filename) {
	printf("Entering createEnvironmentFromFile\n");
	std::pair<EnvConfig,std::streampos> tempPair = makeEnvConf(filename);
	EnvConfig envconf     = tempPair.first;
	std::streampos offset = tempPair.second;
	
	// Process file into program struct. Pass the offset found earlier so it can skip the environment config
	//printf("Offset is %i\n", (int)offset);
	Program prog = interpretFile(filename, offset);
	//printf("interpretFile returned okay\n");
	
	printf("Exiting createEnvironmentFromFile\n");
	// Finally, setup the environment with the given environment config and program struct
	return setupEnvironment(envconf, prog);
}

Env iterateOnce(Env &env) {
	// Get the Line struct representing the current line 
	Program program = env.program;
	Line cline;
	//printf("current line is %i and size of program is %i\n", env.line, (int)program.lines.size());
	if (env.line >= (int)program.lines.size()) {
		env.endProgram = true;
		goto RETURN;
	}
	//printf("getting current line\n");
	cline = program.lines.at(env.line);
	
	// Execute the correct function given the line
	//printf("executing instruction %i\n", static_cast<int>(cline.operation));
	doInstruction(cline, env);
	RETURN:
	//printf("Exiting iterateOnce\n");
	return env;
}

// Prints the state of a given environment.
void printState(Env env) {
	//printf("Entering printState\n");
	printf("memorySize is %i\n", (int)env.memory.size());
	// print the current line num and register value
	printf("ISEND: %s ACC: %i  LINE: %i - MEM: [", (env.endProgram ? "true" : "false"), env.reg, env.line);
	
	// Print the memory
	for (int i = 0; i < (int)env.memory.size(); ++i) {
		if ((i+1) == (int)env.memory.size()) {
			printf("%i]\n", env.memory[i]);
		} else {
			printf("%i, ", env.memory[i]);
		}
	}
	//printf("Exiting printState\n");
}

Env runProgram(std::string filename) {
	Env env = createEnvironmentFromFile(filename);
	printf("createEnvironmentFromFile returned okay\n");
	int i = 0;
	while (!env.states[IS_END]) {
		//printf("Iterating once\n");
		iterateOnce(env);
		//if (env.line > 1 && env.program.lines[env.line-1].operation == Op::END) {
		//	printf("Program should end after this\n");
		//}
		printState(env);
		if (i > 10) {
			printf("Too many iterations, exiting\n");
			break;
		}
	}
	//printf("Exiting runProgram\n");
	return env;
}

#endif
