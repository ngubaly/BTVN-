#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// UART Configuration
#define UART_ID         uart0
#define BAUD_RATE       9600
#define UART_TX_PIN     0
#define UART_RX_PIN     1

// CPU Definitions
#define ROM_SIZE        128
#define RAM_SIZE        128

// Status Register Flags
#define STATUS_ZERO     0x01
#define STATUS_OF       0x02

// General-purpose Registers
typedef struct {
    uint8_t r0;
    uint8_t r1;
    uint8_t r2;
    uint8_t r3;
} GP_Registers;

// CPU State
typedef struct {
    uint8_t PC;                    // Program Counter
    GP_Registers regs;             // General-purpose registers
    uint8_t status;                // Status register (Zero and Overflow flags)
    uint8_t program_rom[ROM_SIZE]; // Program ROM
    uint8_t program_ram[RAM_SIZE]; // Program RAM
} CPU_State;

// Function Prototypes
void initialize_cpu(CPU_State *cpu);
void load_factorial_program(CPU_State *cpu);
void fetch_instruction(CPU_State *cpu, uint8_t *instruction);
void decode_and_execute(CPU_State *cpu, uint8_t instruction, bool *end_flag);
void execute_instruction(CPU_State *cpu, uint8_t instruction, bool *end_flag);

// Instruction Execution Functions
void instr_mem_to_r0(CPU_State *cpu, uint8_t rx);
void instr_r0_to_mem(CPU_State *cpu, uint8_t rx);
void instr_transfer_reg(CPU_State *cpu, uint8_t rx, uint8_t ry);
void instr_immediate_set(CPU_State *cpu, uint8_t value);
void instr_add(CPU_State *cpu, uint8_t rx, uint8_t ry);
void instr_sub(CPU_State *cpu, uint8_t rx, uint8_t ry);
void instr_shift_left(CPU_State *cpu, uint8_t rx);
void instr_shift_right(CPU_State *cpu, uint8_t rx);
void instr_compare(CPU_State *cpu, uint8_t rx);
void instr_output(CPU_State *cpu, uint8_t rx);
void instr_input(CPU_State *cpu, uint8_t rx);
void instr_cond_branch(CPU_State *cpu, uint8_t rx, bool *end_flag);
void instr_uncond_branch(CPU_State *cpu, uint8_t rx, bool *end_flag);

// Utility Functions
uint8_t get_register_value(CPU_State *cpu, uint8_t reg);
void set_register_value(CPU_State *cpu, uint8_t reg, uint8_t value);
void update_zero_flag(CPU_State *cpu, uint8_t result);
void update_overflow_flag(CPU_State *cpu, int result);

// Main Function
int main() {
    // Initialize UART
    stdio_init_all();
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Initialize CPU
    CPU_State cpu;
    initialize_cpu(&cpu);

    // Load Factorial Program into ROM
    load_factorial_program(&cpu);

    // Execute the program
    bool end_flag = false;
    while (!end_flag) {
        uint8_t instruction;
        fetch_instruction(&cpu, &instruction);
        decode_and_execute(&cpu, instruction, &end_flag);
    }

    uart_puts(UART_ID, "Program execution completed.\n\r");
    return 0;
}

// Initialize CPU State
void initialize_cpu(CPU_State *cpu) {
    cpu->PC = 0;
    cpu->regs.r0 = 0;
    cpu->regs.r1 = 0;
    cpu->regs.r2 = 0;
    cpu->regs.r3 = 0;
    cpu->status = 0;
    memset(cpu->program_rom, 0, ROM_SIZE);
    memset(cpu->program_ram, 0, RAM_SIZE);
}

