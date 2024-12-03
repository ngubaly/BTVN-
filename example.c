#include <stdio.h>
#include "pico/stdlib.h"

// ROM and RAM sizes
#define ROM_SIZE 128  // 128 bytes
#define RAM_SIZE 128  // 128 bytes

// Registers
uint8_t PC = 0;         // Program Counter
uint8_t r[4] = {0, 0, 0, 0};  // General-purpose registers (r0 to r3)
uint8_t STATUS = 0;     // Status Register (Zero flag = bit 0, Overflow flag = bit 1)

// Memory
uint8_t ROM[ROM_SIZE];  // Program ROM
uint8_t RAM[RAM_SIZE];  // Data RAM

// Instruction set opcodes
#define LOAD_IMM  0x01  // Load immediate value into r0
#define ADD       0x02  // Add r0 + r1 -> r2
#define SUB       0x03  // Subtract r1 from r0 -> r2
#define MOV       0x04  // Move r0 -> r1
#define SHL       0x05  // Shift left r0 by 4 bits
#define SHR       0x06  // Shift right r0 by 4 bits
#define CMP       0x07  // Compare r0 and r1, set Zero flag
#define BRANCH_Z  0x08  // Branch if Zero flag is set

// Function prototypes
void fetch_and_execute();
void execute(uint8_t instruction, uint8_t operand);
void set_flags(uint8_t result);
void load_program();
void debug_registers();

int main() {
    stdio_init_all();  // Initialize UART for debugging

    printf("Initializing 8-bit CPU on RP2040...\n");

    // Load program into ROM
    load_program();

    // Main execution loop
    while (PC < ROM_SIZE) {
        fetch_and_execute();  // Fetch and execute instructions
    }

    return 0;
}

// Load a program into ROM
void load_program() {
    // Example program:
    // Load immediate values into r0 and r1, add them, and check for Zero flag
    ROM[0] = LOAD_IMM;  // Load immediate value into r0
    ROM[1] = 5;         // Immediate value: 5
    ROM[2] = LOAD_IMM;  // Load immediate value into r1
    ROM[3] = 10;        // Immediate value: 10
    ROM[4] = ADD;       // Add r0 + r1 -> r2
    ROM[5] = CMP;       // Compare r2 and r0, set Zero flag
    ROM[6] = BRANCH_Z;  // Branch if Zero flag is set
    ROM[7] = 2;         // Branch to instruction 2 (loop for demo)
}

// Fetch and execute an instruction
void fetch_and_execute() {
    if (PC >= ROM_SIZE) return;  // Stop if PC exceeds ROM size

    uint8_t instruction = ROM[PC];  // Fetch instruction
    uint8_t operand = ROM[PC + 1];  // Fetch operand (if any)
    PC += 2;  // Increment PC by 2 (1 for opcode, 1 for operand)

    printf("PC: %d, Instruction: 0x%02X, Operand: %d\n", PC, instruction, operand);

    execute(instruction, operand);  // Decode and execute the instruction
}

// Execute an instruction
void execute(uint8_t instruction, uint8_t operand) {
    switch (instruction) {
        case LOAD_IMM:  // Load immediate value into r0
            r[0] = operand;
            printf("LOAD_IMM: r0 = %d\n", r[0]);
            break;

        case ADD:  // Add r0 + r1 -> r2
            r[2] = r[0] + r[1];
            set_flags(r[2]);
            printf("ADD: r0 + r1 = %d + %d = %d\n", r[0], r[1], r[2]);
            break;

        case SUB:  // Subtract r1 from r0 -> r2
            r[2] = r[0] - r[1];
            set_flags(r[2]);
            printf("SUB: r0 - r1 = %d - %d = %d\n", r[0], r[1], r[2]);
            break;

        case MOV:  // Move r0 -> r1
            r[1] = r[0];
            printf("MOV: r0 = %d -> r1 = %d\n", r[0], r[1]);
            break;

        case SHL:  // Shift left r0 by 4 bits
            r[0] <<= 4;
            set_flags(r[0]);
            printf("SHL: r0 << 4 = %d\n", r[0]);
            break;

        case SHR:  // Shift right r0 by 4 bits
            r[0] >>= 4;
            set_flags(r[0]);
            printf("SHR: r0 >> 4 = %d\n", r[0]);
            break;

        case CMP:  // Compare r0 and r1, set Zero flag
            STATUS = (r[0] == r[1]) ? (STATUS | 0x01) : (STATUS & ~0x01);
            printf("CMP: r0 = %d, r1 = %d, Zero flag = %d\n", r[0], r[1], STATUS & 0x01);
            break;

        case BRANCH_Z:  // Branch if Zero flag is set
            if (STATUS & 0x01) {
                PC = operand;
                printf("BRANCH_Z: Jumping to PC = %d\n", PC);
            }
            break;

        default:
            printf("Unknown instruction: 0x%02X\n", instruction);
            break;
    }

    debug_registers();  // Print the state of registers and flags
}

// Set status flags (Zero, Overflow)
void set_flags(uint8_t result) {
    // Set Zero flag
    STATUS = (result == 0) ? (STATUS | 0x01) : (STATUS & ~0x01);

    // Overflow detection (if result > 255 or < 0 in 8-bit arithmetic)
    STATUS = (result > 255) ? (STATUS | 0x02) : (STATUS & ~0x02);
}

// Debugging: Print register states
void debug_registers() {
    printf("Registers: r0 = %d, r1 = %d, r2 = %d, r3 = %d\n", r[0], r[1], r[2], r[3]);
    printf("STATUS: Zero = %d, Overflow = %d\n", STATUS & 0x01, (STATUS & 0x02) >> 1);
}
