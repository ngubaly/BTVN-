
皆さん、改めましてこんにちは。
ベトナムから参りました Nguyen Ba Linh と申します。

以前、1か月間、こちらでインターンシップを経験させていただき、大変お世話になりました。その経験を通して、チームの皆さんの働き方や日本の職場文化に触れることができ、とても勉強になりました。

今回、正式にメンバーとして再び参加できることを嬉しく思っています。これからもより多くのことを学びつつ、少しでもチームに貢献できるよう頑張りたいと思います。

どうぞよろしくお願いいたします。

Minasan, aratamemashite konnichiwa.
Betonamu kara mairimashita Nguyen Ba Linh to moushimasu.

Izen, ikkagetsukan, kochira de intaanshippu o keikensase te itadaki, taihen osewa ni narimashita. Sono keiken o tooshite, chiimu no minasan no hatarakikata ya Nihon no shokuba bunka ni fureru koto ga deki, totemo benkyou ni narimashita.

Konkai, seishiki ni menbaa toshite futatabi sanka dekiru koto o ureshiku omoimasu. Korekara mo yori ooku no koto o manabi tsutsu, sukoshi demo chiimu ni kouken dekiru you ganbaritai to omoimasu.

Douzo yoroshiku onegaishimasu.
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// CPU structure definition
typedef struct {
    uint8_t PC;          // Program Counter
    uint8_t R[4];        // General-purpose registers R0 to R3
    struct {
        bool ZF;         // Zero Flag
        bool OF;         // Overflow Flag
    } SR;                // Status Register
} CPU;

// Memory definitions
#define ROM_SIZE 128
#define RAM_SIZE 128

uint8_t ROM[ROM_SIZE];   // Program ROM from address 0x00 to 0x7F
uint8_t RAM[RAM_SIZE];   // Data RAM from address 0x80 to 0xFF

// Function prototypes
void fetch(uint8_t *instruction);
void decode_execute(uint8_t instruction);
void load_program(const uint8_t *program, size_t size);

// Global CPU instance
CPU cpu;

// Function implementations

/**
 * @brief Fetches the next instruction from the ROM.
 * 
 * This function reads the instruction at the current Program Counter (PC)
 * from the ROM and increments the PC. If PC reaches the end of the ROM,
 * it wraps around to 0.
 * 
 * @param instruction Pointer to store the fetched instruction.
 */
void fetch(uint8_t *instruction) {
    *instruction = ROM[cpu.PC];
    cpu.PC = (cpu.PC + 1) % ROM_SIZE;
}

/**
 * @brief Decodes and executes the fetched instruction.
 * 
 * This function decodes the opcode and operands from the instruction,
 * and performs the corresponding operation. It handles data transfer,
 * arithmetic, bit shift, comparison, I/O, and branch instructions.
 * 
 * @param instruction The instruction byte fetched from ROM.
 */