// Load Factorial Program into ROM
void load_factorial_program(CPU_State *cpu) {
    // Chương trình tính giai thừa:
    // 1. Nhập N từ UART vào r1
    // 2. Đặt r2 = 1 (accumulator)
    // 3. Đặt r3 = r1 (counter)
    // 4. Loop:
    //    a. So sánh r3 == 0
    //       - Nếu đúng, nhảy đến end
    //    b. Multiply r2 = r2 * r3 bằng cách sử dụng phép cộng
    //       i. Đặt r0 = r2
    //       ii. Đặt r2 = 0
    //       iii. Đặt r4 (sử dụng r1) = r3
    //       iv. Inner Loop:
    //           - So sánh r1 == 0
    //               + Nếu đúng, nhảy đến sau inner loop
    //           - Cộng r0 vào r2
    //           - Trừ 1 từ r1
    //           - Nhảy lại vào inner loop
    //    c. Đặt r3 = r3 - 1
    //    d. Nhảy lại vào Loop
    // 5. End:
    //    a. Xuất r2 (kết quả) ra UART
    //    b. Nhảy vô hạn để kết thúc chương trình

    // Mã lệnh đã mã hóa:
    uint8_t factorial_program[] = {
        // 1. Input N vào r1
        0xD1, // 1101 ddxx: Nhập vào r1 (xx=01)

        // 2. Đặt r2 = 1
        0x5    // 0101 xxxx: Đặt r0 = 0001 (1)
              // Followed by 0xC2: Transfer r0 vào r2
              // But as per instruction set, we need to use transfer instruction
        // Since 0101 xxxx chỉ đặt giá trị vào r0, chúng ta cần chuyển r0 vào r2
              // 0100 xxyy: Chuyển từ r0 vào r2 (x=00, y=10)
        0x42, // 0100 xxyy: r0 → r2 (xx=00, yy=10)

        // 3. Đặt r3 = r1
        // 0100 xxyy: Chuyển từ r1 vào r3 (x=01, y=11)
        0x43, // 0100 xxyy: r1 → r3

        // 4a. So sánh r3 == 0
        0xA0, // 1010 ddxx: So sánh r3 ==0 (xx=00)

        // 4a. Nếu r3 ==0, nhảy đến end (PC = address của bước 5)
        // Giả sử bước 5 bắt đầu tại địa chỉ 13 (0x0D)
        // Set r0 = 0x0D (13)
        0x5D, // 0101 xxxx: Đặt r0 = 1101 (13 in hex is 0x0D)
              // But 0101 xxxx: xxxx = 1101 (13)

        // 4a. Nhảy có điều kiện đến end nếu Zero
        0xE0, // 1110 ddxx: Conditional Branch to r0 (xx=00)

        // 4b. Multiply r2 = r2 * r3 bằng cách sử dụng phép cộng
        // i. Đặt r0 = r2
        // 0100 xxyy: Chuyển từ r2 vào r0 (x=10, y=00)
        0x40, // 0100 xxyy: r2 → r0

        // ii. Đặt r2 = 0
        // 0101 xxxx: Đặt r0 = 0000
        0x50, // 0101 xxxx: Đặt r0 = 0000

        // iii. Đặt r1 = r3 (sử dụng r1 làm inner loop counter)
        // 0100 xxyy: Chuyển từ r3 vào r1 (x=11, y=01)
        0x41, // 0100 xxyy: r3 → r1

        // iv. Inner Loop:
        // So sánh r1 == 0
        0xA1, // 1010 ddxx: So sánh r1 ==0 (xx=01)

        // So sánh r1 ==0, nếu đúng, nhảy đến sau inner loop (địa chỉ 10)
        // Set r0 = địa chỉ sau inner loop, giả sử là 12 (0x0C)
        0x5C, // 0101 xxxx: Đặt r0 = 1100 (12)

        // Nhảy có điều kiện đến sau inner loop
        0xE1, // 1110 ddxx: Conditional Branch to r0 (xx=01)

        // Cộng r0 vào r2
        // 0110 xxyy: Add (r0 + r2 → r0)
        0x60, // 0110 xxyy: Add r0 + r2 → r0 (x=00, y=00)

        // Đặt r2 = r0
        // 0100 xxyy: Chuyển từ r0 vào r2 (x=00, y=10)
        0x42, // 0100 xxyy: r0 → r2

        // Trừ 1 từ r1
        // Đặt r0 = 1
        0x51, // 0101 xxxx: Đặt r0 = 0001

        // Trừ r0 từ r1
        // 0111 xxyy: Subtract (r1 - r0 → r1) (x=01, y=00)
        0x71, // 0111 xxyy: Subtract r0 từ r1 (x=01, y=01)

        // Nhảy lại vào inner loop (địa chỉ 8)
        0xF1, // 1111 ddxx: Unconditional Branch to r1 (xx=01)

        // 5a. Xuất kết quả từ r2
        0xC2, // 1100 ddxx: Output từ r2 (xx=10)

        // 5b. Nhảy vô hạn để kết thúc chương trình
        // Đặt r0 = địa chỉ hiện tại (địa chỉ 12)
        0x5C, // 0101 xxxx: Đặt r0 = 1100 (12)

        // Nhảy không điều kiện tới r0 (địa chỉ 12) để tạo vòng lặp vô hạn
        0xF2  // 1111 ddxx: Unconditional Branch to r2 (xx=10)
    };

    // Load the factorial program into program_rom
    memcpy(cpu->program_rom, factorial_program, sizeof(factorial_program));

    // Fill the rest of ROM with NOPs or similar (optional)
    for (int i = sizeof(factorial_program); i < ROM_SIZE; i++) {
        cpu->program_rom[i] = 0x00; // Assuming 0x00 is NOP or unused
    }

    uart_puts(UART_ID, "Factorial program loaded into ROM.\n\r");
}

