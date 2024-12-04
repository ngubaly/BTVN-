#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"

// Định nghĩa kích thước bộ nhớ
#define PROGRAM_ROM_SIZE 128
#define PROGRAM_RAM_SIZE 128

// Định nghĩa các thanh ghi
typedef struct {
    uint16_t PC;           // Program Counter (8-bit đủ cho 128 bytes)
    uint8_t r[4];          // Các thanh ghi tổng quát r0 - r3
    struct {
        uint8_t Zero : 1;  // Cờ Zero
        uint8_t OF : 1;    // Cờ Overflow
    } STATUS;
} CPU_State;

// Khai báo bộ nhớ
uint8_t program_ROM[PROGRAM_ROM_SIZE];
uint8_t program_RAM[PROGRAM_RAM_SIZE];

// Hàm khởi tạo CPU
void initialize_CPU(CPU_State *cpu) {
    cpu->PC = 0;                   // Khởi động PC từ 0
    for(int i = 0; i < 4; i++) {
        cpu->r[i] = 0;             // Khởi động các thanh ghi tổng quát
    }
    cpu->STATUS.Zero = 0;
    cpu->STATUS.OF = 0;
}

// Hàm Fetch: Lấy lệnh từ Program ROM
uint8_t fetch_instruction(CPU_State *cpu) {
    if(cpu->PC >= PROGRAM_ROM_SIZE) {
        printf("Program Counter out of bounds!\n");
        return 0xFF; // Mã lệnh dừng hoặc lỗi
    }
    return program_ROM[cpu->PC++];
}

