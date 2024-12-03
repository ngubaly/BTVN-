#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hardware/uart.h"
#include "pico/stdlib.h"

// Define constants
#define ROM_SIZE 128
#define RAM_SIZE 128
#define NUM_REGISTERS 4
#define FACTORIAL_RESULT_ADDRESS 0x80 // RAM address for storing factorial result

// UART configuration
#define UART_PORT uart0
#define UART_BAUD_RATE 115200

// Registers
uint8_t PC = 0; // Program Counter
uint8_t registers[NUM_REGISTERS] = {0}; // General-purpose registers r0 to r3
uint8_t status = 0; // Status register: Zero flag and Overflow flag

// Memory
uint8_t ROM[ROM_SIZE]; // Program ROM
uint8_t RAM[RAM_SIZE]; // Data RAM

// Status flag masks
#define STATUS_ZERO 0x01
#define STATUS_OF 0x02

// Helper macros for status flags
#define SET_ZERO_FLAG() (status |= STATUS_ZERO)
#define CLEAR_ZERO_FLAG() (status &= ~STATUS_ZERO)
#define SET_OF_FLAG() (status |= STATUS_OF)
#define CLEAR_OF_FLAG() (status &= ~STATUS_OF)

// Function prototypes
void uart_init_custom();
void fetch(uint8_t *instruction);
void decode_and_execute(uint8_t instruction);
void execute_data_transfer(uint8_t opcode);
void execute_arithmetic(uint8_t opcode);
void execute_shift(uint8_t opcode);
void execute_compare(uint8_t opcode);
void execute_branch(uint8_t opcode);
void execute_io(uint8_t opcode);
void uart_write(uint8_t data);
uint8_t uart_read();
void factorial();

// UART initialization function
void uart_init_custom() {
    uart_init(UART_PORT, UART_BAUD_RATE);
    gpio_set_function(0, GPIO_FUNC_UART); // UART TX pin
    gpio_set_function(1, GPIO_FUNC_UART); // UART RX pin
}

// Function to write data to UART
void uart_write(uint8_t data) {
    uart_putc(UART_PORT, data);
}

// Function to read data from UART
uint8_t uart_read() {
    while (!uart_is_readable(UART_PORT)) {
        tight_loop_contents();
    }
    return uart_getc(UART_PORT);
}

// Fetch instruction from ROM
void fetch(uint8_t *instruction) {
    if (PC < ROM_SIZE) {
        *instruction = ROM[PC++];
    } else {
        uart_write('E'); // Indicating error in fetching instruction
    }
}

// Decode and execute instruction
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
            uart_write('E'); // Invalid opcode
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

// Execute arithmetic instructions (e.g., addition, subtraction)
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

// Execute comparison instructions
void execute_compare(uint8_t opcode) {
    uint8_t x = (opcode >> 3) & 0x03;
    uint8_t y = opcode & 0x03;
    if (registers[x] == registers[y]) SET_ZERO_FLAG();
    else CLEAR_ZERO_FLAG();
}

// Execute branch instructions (conditional or unconditional)
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
        uart_write('I'); // Placeholder for input action
    } else { // Output
        uart_write(registers[port]);
    }
}

// Function to compute factorial (5!)
void factorial() {
    registers[0] = 1;  // r0 = 1
    registers[1] = 5;  // r1 = 5
    while (registers[1] > 1) {
        registers[0] *= registers[1];  // r0 = r0 * r1
        registers[1]--;  // r1 = r1 - 1
    }
    RAM[FACTORIAL_RESULT_ADDRESS] = registers[0]; // Store result in RAM
    uart_write(registers[0]);  // Output result via UART
}

// Main loop
int main() {
    // Initialize UART communication
    uart_init_custom();

    // Wait for input assembly instructions via UART (e.g., "LD R0, #5")
    while (1) {
        uint8_t instruction = uart_read();
        if (instruction == 'R') { // Simple condition for ready-to-execute command
            factorial();  // Calculate factorial of 5
            break;
        } else {
            ROM[PC++] = instruction;  // Store instruction in ROM
        }
    }

    // Main CPU cycle: Fetch, decode, and execute instructions
    while (PC < ROM_SIZE) {
        uint8_t instruction;
        fetch(&instruction);
        decode_and_execute(instruction);
    }

    // Indicate the end of execution
    uart_write('D');
    return 0;
}