void decode_execute(uint8_t instruction) {
    uint8_t opcode = instruction >> 4;     // Upper 4 bits
    uint8_t operand = instruction & 0x0F;  // Lower 4 bits

    switch (opcode) {
        case 0x0:  // NOP
            // No operation
            break;
        case 0x1:  // LOAD_IMM
            {
                uint8_t reg = (operand >> 2) & 0x03;  // Bits 3-2
                uint8_t imm;
                fetch(&imm);
                cpu.R[reg] = imm;
            }
            break;
        case 0x2:  // LOAD_MEM
            {
                uint8_t addr;
                fetch(&addr);
                cpu.R[0] = (addr < 0x80) ? ROM[addr] : RAM[addr - 0x80];
            }
            break;
        case 0x3:  // STORE_MEM
            {
                uint8_t addr;
                fetch(&addr);
                if (addr >= 0x80) {
                    RAM[addr - 0x80] = cpu.R[0];
                } else {
                    // Cannot write to ROM; handle error or ignore
                }
            }
            break;
        case 0x4:  // MOV_REG
            {
                uint8_t dest = (operand >> 2) & 0x03;
                uint8_t src = operand & 0x03;
                cpu.R[dest] = cpu.R[src];
            }
            break;
        case 0x5:  // ADD
            {
                uint8_t dest = (operand >> 2) & 0x03;
                uint8_t src = operand & 0x03;
                uint16_t result = cpu.R[dest] + cpu.R[src];
                cpu.R[dest] = (uint8_t)result;
                cpu.SR.ZF = (cpu.R[dest] == 0);
                cpu.SR.OF = (result > 0xFF);
            }
            break;
        case 0x6:  // SUB
            {
                uint8_t dest = (operand >> 2) & 0x03;
                uint8_t src = operand & 0x03;
                uint16_t result = cpu.R[dest] - cpu.R[src];
                cpu.R[dest] = (uint8_t)result;
                cpu.SR.ZF = (cpu.R[dest] == 0);
                cpu.SR.OF = (result > 0xFF);
            }
            break;
        case 0x7:  // SHIFT
            {
                uint8_t direction = (operand >> 3) & 0x01;  // Bit 3
                uint8_t reg = (operand >> 1) & 0x03;        // Bits 2-1
                if (direction == 0) {
                    // Left shift by 4 bits
                    cpu.R[reg] <<= 4;
                } else {
                    // Right shift by 4 bits
                    cpu.R[reg] >>= 4;
                }
                cpu.SR.ZF = (cpu.R[reg] == 0);
            }
            break;
        case 0x8:  // CMP
            {
                uint8_t dest = (operand >> 2) & 0x03;
                uint8_t src = operand & 0x03;
                cpu.SR.ZF = (cpu.R[dest] == cpu.R[src]);
            }
            break;
        case 0x9:  // INPUT/OUTPUT
            {
                uint8_t direction = (operand >> 3) & 0x01;  // Bit 3
                uint8_t reg = (operand >> 1) & 0x03;        // Bits 2-1
                if (direction == 0) {
                    // Input to register
                    printf("Enter value for R%d: ", reg);
                    scanf("%hhu", &cpu.R[reg]);
                } else {
                    // Output from register
                    printf("Output from R%d: %u\n", reg, cpu.R[reg]);
                }
            }
            break;
        case 0xA:  // JMP
            {
                uint8_t reg = operand & 0x03;
                cpu.PC = cpu.R[reg];
            }
            break;
        case 0xB:  // JZ
            {
                uint8_t reg = operand & 0x03;
                if (cpu.SR.ZF) {
                    cpu.PC = cpu.R[reg];
                }
            }
            break;
        default:
            // Invalid opcode; handle error
            break;
    }
}

/**
 * @brief Loads a program into the ROM.
 * 
 * This function copies the program code into the ROM array, up to the maximum
 * ROM size. It ensures that the program does not exceed the ROM capacity.
 * 
 * @param program Pointer to the array containing the program code.
 * @param size Size of the program code in bytes.
 */
void load_program(const uint8_t *program, size_t size) {
    if (size > ROM_SIZE) {
        size = ROM_SIZE;  // Truncate program if it exceeds ROM size
    }
    memcpy(ROM, program, size);
}

/**
 * @brief Main function to initialize and run the CPU simulator.
 * 
 * This function initializes the CPU and memory, loads a sample program,
 * and starts the fetch-decode-execute loop. It runs until the program
 * counter reaches the end of the ROM or a halt condition is met.
 * 
 * @return int Exit code of the program.
 */
int main() {
    // Initialize CPU
    memset(&cpu, 0, sizeof(cpu));

    // Sample program (to be defined)
    uint8_t program[] = {
        // Example instructions
        0x14, 0x05,   // LOAD_IMM R1, 5
        0x18, 0x0A,   // LOAD_IMM R2, 10
        0x54,         // ADD R1, R2
        0x95,         // OUTPUT R1
        0x00          // NOP (end of program)
    };

    // Load program into ROM
    load_program(program, sizeof(program));

    // Main loop
    while (cpu.PC < ROM_SIZE) {
        uint8_t instruction;
        fetch(&instruction);
        if (instruction == 0x00) {
            // NOP or HALT
            break;
        }
        decode_execute(instruction);
    }

    return 0;
}