// Hàm Decode và Execute
void decode_execute(CPU_State *cpu, uint8_t instruction) {
    uint8_t opcode = instruction >> 4;    // 4 bit cao
    uint8_t operand = instruction & 0x0F; // 4 bit thấp

    switch(opcode) {
        case 0x0: { // Các lệnh Data Transfer (0000)
            uint8_t xx = operand & 0x03; // 2 bit thấp xác định rx
            // Kiểm tra bit thứ 2 để phân biệt lệnh
            // Nếu bit 2 = 0: Transfer từ memory tới r0
            // Nếu bit 2 = 1: Transfer từ r0 tới memory
            if ((operand & 0x0C) == 0x00) { 
                // Lệnh 1: Memory → r0
                uint8_t rx = xx;
                if(rx >=4){
                    printf("Invalid register r%d\n", rx);
                    break;
                }
                uint8_t address = cpu->r[rx] & 0x7F; // Giới hạn địa chỉ 0-7F
                cpu->r[0] = program_RAM[address];
                cpu->STATUS.Zero = (cpu->r[0] == 0) ? 1 : 0;
            }
            else { 
                // Lệnh 2: r0 → Memory
                uint8_t rx = xx;
                if(rx >=4){
                    printf("Invalid register r%d\n", rx);
                    break;
                }
                uint8_t address = cpu->r[rx] & 0x7F; // Giới hạn địa chỉ 0-7F
                program_RAM[address] = cpu->r[0];
                cpu->STATUS.Zero = (cpu->r[0] == 0) ? 1 : 0;
            }
            break;
        }
        case 0x1: { // Lệnh Chuyển dữ liệu giữa các thanh ghi (0001 xxyy)
            uint8_t rx = (operand >> 2) & 0x03;
            uint8_t ry = operand & 0x03;
            if(rx >=4 || ry >=4){
                printf("Invalid register r%d or r%d\n", rx, ry);
                break;
            }
            cpu->r[ry] = cpu->r[rx];
            cpu->STATUS.Zero = (cpu->r[ry] == 0) ? 1 : 0;
            break;
        }
        case 0x2: { // Lệnh Đặt giá trị lập tức vào r0 (0010 xxxx)
            uint8_t immediate = operand;
            cpu->r[0] = immediate;
            cpu->STATUS.Zero = (cpu->r[0] == 0) ? 1 : 0;
            break;
        }
        case 0x3: { // Lệnh Cộng giá trị thanh ghi (0011 xxyy)
            uint8_t rx = (operand >> 2) & 0x03;
            uint8_t ry = operand & 0x03;
            if(rx >=4 || ry >=4){
                printf("Invalid register r%d or r%d\n", rx, ry);
                break;
            }
            uint16_t result = (uint16_t)cpu->r[rx] + (uint16_t)cpu->r[ry];
            cpu->STATUS.Zero = ((uint8_t)result == 0) ? 1 : 0;
            cpu->STATUS.OF = (result > 0xFF) ? 1 : 0;
            cpu->r[rx] = (uint8_t)result;
            break;
        }
        case 0x4: { // Lệnh Trừ giá trị thanh ghi (0100 xxyy)
            uint8_t rx = (operand >> 2) & 0x03;
            uint8_t ry = operand & 0x03;
            if(rx >=4 || ry >=4){
                printf("Invalid register r%d or r%d\n", rx, ry);
                break;
            }
            int16_t result = (int16_t)cpu->r[rx] - (int16_t)cpu->r[ry];
            cpu->STATUS.Zero = ((uint8_t)result == 0) ? 1 : 0;
            cpu->STATUS.OF = (result < 0) ? 1 : 0;
            cpu->r[rx] = (uint8_t)result;
            break;
        }
        case 0x5: { // Lệnh Bit Shift (0101 ddxx)
            uint8_t xx = operand & 0x03; // Xác định rx
            uint8_t direction = (operand >> 2) & 0x01; // Bit 2 xác định hướng shift
            uint8_t rx = xx;
            if(rx >=4){
                printf("Invalid register r%d\n", rx);
                break;
            }
            if(direction == 0) {
                // Dịch trái 4 bit
                cpu->r[rx] = cpu->r[rx] << 4;
            }
            else {
                // Dịch phải 4 bit
                cpu->r[rx] = cpu->r[rx] >> 4;
            }
            // Không cập nhật STATUS
            break;
        }
        case 0x6: { // Các lệnh So sánh hoặc Xuất dữ liệu (0110 ddxx)
            uint8_t xx = operand & 0x03; // Xác định rx
            uint8_t command = (operand >> 2) & 0x03; // Bit 2-3 xác định lệnh
            uint8_t rx = xx;
            if(rx >=4){
                printf("Invalid register r%d\n", rx);
                break;
            }
            if(command == 0x0) {
                // Lệnh 9: So sánh (rx == 0)
                cpu->STATUS.Zero = (cpu->r[rx] == 0) ? 1 : 0;
            }
            else if(command == 0x1) {
                // Lệnh 10: Xuất dữ liệu từ rx ra ngoài (Output)
                printf("Output (r%d): %d\n", rx, cpu->r[rx]);
            }
            else {
                printf("Undefined command in opcode 0x6\n");
            }
            break;
        }
        case 0x7: { // Lệnh Nhập dữ liệu từ ngoài vào rx (0111 ddxx)
            uint8_t xx = operand & 0x03; // Xác định rx
            uint8_t rx = xx;
            if(rx >=4){
                printf("Invalid register r%d\n", rx);
                break;
            }
            // Nhập từ UART: chờ và đọc một byte nếu có dữ liệu
            if (uart_is_readable(uart0)) {
                uint8_t data = uart_getc(uart0);
                cpu->r[rx] = data;
            }
            break;
        }
        case 0xE: { // Lệnh Nhảy có điều kiện (1110 ddxx)
            uint8_t xx = operand & 0x03; // Xác định rx
            uint8_t rx = xx;
            if(rx >=4){
                printf("Invalid register r%d\n", rx);
                break;
            }
            if(cpu->STATUS.Zero) {
                cpu->PC = cpu->r[rx];
            }
            break;
        }
        case 0xF: { // Lệnh Nhảy không điều kiện (1111 ddxx)
            uint8_t xx = operand & 0x03; // Xác định rx
            uint8_t rx = xx;
            if(rx >=4){
                printf("Invalid register r%d\n", rx);
                break;
            }
            cpu->PC = cpu->r[rx];
            break;
        }
        default:
            printf("Unknown instruction: 0x%02X at PC=%d\n", instruction, cpu->PC -1);
            break;
    }
}

