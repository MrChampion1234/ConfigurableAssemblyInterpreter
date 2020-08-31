# ConfigurableAssemblyInterpreter
This is a little C++ project I started. I started it because I couldn't find an assembly interpreter that could have operations added to it to make it extensible. It's basically just an interpreter for assembly code that you can add instructions to by modifying instructions.cpp/h. Note, this project is incredibly messy, and I haven't really gone through and cleaned it up. So expect a lot of messy/unused/commented out code.

# Making a special interpreter
If you want to make a modified version of this interpreter, but instead use it as your own, then feel free to fork this repository. Just make sure to keep the same license(since GPL v3.0 requires that as far as I know).

# Structs
I used many different structs to allow for data to be passed around function calls easily, and for extensions to be much easier. So, here I will outline some of the structs that were used, as well as their purpose.
## Env
This is the struct that represents the program environment. The struct definition(omitting any redundant declarations that haven't been used yet) is as follows,
```C++
struct Env {
  int reg;
  int line;
  int memSize;
  std::vector<int> memory;
  Program program;
  int steps { 0 };
  std::vector<bool> states;
  bool endProgram{false};
};
```
`reg` is analagous to the use of "ACC" in single register CPUs. It is used for temporary storage for until it is put into output or written to memory. `line` is the instruction pointer / line number. `memSize` was originally used to do boundary checks(since I was going to use a dynamic array to store the memory, but switched to `std::vector` when I learned that it's pretty much an array), but now it's used for EnvConf to put the configuration into. Not sure why that is, but like I said before, I really need to clean up a lot of the code here to remove redundancies. `program` is to store the struct Program. `steps` is to record how long each program takes to execute, which is how many instructions were run before the program ended. `states` is a special one. It is used to keep track of any extra boolean states you want. Currently, only IS_END and NULL_REGISTER(states that the current register should have NULL in it, so it should cause an error if you try to write NULL to the memory, add NULL to anything, or really do anything with the register except write a value to it) are used, but you can add more if you want.

## EnvConfig
This is a struct for collecting values in an environment configuration header. `reg`, `line`, and `memSize` are the values that their respective Env parts are initialized to(ie. Env.reg is initialized to EnvConf.reg, etc.). `initialMemory` is exactly what you'd expect.

## Arg
This struct represents an argument that is given to an operation. A vector of the ones given in the text is passed to the operation's function when the instruction is being run. Currently, only two values are needed, `value`, and `derefLevel`. `value` can be whatever you want, but is usually a constant number(if `derefLevel == 0`), a memory address(if `derefLevel >= 1`), or a line number(if operation is a jmp-like, which means it's either `jmp`, `jiz`, or `jlz`). `derefLevel` is how many times the value is dereferenced before returning. 

## Line
This struct represents one line of the `.asm` file. `Op` is the enum class of the operation that this line is doing. `func` is a function pointer to the operation's function. `lineNum` is the line number that the line is on. `numArgs` specifies how many arguments the line's operation should get. And lastly `arguments` is the `vector<Arg>` of the processed arguments that is was given. 

# Files
Here I will try to briefly explain each file and it's purpose.
## instructions.{cpp,h}
Modify these files if you want to add new operations to the interpreter or modify pre-existing ones. Each function should have the same arguments with no return value. This is because the code calls the functions put in instructions.cpp(or really, instructions.h, since the function pointers go there) with the assumption that the first argument is an Env& type, and the second argument is a vector<Arg> type. When making the functions, keep in mind that a reference to the environent is passed into the function, not a copy of it, so you can modify the env to create side-affects. Also note that you have to increment the instruction pointer(which is Env.line) manually at the end of functions that should do so. This is done to allow for jmp instructions to change Env.line to the label line number. This is also to allow for instructions that change Env.line in whatever way you want. So, you could have a "jmp X" instruction, which moves the instruction pointer ahead X lines. That way, you can make label-less assembly code or you could make programs which simulate function calls(jumping to a certain place in the memory, running the code there until it sees a "push", then jumping back to the place where it was before) without needing to worry about the instruction pointer being off by one because the VM automatically incremented Env.line. Also, the "{cpp,h}" part means "instructions.cpp instructions.h", it's bash syntax.

## calltree.dot
This is a [GraphViz](https://graphviz.org) `.dot` file of the call tree to help with understanding what functions call what other functions. I made this to help with understanding what was calling what to help debug this monstrosity, so I thought I might as well put it here.

## instructionsEnum.h
This is just a header file for the enum class `Op`.

## mainLib.{cpp,h}
maiLib.cpp does all the interpreting(except for any string operations, which are in `stringops.cpp`). 

# TODO
1. Add the capability to do basic I/O using `cout` and `cin`.
2. Clean up this mess. Seriously, this code is very ugly and messy. If you can help with this, please feel free to try and make it better.
3. Add an option to compile the Program struct to C++ code. This would mean adding an `#include` to the start of the new file, putting any other boilerplate needed for a standard C program, then adding whatever function calls are needed to the file at each line, and lastly adding `goto` statements and labels for jmp instructions. Above all, the code should be compilable using a standard C++ compiler like `gpp`. This should be pretty simple, as long as I create a new map from an Op to string that gives the function name as a string given its Op enum class. Or, if I'm feeling extra lazy, I could just define a Program in the new file that hardcodes the Program struct that was just interpreted into the new file, and then add on any boilerplate code that is needed to get it to run the Program struct.