2:
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// CPU structure definition
typedef struct {
    uint8_t PC;          // Program Counter
    uint8_t R[4];        // General-purpose registers R0 to R3
    struct {
        bool ZF;         // Zero Flag
        bool OF;         // Overflow Flag
    } SR;                // Status Register
} CPU;

// Memory definitions
#define ROM_SIZE 128
#define RAM_SIZE 128

uint8_t ROM[ROM_SIZE];   // Program ROM from address 0x00 to 0x7F
uint8_t RAM[RAM_SIZE];   // Data RAM from address 0x80 to 0xFF

// Function prototypes
void fetch(uint8_t *instruction);
void decode_execute(uint8_t instruction);
void load_program(const uint8_t *program, size_t size);

// Global CPU instance
CPU cpu;

// Function implementations

/**
 * @brief Fetches the next instruction from the ROM.
 *
 * This function reads the instruction at the current Program Counter (PC)
 * from the ROM and increments the PC. If PC reaches the end of the ROM,
 * it wraps around to 0.
 *
 * @param instruction Pointer to store the fetched instruction.
 */
void fetch(uint8_t *instruction) {
    *instruction = ROM[cpu.PC];
    cpu.PC = (cpu.PC + 1) % ROM_SIZE;
}

/**
 * @brief Decodes and executes the fetched instruction.
 *
 * This function decodes the opcode, sub-opcode, and operands from the instruction,
 * and performs the corresponding operation as per the instruction set provided.
 * It handles data transfer, arithmetic, bit shift, comparison, I/O, and branch instructions.
 *
 * @param instruction The instruction byte fetched from ROM.
 */
void decode_execute(uint8_t instruction) {
    uint8_t opcode = (instruction >> 4) & 0x0F;     // Bits 7-4
    uint8_t dd = (instruction >> 2) & 0x03;         // Bits 3-2 (sub-opcode or don't care)
    uint8_t xx = instruction & 0x03;                // Bits 1-0
    uint8_t yy = instruction & 0x03;                // Bits 1-0 (when needed)
    uint8_t immediate = instruction & 0x0F;         // Bits 3-0 (for immediate value)

    switch (opcode) {
        case 0x0:  // 0000: LOAD/STORE Memory and R0
            if (dd == 0x00) {
                // LOAD: Transfer data from memory into R0
                uint8_t addr = xx;
                uint8_t data = (addr < 0x80) ? ROM[addr] : RAM[addr - 0x80];
                cpu.R[0] = data;
                cpu.SR.ZF = (cpu.R[0] == 0);
            } else if (dd == 0x01) {
                // STORE: Transfer data from R0 into memory
                uint8_t addr = xx;
                uint8_t data = cpu.R[0];
                if (addr >= 0x80) {
                    RAM[addr - 0x80] = data;
                } else {
                    // Cannot write to ROM; handle error or ignore
                }
                cpu.SR.ZF = (data == 0);
            }
            break;
        case 0x1:  // 0001: Transfer between Registers (RX → RY)
            {
                uint8_t rx = (instruction >> 2) & 0x03;  // Bits 3-2
                uint8_t ry = instruction & 0x03;         // Bits 1-0
                cpu.R[ry] = cpu.R[rx];
                cpu.SR.ZF = (cpu.R[ry] == 0);
            }
            break;
        case 0x2:  // 0010: Immediate Value Setting into R0
            {
                cpu.R[0] = immediate;
                cpu.SR.ZF = (cpu.R[0] == 0);
            }
            break;
        case 0x3:  // 0011: ADD RX + RY → RX
            {
                uint8_t rx = (instruction >> 2) & 0x03;  // Bits 3-2
                uint8_t ry = instruction & 0x03;         // Bits 1-0
                uint16_t result = cpu.R[rx] + cpu.R[ry];
                cpu.R[rx] = (uint8_t)result;
                cpu.SR.ZF = (cpu.R[rx] == 0);
                cpu.SR.OF = (result > 0xFF);
            }
            break;
        case 0x4:  // 0100: SUB RX - RY → RX
            {
                uint8_t rx = (instruction >> 2) & 0x03;  // Bits 3-2
                uint8_t ry = instruction & 0x03;         // Bits 1-0
                uint16_t result = cpu.R[rx] - cpu.R[ry];
                cpu.R[rx] = (uint8_t)result;
                cpu.SR.ZF = (cpu.R[rx] == 0);
                cpu.SR.OF = (result > 0xFF);
            }
            break;
        case 0x5:  // 0101: SHIFT Operations
            if (dd == 0x00) {
                // LEFT SHIFT RX << 4 → RX
                uint8_t rx = xx;
                cpu.R[rx] <<= 4;
                // STATUS not updated
            } else if (dd == 0x01) {
                // RIGHT SHIFT RX >> 4 → RX
                uint8_t rx = xx;
                cpu.R[rx] >>= 4;
                // STATUS not updated
            }
            break;
        case 0x6:  // 0110: CMP RX == 0 or OUTPUT RX
            if (dd == 0x00) {
                // COMPARE RX == 0
                uint8_t rx = xx;
                cpu.SR.ZF = (cpu.R[rx] == 0);
            } else if (dd == 0x01) {
                // OUTPUT RX
                uint8_t rx = xx;
                printf("Output from R%d: %u\n", rx, cpu.R[rx]);
            }
            break;
        case 0x7:  // 0111: INPUT into RX
            {
                uint8_t rx = xx;
                printf("Enter value for R%d: ", rx);
                scanf("%hhu", &cpu.R[rx]);
            }
            break;
        case 0xE:  // 1110: Conditional Branch if Zero Flag is set
            {
                uint8_t rx = xx;
                if (cpu.SR.ZF) {
                    cpu.PC = cpu.R[rx];
                }
            }
            break;
        case 0xF:  // 1111: Unconditional Branch
            {
                uint8_t rx = xx;
                cpu.PC = cpu.R[rx];
            }
            break;
        default:
            // Invalid opcode; handle error
            break;
    }
}

