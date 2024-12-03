#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define ROM_SIZE 128  // 128 bytes
#define RAM_SIZE 128  // 128 bytes

// 8-bit Registers and Memory
uint8_t ROM[ROM_SIZE];  // Program ROM (128 bytes)
uint8_t RAM[RAM_SIZE];  // Data RAM (128 bytes)

// Registers: Program Counter (PC) and General-purpose registers (r0 to r3)
uint8_t PC = 0;         // Program Counter starts from 0
uint8_t r[4] = {0, 0, 0, 0};  // General-purpose registers (r0, r1, r2, r3)
uint8_t STATUS = 0;  // Status Register (Zero flag = bit 0, Overflow flag = bit 1)

// Define instruction set opcodes
#define LOAD 0x01     // Load immediate value into r0
#define ADD  0x02     // Add r0 + r1, store result in r0
#define SUB  0x03     // Subtract r1 from r0, store result in r0
#define MOV  0x04     // Move r0 to r1
#define BZ   0x05     // Branch if Zero (if Zero flag is set)
#define BEQ  0x06     // Branch if Equal (r0 == r1)
#define SHL  0x07     // Shift left r0 by 4 bits
#define SHR  0x08     // Shift right r0 by 4 bits

// Function prototypes
void execute_instruction(uint8_t instruction);
void fetch_and_execute();
void set_flags(uint8_t result);
void load_program();

// Main entry point
int main() {
    // Load a simple program into ROM for demonstration
    load_program();

    // Execute instructions in a loop
    while (PC < ROM_SIZE) {
        fetch_and_execute();  // Fetch and execute the next instruction
    }

    return 0;
}

// Load a simple program (hardcoded) into ROM
void load_program() {
    // Sample program:
    // 1. LOAD immediate value 5 into r0
    // 2. ADD r0 + r1 -> r0
    // 3. MOV r0 -> r1
    // 4. SHL r0 by 4 bits
    // 5. BZ branch if Zero flag is set (but here it won't be set)
    
    ROM[0] = LOAD;  // Load 5 into r0
    ROM[1] = 5;     // Immediate value to load into r0
    ROM[2] = ADD;   // Add r0 + r1 -> r0
    ROM[3] = MOV;   // Move r0 to r1
    ROM[4] = SHL;   // Shift r0 left by 4 bits
    ROM[5] = BZ;    // Branch if Zero flag is set (not used)
}

// Fetch and Execute the instruction at the current PC
void fetch_and_execute() {
    uint8_t instruction = ROM[PC];  // Fetch the instruction at PC
    uint8_t operand = ROM[PC + 1];  // Operand for the instruction (if any)
    PC += 2;  // Increment PC by 2 (1 byte for opcode, 1 byte for operand)

    execute_instruction(instruction, operand);  // Execute the instruction
}

// Execute the fetched instruction
void execute_instruction(uint8_t instruction, uint8_t operand) {
    switch (instruction) {
        case LOAD:  // Load immediate value into r0
            r[0] = operand;
            printf("LOAD: r0 = %d\n", r[0]);
            break;
        
        case ADD:  // Add r0 + r1, store result in r0
            r[0] = r[0] + r[1];
            set_flags(r[0]);
            printf("ADD: r0 = %d\n", r[0]);
            break;

        case SUB:  // Subtract r1 from r0, store result in r0
            r[0] = r[0] - r[1];
            set_flags(r[0]);
            printf("SUB: r0 = %d\n", r[0]);
            break;

        case MOV:  // Move r0 to r1
            r[1] = r[0];
            printf("MOV: r1 = %d\n", r[1]);
            break;

        case SHL:  // Shift left r0 by 4 bits
            r[0] = r[0] << 4;
            set_flags(r[0]);
            printf("SHL: r0 = %d\n", r[0]);
            break;

        case SHR:  // Shift right r0 by 4 bits
            r[0] = r[0] >> 4;
            set_flags(r[0]);
            printf("SHR: r0 = %d\n", r[0]);
            break;

        case BZ:   // Branch if Zero flag is set (skip next instruction if Z flag is set)
            if (STATUS & 0x01) {
                PC = operand;  // Jump to the address in operand
                printf("BZ: Branching to address %d\n", PC);
            }
            break;

        default:
            printf("Unknown instruction: %x\n", instruction);
            break;
    }
}

// Set the status flags based on the result of an operation
void set_flags(uint8_t result) {
    // Zero flag: Set if result is zero
    if (result == 0) {
        STATUS |= 0x01;  // Set Zero flag (bit 0)
    } else {
        STATUS &= ~0x01; // Clear Zero flag (bit 0)
    }

    // Overflow flag: Simple overflow detection for 8-bit operations
    if (result > 255) {
        STATUS |= 0x02;  // Set Overflow flag (bit 1)
    } else {
        STATUS &= ~0x02; // Clear Overflow flag (bit 1)
    }
}