// Fetch Instruction from ROM
void fetch_instruction(CPU_State *cpu, uint8_t *instruction) {
    if (cpu->PC >= ROM_SIZE) {
        uart_puts(UART_ID, "Program Counter out of ROM bounds.\n\r");
        *instruction = 0xFF; // Undefined opcode to halt
        return;
    }
    *instruction = cpu->program_rom[cpu->PC];
}

// Decode and Execute Instruction
void decode_and_execute(CPU_State *cpu, uint8_t instruction, bool *end_flag) {
    execute_instruction(cpu, instruction, end_flag);
}

// Execute Instruction Based on Opcode
void execute_instruction(CPU_State *cpu, uint8_t instruction, bool *end_flag) {
    uint8_t opcode = (instruction & 0xF0) >> 4; // Upper 4 bits
    uint8_t operand = (instruction & 0x0F);    // Lower 4 bits

    switch (opcode) {
        case 0x0:
            // 0000 ddxx: Address indicated by rx ⇒ r0
            instr_mem_to_r0(cpu, operand & 0x03); // rx là 2 bit thấp
            break;
        case 0x2:
            // 0010 ddxx: r0 ⇒ Address indicated by rx
            instr_r0_to_mem(cpu, operand & 0x03); // rx là 2 bit thấp
            break;
        case 0x4:
            // 0100 xxyy: Transfer giữa các thanh ghi (rx → ry)
            {
                uint8_t rx = (operand & 0xC) >> 2; // 2 bit cao
                uint8_t ry = operand & 0x3;        // 2 bit thấp
                instr_transfer_reg(cpu, rx, ry);
            }
            break;
        case 0x5:
            // 0101 xxxx: Đặt giá trị lập tức vào r0
            {
                uint8_t value = operand; // 4 bit immediate
                instr_immediate_set(cpu, value);
            }
            break;
        case 0x6:
            // 0110 xxyy: Add (rx + ry → rx)
            {
                uint8_t rx = (operand & 0xC) >> 2;
                uint8_t ry = operand & 0x3;
                instr_add(cpu, rx, ry);
            }
            break;
        case 0x7:
            // 0111 xxyy: Subtract (rx - ry → rx)
            {
                uint8_t rx = (operand & 0xC) >> 2;
                uint8_t ry = operand & 0x3;
                instr_sub(cpu, rx, ry);
            }
            break;
        case 0x8:
            // 1000 ddxx: Shift left (rx << 4 → rx)
            {
                uint8_t rx = operand & 0x3;
                instr_shift_left(cpu, rx);
            }
            break;
        case 0x9:
            // 1001 ddxx: Shift right (rx >> 4 → rx)
            {
                uint8_t rx = operand & 0x3;
                instr_shift_right(cpu, rx);
            }
            break;
        case 0xA:
            // 1010 ddxx: Compare (rx == 0)
            {
                uint8_t rx = operand & 0x3;
                instr_compare(cpu, rx);
            }
            break;
        case 0xC:
            // 1100 ddxx: Output từ rx
            {
                uint8_t rx = operand & 0x3;
                instr_output(cpu, rx);
            }
            break;
        case 0xD:
            // 1101 ddxx: Input vào rx
            {
                uint8_t rx = operand & 0x3;
                instr_input(cpu, rx);
            }
            break;
        case 0xE:
            // 1110 ddxx: Conditional Branch (nếu Zero = 1, PC ← rx)
            {
                uint8_t rx = operand & 0x3;
                instr_cond_branch(cpu, rx, end_flag);
            }
            break;
        case 0xF:
            // 1111 ddxx: Unconditional Branch (PC ← rx)
            {
                uint8_t rx = operand & 0x3;
                instr_uncond_branch(cpu, rx, end_flag);
            }
            break;
        default:
            uart_puts(UART_ID, "Undefined opcode encountered.\n\r");
            *end_flag = true;
            break;
    }
}

// Instruction Implementations