/**
 * @brief Loads a program into the ROM.
 *
 * This function copies the program code into the ROM array, up to the maximum
 * ROM size. It ensures that the program does not exceed the ROM capacity.
 *
 * @param program Pointer to the array containing the program code.
 * @param size Size of the program code in bytes.
 */
void load_program(const uint8_t *program, size_t size) {
    if (size > ROM_SIZE) {
        size = ROM_SIZE;  // Truncate program if it exceeds ROM size
    }
    memcpy(ROM, program, size);
}

/**
 * @brief Main function to initialize and run the CPU simulator.
 *
 * This function initializes the CPU and memory, loads a sample program,
 * and starts the fetch-decode-execute loop. It runs until it reaches a NOP instruction.
 *
 * @return int Exit code of the program.
 */
int main() {
    // Initialize CPU
    memset(&cpu, 0, sizeof(cpu));

    // Sample program demonstrating the instruction set
    uint8_t program[] = {
        // LOAD immediate value into R0
        0x20,         // 0010 0000: R0 <- Immediate value (next byte)
        0x0A,         // Immediate value: 10
        // STORE R0 into memory address 0x80 (RAM address)
        0x04,         // 0000 0000: STORE R0 -> Memory[xx] (dd = 01 for STORE)
        // Assuming dd bits set to 01 for STORE operation
        // But as per instruction set, STORE is 0000 01xx
        // So we need to encode dd = 01
        // Adjusting the encoding:
        0x04,         // 0000 0000: We need dd = 01, so instruction is 0000 01xx
        // Let's write the correct instruction for STORE
        0x08,         // 0000 10xx: STORE R0 -> Memory[xx] with dd=01, xx=00
        // So the instruction is 0000 0100, which is 0x04
        // For clarity, we will adjust the encoding in the program
        // Let's correct the program to match the instruction set

        // Corrected sample program:
        0x20,         // 0010 0000: Immediate value setting into R0
        0x0A,         // Immediate value: 10
        0x04,         // 0000 0100: STORE R0 into Memory[xx], dd=01, xx=00 (address 0x00)
        // ADD R0 + R0 -> R0
        0x30,         // 0011 0000: ADD R0 + R0 -> R0
        // OUTPUT R0
        0x65,         // 0110 0101: OUTPUT RX, dd=01, xx=01 (R1)
        // Note: We need to set R1 with the result
        // So let's adjust the program to move R0 to R1
        // MOVE R0 to R1
        0x11,         // 0001 0001: MOVE R0 -> R1
        // OUTPUT R1
        0x65,         // 0110 0101: OUTPUT R1, dd=01, xx=01
        // NOP (end of program)
        0x00          // Assuming 0x00 is NOP or HALT
    };

    // Load program into ROM
    load_program(program, sizeof(program));

    // Main loop
    while (1) {
        uint8_t instruction;
        fetch(&instruction);
        if (instruction == 0x00) {
            // NOP or HALT
            break;
        }
        decode_execute(instruction);
    }

    return 0;
}


