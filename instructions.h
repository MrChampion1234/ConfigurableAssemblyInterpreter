// -*- grammar-ext: .cpp -*-
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
#include <vector>

#include "instructionsEnum.h"
#include "mainLib.h"

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

//using OpFunc = std::function<void(Env&,std::vector<Arg>)>;
using OpFunc = void(*)(Env&,std::vector<Arg>);
using OpToFuncmap_t = std::map<Op,OpFunc>;
using strToOpmap_t = std::map<std::string,Op>;

//OpToFuncmap_t optofunc;
//strToOpmap_t strtoop;

void nop(Env &env, std::vector<Arg> args);
void label(Env &env, std::vector<Arg> args);

void mov(Env &env, std::vector<Arg> args);

void cpf(Env &env, std::vector<Arg> args);
void cpt(Env &env, std::vector<Arg> args);

void add(Env &env, std::vector<Arg> args);
void sub(Env &env, std::vector<Arg> args);

void inc(Env &env, std::vector<Arg> args);
void dec(Env &env, std::vector<Arg> args);
void endprog(Env &env, std::vector<Arg> args);

void jmp(Env &env, std::vector<Arg> args);
void jiz(Env &env, std::vector<Arg> args);
void jlz(Env &env, std::vector<Arg> args);

void inp(Env &env, std::vector<Arg> args);
void out(Env &env, std::vector<Arg> args);

// Use this to create a map from the OP enum class to the func
const OpToFuncmap_t optofunc {
	{Op::NOP, nop},
	{Op::LABEL, label},
	{Op::MOV, mov},
	{Op::COPY_FROM, cpf},
	{Op::COPY_TO, cpt},
	{Op::ADD, add},
	{Op::SUB, sub},
	{Op::INC, inc},
	{Op::DEC, dec},
	{Op::JUMP, jmp},
	{Op::JUMP_IF_ZERO, jiz},
	{Op::JUMP_IF_NEGATIVE, jlz},
	{Op::INP, inp},
	{Op::OUT, out},
	{Op::END, endprog}
};

// A map from the string of an operation to the enum class OP
// Note, some of these, namely the label, will need to be treated specially
const strToOpmap_t strtoop {
	{"nop", Op::NOP},
	{"*:", Op::LABEL},
	{"mov", Op::MOV},
	{"cpf", Op::COPY_FROM},
	{"cpt", Op::COPY_TO},
	{"add", Op::ADD},
	{"sub", Op::SUB},
	{"inc", Op::INC},
	{"dec", Op::DEC},
	{"jmp", Op::JUMP},
	{"jiz", Op::JUMP_IF_ZERO},
	{"jlz", Op::JUMP_IF_NEGATIVE},
	{"inp", Op::INP},
	{"out", Op::OUT},
	{"end", Op::END}
};

enum State {
	IS_END,          // For when the program has ended
	NULL_REGISTER    // Means the register is null. This can't be done normally
};

#endif
