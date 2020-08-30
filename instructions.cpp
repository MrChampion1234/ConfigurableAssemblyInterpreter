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
#include <functional>
#include <map>
#include <vector>
#include "instructions.h"
#include "instructionsEnum.h"
#include "mainLib.h"


#ifndef INSTRUCTIONS
#define INSTRUCTIONS

/*
	multilevel dereferencing is best explained by first talking about dereferencing.

	A dereference works by instead referencing to the value pointed to by the value in "a".
	For example, the following will add a the numbers [1, 2, 3] by using dereferencing if the
	memory layout is like this. Note, "?" means it's undefined, and the numbers above are the addresses
	 0 1 2 3 4 5 6
	[1 2 3 0 ? ? ? ...]

	"
	loop:
	  cpf 4*
	  jiz B
	  cpf 5
	  add 4*
	  cpt 5
	  jmp loop
	B:
	out 5
	"
	I may add the option to use [a] for a single dereference

	So, where a single dereference can be viewed as "a" pointing to another value "b", like so,
	a -> b
	multilevel dereferencing can be viewed like this,
	(a -> b) -> c

	Here, "a" points to "b", but "b" points to "c". The only reason for the brackets is to show the
	values that are found first. First, the value that "a" points to is found, which is "b", then the
	value that "b" points to is found, which ends up being "c".

	And you can go on with this. You can have a 4 level deep dereference, 5 level deep dereference, even 120 level deep dereference.
	The list goes on.
*/

/*
	NOTE: Each function MUST return the env* and also increment the line number. The line number increment is so that
	interpreter can safely rely on the new line number being the actual new line number. This is also so that jmp
	instructions which change the line number can work, as well as jmp instructions that specify a relative line number
	(ie. "jmp -3" means "jump to the current line number - 3"), and ones that specify an absolute line number
	(ie. "jmp 14" means "jump to line 14")

	Since my intention with this is to create a general assembly interpreter, I must allow for as much flexibility as possible.
	
	* This has been changed. All functions can now technically return anything they want. The code which handles this 
	discards any value which is returned.
*/


void nop(Env &env, std::vector<Arg> args) {
	// Do nothing
	env.line++;     // Move to next line
	env.steps++;    // Increment steps
}

void label(Env &env, std::vector<Arg> args) {
	// Same as nop, but without incrementing steps 
	env.line++;
}

void mov(Env &env, std::vector<Arg> args) {

	int val1 = getDeref(env, args[0]);     // Get the possibly dereferenced value from the first argument
	setDeref(env, args[1], val1);          // Set the possibly dereferenced value to the referenced value from the first argument

	env.line++;    // Move to next line
	env.steps++;
}

void cpf(Env &env, std::vector<Arg> args) {
	env.reg = getDeref(env, args[0]);

	env.line++;
	env.steps++;
	//return env;
}

void cpt(Env &env, std::vector<Arg> args) {
	setDeref(env, args[0], env.reg);  // Set memory block at arg1 to the value in the register

	env.line++;
	env.steps++;
	//return env;
}

void add(Env &env, std::vector<Arg> args) {
	int val = getDeref(env, args[0]);
	env.reg += val;                   // Add value in memory to current register

	env.line++;
	env.steps++;
	//return env;
}

void sub(Env &env, std::vector<Arg> args) {
	int val = getDeref(env, args[0]);
	env.reg -= val;  // Set reg = reg - val;
	
	env.line++;
	env.steps++;
}

void inc(Env &env, std::vector<Arg> args) {
	int *val = getDerefp(env, args[0]); // Get pointer to value to use ++ operator
	// Increment the value val is pointing to
	val++;
	env.line++;
	env.steps++;
	//return env;
}

void dec(Env &env, std::vector<Arg> args) {
	int *val = getDerefp(env, args[0]);
	
	val--;
	env.line++;
	env.steps++;
	//return env;
}

void jmp(Env &env, std::vector<Arg> args) {
	// For jmp, the args will actually be different
	// The value of the arg will be the line number to jump to
	// instead of a memory address
	int val = args[0].value;
	env.line = val;          // Set current line number to argument
	env.steps++;
}

void jiz(Env &env, std::vector<Arg> args) {
	if (env.reg == 0) {
		env.line = args[0].value;
	} else {
		env.line++;
	}
	env.steps++;
}

void jlz(Env &env, std::vector<Arg> args) {
	if (env.reg < 0) {
		env.line = args[0].value;
	} else {
		env.line++;
	}
	env.steps++;
}

// Sets the current register to the number of input values left
void gis(Env &env, std::vector<Arg> args) {
	setReg(env, env.input.size());
	env.line++;
	env.steps++;
}

void inp(Env &env, std::vector<Arg> args) {
	// Get input value, and set current register to it
	setReg(env, env.input.front()); env.input.pop();
	env.line++;
	env.steps++;
}

void out(Env &env, std::vector<Arg> args) {
	env.output.push(getReg(env, true));
}

void endprog(Env &env, std::vector<Arg> args) {
	// Set endprogram flag to true
	env.states[IS_END] = true;
}

#endif