// 1. 0000 ddxx: Address indicated by rx ⇒ r0
void instr_mem_to_r0(CPU_State *cpu, uint8_t rx) {
    if (rx > 0x3) {
        uart_puts(UART_ID, "Invalid register index in mem_to_r0.\n\r");
        return;
    }
    uint8_t address = get_register_value(cpu, rx);
    if (address < 0x80 || address >= 0x100) {
        uart_puts(UART_ID, "Memory address out of bounds in mem_to_r0.\n\r");
        return;
    }
    uint8_t value = cpu->program_ram[address - 0x80];
    set_register_value(cpu, 0, value); // r0 = Memory[address]
    update_zero_flag(cpu, value);
    cpu->PC += 1;
}

// 2. 0010 ddxx: r0 ⇒ Address indicated by rx
void instr_r0_to_mem(CPU_State *cpu, uint8_t rx) {
    if (rx > 0x3) {
        uart_puts(UART_ID, "Invalid register index in r0_to_mem.\n\r");
        return;
    }
    uint8_t address = get_register_value(cpu, rx);
    if (address < 0x80 || address >= 0x100) {
        uart_puts(UART_ID, "Memory address out of bounds in r0_to_mem.\n\r");
        return;
    }
    uint8_t value = get_register_value(cpu, 0); // r0 value
    cpu->program_ram[address - 0x80] = value;
    update_zero_flag(cpu, value);
    cpu->PC += 1;
}

// 3. 0100 xxyy: Transfer giữa các thanh ghi (rx → ry)
void instr_transfer_reg(CPU_State *cpu, uint8_t rx, uint8_t ry) {
    if (rx > 0x3 || ry > 0x3) {
        uart_puts(UART_ID, "Invalid register index in transfer_reg.\n\r");
        return;
    }
    uint8_t value = get_register_value(cpu, rx);
    set_register_value(cpu, ry, value);
    update_zero_flag(cpu, value);
    cpu->PC += 1;
}

// 4. 0101 xxxx: Đặt giá trị lập tức vào r0
void instr_immediate_set(CPU_State *cpu, uint8_t value) {
    set_register_value(cpu, 0, value); // Set r0
    update_zero_flag(cpu, value);
    cpu->PC += 1;
}

// 5. 0110 xxyy: Add (rx + ry → rx)
void instr_add(CPU_State *cpu, uint8_t rx, uint8_t ry) {
    if (rx > 0x3 || ry > 0x3) {
        uart_puts(UART_ID, "Invalid register index in add.\n\r");
        return;
    }
    uint8_t a = get_register_value(cpu, rx);
    uint8_t b = get_register_value(cpu, ry);
    uint16_t result = a + b;
    set_register_value(cpu, rx, result & 0xFF);
    update_zero_flag(cpu, result & 0xFF);
    // Update Overflow Flag
    if (result > 0xFF) {
        cpu->status |= STATUS_OF;
    }
    else {
        cpu->status &= ~STATUS_OF;
    }
    cpu->PC += 1;
}

// 6. 0111 xxyy: Subtract (rx - ry → rx)
void instr_sub(CPU_State *cpu, uint8_t rx, uint8_t ry) {
    if (rx > 0x3 || ry > 0x3) {
        uart_puts(UART_ID, "Invalid register index in sub.\n\r");
        return;
    }
    int16_t a = get_register_value(cpu, rx);
    int16_t b = get_register_value(cpu, ry);
    int16_t result = a - b;
    set_register_value(cpu, rx, result & 0xFF);
    update_zero_flag(cpu, result & 0xFF);
    // Update Overflow Flag
    if (result < 0) {
        cpu->status |= STATUS_OF;
    }
    else {
        cpu->status &= ~STATUS_OF;
    }
    cpu->PC += 1;
}

// 7. 1000 ddxx: Shift left (rx << 4 → rx)
void instr_shift_left(CPU_State *cpu, uint8_t rx) {
    if (rx > 0x3) {
        uart_puts(UART_ID, "Invalid register index in shift_left.\n\r");
        return;
    }
    uint8_t value = get_register_value(cpu, rx);
    value <<= 4;
    set_register_value(cpu, rx, value);
    // No status update
    cpu->PC += 1;
}

// 8. 1001 ddxx: Shift right (rx >> 4 → rx)
void instr_shift_right(CPU_State *cpu, uint8_t rx) {
    if (rx > 0x3) {
        uart_puts(UART_ID, "Invalid register index in shift_right.\n\r");
        return;
    }
    uint8_t value = get_register_value(cpu, rx);
    value >>= 4;
    set_register_value(cpu, rx, value);
    // No status update
    cpu->PC += 1;
}