3. check of:
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// CPU structure definition
typedef struct {
    uint8_t PC;          // Program Counter
    uint8_t R[4];        // General-purpose registers R0 to R3
    struct {
        bool ZF;         // Zero Flag
        bool OF;         // Overflow Flag
    } SR;                // Status Register
} CPU;

// Memory definitions
#define ROM_SIZE 128
#define RAM_SIZE 128

uint8_t ROM[ROM_SIZE];   // Program ROM from address 0x00 to 0x7F
uint8_t RAM[RAM_SIZE];   // Data RAM from address 0x80 to 0xFF

// Function prototypes
void fetch(uint8_t *instruction);
void decode_execute(uint8_t instruction);
void load_program(const uint8_t *program, size_t size);

// Global CPU instance
CPU cpu;

// Function implementations

/**
 * @brief Fetches the next instruction from the ROM.
 *
 * This function reads the instruction at the current Program Counter (PC)
 * from the ROM and increments the PC. If PC reaches the end of the ROM,
 * it wraps around to 0.
 *
 * @param instruction Pointer to store the fetched instruction.
 */
void fetch(uint8_t *instruction) {
    *instruction = ROM[cpu.PC];
    cpu.PC = (cpu.PC + 1) % ROM_SIZE;
}

/**
 * @brief Decodes and executes the fetched instruction.
 *
 * This function decodes the opcode, sub-opcode, and operands from the instruction,
 * and performs the corresponding operation as per the instruction set provided.
 * It handles data transfer, arithmetic, bit shift, comparison, I/O, and branch instructions.
 *
 * @param instruction The instruction byte fetched from ROM.
 */