// Chương trình mẫu: Sử dụng UART để nhận hai số, cộng và xuất kết quả
// Giả sử chương trình ROM được lập trình trước với các lệnh tương ứng
void load_sample_program() {
    // Reset chương trình ROM
    memset(program_ROM, 0, sizeof(program_ROM));

    /*
    Chương trình mẫu bao gồm các lệnh:
    1. Nhập số thứ nhất vào r1
    2. Nhập số thứ hai vào r2
    3. Cộng r1 và r2, kết quả vào r1
    4. Xuất kết quả từ r1
    5. Nhảy vô hạn về địa chỉ 0 để lặp lại
    */

    // Lệnh 11: Nhập dữ liệu vào r1 -> opcode 0111 (0x7), dd=01 (r1)
    // Mã lệnh: 0111 ddxx -> 0111 01xx (xx=01 cho r1)
    program_ROM[0] = 0x75; // 0111 0101 (dd=01, xx=01) - Nhập vào r1

    // Lệnh 11: Nhập dữ liệu vào r2 -> opcode 0111 (0x7), dd=10 (r2)
    // Mã lệnh: 0111 10xx (xx=10 cho r2)
    program_ROM[1] = 0x7A; // 0111 1010 (dd=10, xx=10) - Nhập vào r2

    // Lệnh 5: Cộng r1 và r2 -> opcode 0011 (0x3), xxyy=r1 + r2
    // Mã lệnh: 0011 xxyy (x=01 cho r1, y=10 cho r2)
    // r1 = 01, r2 = 10 -> xxyy = 0110
    program_ROM[2] = 0x36; // 0011 0110 - Cộng r1 và r2, lưu vào r1

    // Lệnh 10: Xuất dữ liệu từ r1 -> opcode 0110 (0x6), dd=01 (r1)
    // Mã lệnh: 0110 01xx (xx=01 cho r1)
    program_ROM[3] = 0x69; // 0110 1001 - Xuất r1

    // Lệnh 13: Nhảy không điều kiện đến địa chỉ r0 (vô hạn)
    // Opcode: 1111 (0xF), dd=00 (r0)
    // Mã lệnh: 1111 00xx (xx=00 cho r0)
    program_ROM[4] = 0xF0; // 1111 0000 - Nhảy đến r0

    // Lệnh đặt r0 = 0 để nhảy vô hạn về địa chỉ 0
    // Sử dụng lệnh Đặt giá trị lập tức vào r0: 0010 xxxx (opcode 0x2)
    // immediate = 0
    program_ROM[5] = 0x20; // 0010 0000 - Đặt r0 = 0

    // Giữ các lệnh sau (từ địa chỉ 6 trở đi) không cần thiết
    // Vì PC sẽ được đặt lại về 0 liên tục
}

// Hàm cấu hình UART
void setup_UART() {
    // Khởi tạo UART0 với tốc độ 9600 bps
    uart_init(uart0, 9600);
    // Chọn GPIO 0 làm TX và GPIO 1 làm RX
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    // Cấu hình UART
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(uart0, false);
}

// Hàm hiển thị thông tin khi dữ liệu được xuất
void uart_put_output(uint8_t data){
    // Gửi dữ liệu qua UART
    uart_putc(uart0, data);
}

int main() {
    // Khởi tạo tiêu chuẩn I/O
    stdio_init_all();

    // Thiết lập UART
    setup_UART();

    // Khởi tạo CPU
    CPU_State cpu;
    initialize_CPU(&cpu);

    // Tải chương trình mẫu vào Program ROM
    load_sample_program();

    printf("Starting CPU simulation...\n");

    // Vòng lặp chính của CPU
    while(1) {
        uint8_t instruction = fetch_instruction(&cpu);
        if(instruction == 0xFF){
            // Lệnh dừng hoặc lỗi, có thể thêm xử lý tại đây
            break;
        }
        decode_execute(&cpu, instruction);
        // Thêm một khoảng delay nhỏ để không quá tải
        sleep_ms(1);
    }

    return 0;
}
