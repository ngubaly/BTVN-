#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Define memory and register sizes
#define ROM_SIZE 128
#define RAM_SIZE 128
#define NUM_REGISTERS 4

// Registers
uint8_t PC = 0; // Program Counter
uint8_t registers[NUM_REGISTERS] = {0}; // General-purpose registers r0 to r3
uint8_t status = 0; // Status register: bit 0 (Zero), bit 1 (OF)

// Memory
uint8_t ROM[ROM_SIZE]; // Program ROM
uint8_t RAM[RAM_SIZE]; // Data RAM

// Status flag masks
#define STATUS_ZERO 0x01
#define STATUS_OF 0x02

// Helper macros
#define SET_ZERO_FLAG() (status |= STATUS_ZERO)
#define CLEAR_ZERO_FLAG() (status &= ~STATUS_ZERO)
#define SET_OF_FLAG() (status |= STATUS_OF)
#define CLEAR_OF_FLAG() (status &= ~STATUS_OF)

// Function prototypes
void fetch(uint8_t *instruction);
void decode_and_execute(uint8_t instruction);
void execute_data_transfer(uint8_t opcode);
void execute_arithmetic(uint8_t opcode);
void execute_shift(uint8_t opcode);
void execute_compare(uint8_t opcode);
void execute_branch(uint8_t opcode);
void execute_io(uint8_t opcode);

// Fetch instruction from ROM
void fetch(uint8_t *instruction) {
    if (PC < ROM_SIZE) {
        *instruction = ROM[PC++];
    } else {
        printf("PC out of bounds!\n");
    }
}

// Decode and execute the instruction
void decode_and_execute(uint8_t instruction) {
    uint8_t opcode = instruction >> 5; // Top 3 bits
    switch (opcode) {
        case 0b000: // Data Transfer
            execute_data_transfer(instruction);
            break;
        case 0b001: // Arithmetic
            execute_arithmetic(instruction);
            break;
        case 0b010: // Shift
            execute_shift(instruction);
            break;
        case 0b011: // Compare
            execute_compare(instruction);
            break;
        case 0b100: // I/O
            execute_io(instruction);
            break;
        case 0b101: // Conditional Branch
        case 0b110: // Unconditional Branch
            execute_branch(instruction);
            break;
        default:
            printf("Unknown opcode: %02X\n", opcode);
            break;
    }
}

// Execute data transfer instructions
void execute_data_transfer(uint8_t opcode) {
    uint8_t type = (opcode >> 3) & 0x03;
    uint8_t x = opcode & 0x07;
    switch (type) {
        case 0b00: // Memory to r0
            registers[0] = RAM[x];
            break;
        case 0b01: // r0 to Memory
            RAM[x] = registers[0];
            break;
        case 0b10: // Register to Register
            registers[x] = registers[(opcode >> 1) & 0x03];
            break;
        case 0b11: // Immediate value to r0
            registers[0] = x;
            break;
    }
    if (registers[0] == 0) SET_ZERO_FLAG();
    else CLEAR_ZERO_FLAG();
}

// Execute arithmetic instructions
void execute_arithmetic(uint8_t opcode) {
    uint8_t x = (opcode >> 3) & 0x03;
    uint8_t y = opcode & 0x03;
    if (opcode & 0x04) { // Subtraction
        registers[x] -= registers[y];
    } else { // Addition
        registers[x] += registers[y];
    }
    if (registers[x] == 0) SET_ZERO_FLAG();
    else CLEAR_ZERO_FLAG();
}

// Execute shift instructions
void execute_shift(uint8_t opcode) {
    uint8_t x = (opcode >> 3) & 0x03;
    if (opcode & 0x04) { // Right Shift
        registers[x] >>= 4;
    } else { // Left Shift
        registers[x] <<= 4;
    }
}

// Execute compare instructions
void execute_compare(uint8_t opcode) {
    uint8_t x = (opcode >> 3) & 0x03;
    uint8_t y = opcode & 0x03;
    if (registers[x] == registers[y]) SET_ZERO_FLAG();
    else CLEAR_ZERO_FLAG();
}

// Execute branch instructions
void execute_branch(uint8_t opcode) {
    uint8_t x = opcode & 0x07;
    if (((opcode >> 3) & 0x01) == 0) { // Conditional Branch
        if (status & STATUS_ZERO) PC = registers[x];
    } else { // Unconditional Branch
        PC = registers[x];
    }
}

// Execute I/O instructions
void execute_io(uint8_t opcode) {
    uint8_t port = opcode & 0x07;
    if ((opcode >> 3) & 0x01) { // Input
        printf("Input from port %d\n", port);
    } else { // Output
        printf("Output to port %d: %02X\n", port, registers[port]);
    }
}

// Main loop
int main() {
    // Load ROM with example program (to be written)
    ROM[0] = 0x10; // Example instruction
    // ...

    while (PC < ROM_SIZE) {
        uint8_t instruction;
        fetch(&instruction);
        decode_and_execute(instruction);
    }
    return 0;
}
