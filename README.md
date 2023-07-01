CPU_Simulator

This program is designed to execute pseudo assembly code provided as input. It supports various instructions and provides a simple loop example. However, please note that the za flag register should be used with caution.
Registers

    AYB
    BEN
    GIM
    DA
    ECH
    ZA

Instructions

The program accepts pseudo assembly code as input and executes it accordingly. The assembly code should follow the following guidelines:

    The program is case-insensitive.
    To create a label, write LABELNAME: on a separate line. Do not include any other content on the same line as the label and do not add space between the label and : like LABELNAME : (it is incorrect).
    Use instructions according to the provided instruction set (documented below).

Instruction Set

The following instructions are supported by the program:

    MOV: Move a value into a register or memory location. Usage: MOV <destination>, <source>. Example: MOV AYB [10] (move the value at memory address 10 into register AYB).
    ADD: Add a value to a register or memory location. Usage: ADD <destination>, <source>. Example: ADD BEN GIM (add the value in register GIM to register BEN).
    SUB: Subtract a value from a register or memory location. Usage: SUB <destination>, <source>. Example: SUB AYB 5 (subtract 5 from register AYB).
    CMP: Compare two values and set the za flag accordingly. Usage: CMP <operand1>, <operand2>. Example: CMP [5], 0 (compare the value at memory address 5 with 0).
    AND: Perform bitwise AND operation between two values and store the result in a register or memory location. Usage: AND <destination>, <source>. Example: AND AYB GIM (perform bitwise AND between the values in registers AYB and GIM).
    OR: Perform bitwise OR operation between two values and store the result in a register or memory location. Usage: OR <destination>, <source>. Example: OR AYB GIM (perform bitwise OR between the values in registers AYB and GIM).
    NOT: Perform bitwise NOT operation on a value and store the result in a register or memory location. Usage: NOT <destination>. Example: NOT AYB (perform bitwise NOT on the value in register AYB).
    JE: Jump to a specified label if equal (i.e., if the zero condition is met). Usage: JE <label>. Example: JE END (jump to the label END if the za flag is 0).
    JG: Jump to a specified label if greater (i.e., if the zero condition is met). Usage: JG <label>. Example: JG END (jump to the label END if the za flag is 1).
    JL: Jump to a specified label if less (i.e., if the zero condition is met). Usage: JL <label>. Example: JL END (jump to the label END if the za flag is -1).
    JMP: Unconditional jump to a specified label. Usage: JMP <label>. Example: JMP LOOP (jump to the label LOOP).
    dump_memory(): Show the current memory image.

Example

Here's an example of a simple loop implemented in assembly code:


mov ayb 0

mov ben 5

loop:

cmp ben 0

je end

add ayb 1

sub ben 1

jmp loop

end:

mov [0] ayb

dump_memory()


In this example, the program initializes register AYB with the value 0 and register BEN with the value 5. Then it enters a loop where it compares the value in register BEN with 0. If they are equal (za flag is 0), it jumps to the END label. Otherwise, it adds 1 to register AYB and jumps back to the LOOP label to repeat the loop. Finally, it stores the value in register AYB into memory address 0 and displays the current memory image.
Usage

To execute the assembly program, follow these steps:

    Write your assembly code following the guidelines mentioned above.
    Save the assembly code in a file.(extension is optional)
    Compile and run the C++ program using a C++ compiler.

For example, if you have saved your assembly code in a file called program.asm and your C++ program file is program.cpp, you can compile and run the program with the following command:

shell

g++ program.cpp -o program
./program program.asm

Replace program.cpp with the name of your C++ program file.

That's it! The program will execute the provided assembly code and display the results accordingly.

Remember to use the za flag register carefully, as it can affect the program's behavior.