// 9. 1010 ddxx: Compare (rx == 0)
void instr_compare(CPU_State *cpu, uint8_t rx) {
    if (rx > 0x3) {
        uart_puts(UART_ID, "Invalid register index in compare.\n\r");
        return;
    }
    uint8_t value = get_register_value(cpu, rx);
    if (value == 0) {
        cpu->status |= STATUS_ZERO;
    }
    else {
        cpu->status &= ~STATUS_ZERO;
    }
    cpu->PC += 1;
}

// 10. 1100 ddxx: Output từ rx
void instr_output(CPU_State *cpu, uint8_t rx) {
    if (rx > 0x3) {
        uart_puts(UART_ID, "Invalid register index in output.\n\r");
        return;
    }
    uint8_t value = get_register_value(cpu, rx);
    char buffer[6]; // Enough to hold "255\r\n"
    snprintf(buffer, sizeof(buffer), "%d\r\n", value);
    uart_puts(UART_ID, buffer);
    cpu->PC += 1;
}

// 11. 1101 ddxx: Input vào rx
void instr_input(CPU_State *cpu, uint8_t rx) {
    if (rx > 0x3) {
        uart_puts(UART_ID, "Invalid register index in input.\n\r");
        return;
    }
    uart_puts(UART_ID, "Enter a number (0-255): ");
    char input_buffer[4] = {0};
    int idx = 0;
    while (idx < 3) { // Max 3 digits for 0-255
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            if (c == '\n' || c == '\r') {
                break;
            }
            if (isdigit(c)) {
                uart_putc(UART_ID, c); // Echo the character
                input_buffer[idx++] = c;
            }
        }
    }
    input_buffer[idx] = '\0';
    uint8_t value = (uint8_t)atoi(input_buffer);
    set_register_value(cpu, rx, value);
    update_zero_flag(cpu, value);
    uart_puts(UART_ID, "\r\n");
    cpu->PC += 1;
}

// 12. 1110 ddxx: Conditional Branch (nếu Zero = 1, PC ← giá trị trong rx)
void instr_cond_branch(CPU_State *cpu, uint8_t rx, bool *end_flag) {
    if (rx > 0x3) {
        uart_puts(UART_ID, "Invalid register index in cond_branch.\n\r");
        *end_flag = true;
        return;
    }
    if (cpu->status & STATUS_ZERO) {
        uint8_t branch_address = get_register_value(cpu, rx);
        if (branch_address < ROM_SIZE) {
            cpu->PC = branch_address;
        }
        else {
            uart_puts(UART_ID, "Branch address out of bounds.\n\r");
            *end_flag = true;
        }
    }
    else {
        cpu->PC += 1;
    }
}

// 13. 1111 ddxx: Unconditional Branch (PC ← giá trị trong rx)
void instr_uncond_branch(CPU_State *cpu, uint8_t rx, bool *end_flag) {
    if (rx > 0x3) {
        uart_puts(UART_ID, "Invalid register index in uncond_branch.\n\r");
        *end_flag = true;
        return;
    }
    uint8_t branch_address = get_register_value(cpu, rx);
    if (branch_address < ROM_SIZE) {
        cpu->PC = branch_address;
    }
    else {
        uart_puts(UART_ID, "Branch address out of bounds.\n\r");
        *end_flag = true;
    }
}

// Utility Functions

// Get Register Value
uint8_t get_register_value(CPU_State *cpu, uint8_t reg) {
    switch (reg) {
        case 0: return cpu->regs.r0;
        case 1: return cpu->regs.r1;
        case 2: return cpu->regs.r2;
        case 3: return cpu->regs.r3;
        default: return 0;
    }
}

// Set Register Value
void set_register_value(CPU_State *cpu, uint8_t reg, uint8_t value) {
    switch (reg) {
        case 0: cpu->regs.r0 = value; break;
        case 1: cpu->regs.r1 = value; break;
        case 2: cpu->regs.r2 = value; break;
        case 3: cpu->regs.r3 = value; break;
        default: break;
    }
}

// Update Zero Flag
void update_zero_flag(CPU_State *cpu, uint8_t result) {
    if (result == 0) {
        cpu->status |= STATUS_ZERO;
    }
    else {
        cpu->status &= ~STATUS_ZERO;
    }
}

// Update Overflow Flag
void update_overflow_flag(CPU_State *cpu, int result) {
    if (result > 0xFF || result < 0) {
        cpu->status |= STATUS_OF;
    }
    else {
        cpu->status &= ~STATUS_OF;
    }
}
