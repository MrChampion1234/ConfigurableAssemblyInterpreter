// -*- grammar-ext: .cpp -*-
#ifndef INSTRUCTIONS_ENUM_H
#define INSTRUCTIONS_ENUM_H

enum class Op {
	NOP,                // 0   "nop"      Do nothing
	NO_INSTRUCTION,     // 1   Special opcode for when the interpreter should ignore the current line
	MOV,                // 2   "mov a b"  Move value in memory address a to value in memory slot b
	COPY_FROM,          // 3   "cpt a"    Copy value from memory address a to acc
	COPY_TO,            // 4   "cpf a"    Copy value to memory address a to acc
	ADD,                // 5   "add a"    Add value from memory address a to acc
	SUB,                // 6   "sub a"    Subtract a from acc. ie, Do the operation, acc = (acc - a).
	INC,                // 7   "inc a"    ++a
	DEC,                // 8   "dec a"    --a
	JUMP,               // 9   "jmp"      Jump to label a.
	JUMP_IF_ZERO,       // 10  "jiz"      Jump to label a if acc is zero
	JUMP_IF_NEGATIVE,   // 11  "jin"      Jump to label a if acc is less than zero
	INP,                // 12  "inp"      Get one input value and store to acc
	OUT,                // 13  "out"      Output one value
	END,                // 14  "end"      for when the program has ended
	LABEL               // 15  "*:"       For completeness, a label is considered an operation
};

#endif