void decode_execute(uint8_t instruction) {
    uint8_t opcode = (instruction >> 4) & 0x0F;     // Bits 7-4
    uint8_t dd = (instruction >> 2) & 0x03;         // Bits 3-2 (sub-opcode or don't care)
    uint8_t xx = instruction & 0x03;                // Bits 1-0
    uint8_t yy = instruction & 0x03;                // Bits 1-0 (when needed)
    uint8_t immediate = instruction & 0x0F;         // Bits 3-0 (for immediate value)

    switch (opcode) {
        case 0x0:  // 0000: LOAD/STORE Memory and R0
            if (dd == 0x00) {
                // LOAD: Transfer data from memory into R0
                uint8_t addr = xx;
                uint8_t data = (addr < 0x80) ? ROM[addr] : RAM[addr - 0x80];
                cpu.R[0] = data;
                cpu.SR.ZF = (cpu.R[0] == 0);
            } else if (dd == 0x01) {
                // STORE: Transfer data from R0 into memory
                uint8_t addr = xx;
                uint8_t data = cpu.R[0];
                if (addr >= 0x80) {
                    RAM[addr - 0x80] = data;
                } else {
                    // Cannot write to ROM; handle error or ignore
                }
                cpu.SR.ZF = (data == 0);
            }
            break;
        case 0x1:  // 0001: Transfer between Registers (RX → RY)
            {
                uint8_t rx = (instruction >> 2) & 0x03;  // Bits 3-2
                uint8_t ry = instruction & 0x03;         // Bits 1-0
                cpu.R[ry] = cpu.R[rx];
                cpu.SR.ZF = (cpu.R[ry] == 0);
            }
            break;
        case 0x2:  // 0010: Immediate Value Setting into R0
            {
                cpu.R[0] = immediate;
                cpu.SR.ZF = (cpu.R[0] == 0);
            }
            break;
        case 0x3:  // 0011: ADD RX + RY → RX
            {
                uint8_t rx = (instruction >> 2) & 0x03;  // Bits 3-2
                uint8_t ry = instruction & 0x03;         // Bits 1-0
                uint16_t result = cpu.R[rx] + cpu.R[ry];
                cpu.SR.OF = (result > 0xFF);             // Overflow if result exceeds 255
                cpu.R[rx] = (uint8_t)result;             // Store the lower 8 bits
                cpu.SR.ZF = (cpu.R[rx] == 0);
            }
            break;
        case 0x4:  // 0100: SUB RX - RY → RX
            {
                uint8_t rx = (instruction >> 2) & 0x03;  // Bits 3-2
                uint8_t ry = instruction & 0x03;         // Bits 1-0
                int16_t result = (int16_t)cpu.R[rx] - (int16_t)cpu.R[ry];
                cpu.SR.OF = (result < 0);                // Overflow if result is negative
                cpu.R[rx] = (uint8_t)result;             // Store the lower 8 bits
                cpu.SR.ZF = (cpu.R[rx] == 0);
            }
            break;
        case 0x5:  // 0101: SHIFT Operations
            if (dd == 0x00) {
                // LEFT SHIFT RX << 4 → RX
                uint8_t rx = xx;
                cpu.R[rx] <<= 4;
                // STATUS not updated
            } else if (dd == 0x01) {
                // RIGHT SHIFT RX >> 4 → RX
                uint8_t rx = xx;
                cpu.R[rx] >>= 4;
                // STATUS not updated
            }
            break;
        case 0x6:  // 0110: CMP RX == 0 or OUTPUT RX
            if (dd == 0x00) {
                // COMPARE RX == 0
                uint8_t rx = xx;
                cpu.SR.ZF = (cpu.R[rx] == 0);
            } else if (dd == 0x01) {
                // OUTPUT RX
                uint8_t rx = xx;
                printf("Output from R%d: %u\n", rx, cpu.R[rx]);
            }
            break;
        case 0x7:  // 0111: INPUT into RX
            {
                uint8_t rx = xx;
                printf("Enter value for R%d (0-255): ", rx);
                scanf("%hhu", &cpu.R[rx]);
            }
            break;
        case 0xE:  // 1110: Conditional Branch if Zero Flag is set
            {
                uint8_t rx = xx;
                if (cpu.SR.ZF) {
                    cpu.PC = cpu.R[rx];
                }
            }
            break;
        case 0xF:  // 1111: Unconditional Branch
            {
                uint8_t rx = xx;
                cpu.PC = cpu.R[rx];
            }
            break;
        default:
            // Invalid opcode; handle error
            printf("Invalid opcode: 0x%X at PC: 0x%X\n", opcode, cpu.PC);
            break;
    }
}

/**
 * @brief Loads a program into the ROM.
 *
 * This function copies the program code into the ROM array, up to the maximum
 * ROM size. It ensures that the program does not exceed the ROM capacity.
 *
 * @param program Pointer to the array containing the program code.
 * @param size Size of the program code in bytes.
 */
void load_program(const uint8_t *program, size_t size) {
    if (size > ROM_SIZE) {
        size = ROM_SIZE;  // Truncate program if it exceeds ROM size
    }
    memcpy(ROM, program, size);
}

/**
 * @brief Main function to initialize and run the CPU simulator.
 *
 * This function initializes the CPU and memory, loads a sample program,
 * and starts the fetch-decode-execute loop. It runs until it reaches a NOP instruction.
 *
 * @return int Exit code of the program.
 */
int main() {
    // Initialize CPU
    memset(&cpu, 0, sizeof(cpu));

    // Sample program demonstrating addition overflow
    uint8_t program[] = {
        0x20, 0xF0,   // 0010 0000: Immediate value 0xF0 into R0 (240)
        0x21, 0x20,   // 0010 0001: Immediate value 0x20 into R1 (32)
        0x33,         // 0011 0011: ADD R0 + R1 -> R0 (R0 = 240 + 32 = 272)
        // Expected overflow since 272 > 255
        0x65,         // 0110 0101: OUTPUT R0
        // Check overflow flag
        0x60,         // 0110 0000: COMPARE R0 == 0
        0xE0,         // 1110 0000: Conditional Branch to R0 if Zero Flag is set
        // NOP (end of program)
        0x00
    };

    // Load program into ROM
    load_program(program, sizeof(program));

    // Main loop
    while (1) {
        uint8_t instruction;
        fetch(&instruction);
        if (instruction == 0x00) {
            // NOP or HALT
            break;
        }
        decode_execute(instruction);
    }

    // After execution, check the Overflow Flag
    if (cpu.SR.OF) {
        printf("Overflow occurred during addition.\n");
    } else {
        printf("No overflow occurred.\n");
    }

    return 0;
}




test
/**
 * Execute the instruction based on the opcode and operands.
 * @param opcode The decoded opcode.
 * @param operand The operand (lower 4 bits of the instruction).
 */
void execute(Opcode opcode, uint8_t operand) {
    uint8_t rx, ry, temp;

    switch (opcode) {
        case MEM_TO_R0: // Memory to r0
            registers[0] = RAM[operand];
            status = (registers[0] == 0) ? ZERO_FLAG : 0;
            break;

        case R0_TO_MEM: // r0 to Memory
            RAM[operand] = registers[0];
            break;

        case REG_TRANSFER: // Register transfer
            rx = (operand >> 2) & 0x03;
            ry = operand & 0x03;
            registers[ry] = registers[rx];
            status = (registers[ry] == 0) ? ZERO_FLAG : 0;
            break;

        case IMM_SET: // Immediate value set
            registers[0] = operand;
            status = (registers[0] == 0) ? ZERO_FLAG : 0;
            break;

        case ADD: // Addition
            rx = (operand >> 2) & 0x03;
            ry = operand & 0x03;
            temp = registers[rx] + registers[ry];
            registers[rx] = temp;
            status = (temp == 0 ? ZERO_FLAG : 0) | ((temp < registers[ry]) ? OF_FLAG : 0);
            break;

        case SUB: // Subtraction
            rx = (operand >> 2) & 0x03;
            ry = operand & 0x03;
            temp = registers[rx] - registers[ry];
            registers[rx] = temp;
            status = (temp == 0 ? ZERO_FLAG : 0) | ((registers[rx] > registers[ry]) ? OF_FLAG : 0);
            break;

        case LEFT_SHIFT: // Left shift
            rx = operand & 0x03;
            registers[rx] <<= 4;
            break;

        case RIGHT_SHIFT: // Right shift
            rx = operand & 0x03;
            registers[rx] >>= 4;
            break;

        case COMPARE: // Compare
            rx = (operand >> 2) & 0x03;
            ry = operand & 0x03;
            status = (registers[rx] == registers[ry]) ? ZERO_FLAG : 0;
            break;

        case OUTPUT: // External output
            printf("Output: %d\n", registers[operand & 0x03]);
            break;

        case INPUT: // External input
            printf("Enter input: ");
            scanf("%hhu", &registers[operand & 0x03]);
            break;

        case CONDITIONAL_BRANCH: // Conditional branch
            if (status & ZERO_FLAG) {
                PC = registers[operand & 0x03];
            }
            break;

        case UNCONDITIONAL_BRANCH: // Unconditional branch
            PC = registers[operand & 0x03];
            break;

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
            break;
    }
}
