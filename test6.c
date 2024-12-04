/*
2. Logic Chương Trình Tính Giai Thừa
Để tính giai thừa của một số N, chúng ta cần thực hiện các bước sau:

Nhập N từ UART vào thanh ghi r1.
Khởi tạo r2 (accumulator) bằng 1.
Khởi tạo r3 (counter) bằng giá trị của r1 (N).
Vòng lặp tính giai thừa:
So sánh r3 với 0.
Nếu r3 == 0, kết thúc vòng lặp.
Nhân r2 với r3 bằng cách sử dụng phép cộng lặp lại:
Đặt r0 = r2.
Đặt r2 = 0.
Đặt r4 = r3 (sử dụng r1 làm bộ đếm trong vòng lặp nội bộ).
Vòng lặp nội bộ:
So sánh r1 với 0.
Nếu r1 == 0, thoát vòng lặp nội bộ.
Cộng r0 vào r2.
Trừ 1 từ r1.
Lặp lại vòng lặp nội bộ.
Trừ 1 từ r3.
Lặp lại vòng lặp tính giai thừa.
Xuất kết quả từ r2 ra UART.
Kết thúc chương trình bằng một vòng lặp vô hạn.
Địa chỉ 0: 0xD1 (1101 0001) – Nhập vào r1 từ UART

Chức năng: Nhập một số từ UART và lưu vào thanh ghi r1.
Cập nhật STATUS: Không.
Địa chỉ 1: 0x51 (0101 0001) – Đặt r0 = 1

Chức năng: Đặt giá trị 1 vào thanh ghi r0.
Cập nhật STATUS: Zero nếu giá trị là 0 (không trong trường hợp này).
Địa chỉ 2: 0x42 (0100 0010) – Chuyển r0 vào r2

Chức năng: r2 = r0 = 1.
Cập nhật STATUS: Zero nếu r2 = 0 (không trong trường hợp này).
Địa chỉ 3: 0x43 (0100 0011) – Chuyển r1 vào r3

Chức năng: r3 = r1 = N (số cần tính giai thừa).
Cập nhật STATUS: Zero nếu r3 = 0.
Địa chỉ 4: 0xA0 (1010 0000) – So sánh r3 == 0

Chức năng: So sánh r3 với 0.
Cập nhật STATUS: Zero nếu r3 == 0.
Địa chỉ 5: 0x5C (0101 1100) – Đặt r0 = 12

Chức năng: Đặt giá trị 12 vào r0. Địa chỉ 12 là địa chỉ "End" của chương trình.
Cập nhật STATUS: Zero nếu giá trị là 0 (không trong trường hợp này).
Địa chỉ 6: 0xE0 (1110 0000) – Conditional Branch tới r0 nếu Zero

Chức năng: Nếu cờ Zero được thiết lập (tức là r3 == 0), nhảy đến địa chỉ trong r0 = 12.
Cập nhật STATUS: Không.
Hiệu ứng: Nếu r3 == 0, PC sẽ được đặt thành 12, chuyển sang bước "End". Nếu không, chương trình tiếp tục.
Địa chỉ 7: 0x20 (0010 0000) – r0 ⇒ Address indicated by rx (chưa dùng)

Chức năng: Lệnh này không thực sự cần thiết trong chương trình tính giai thừa hiện tại. Có thể bỏ qua hoặc sử dụng cho mục đích khác.
Cập nhật STATUS: Zero nếu giá trị ghi là 0.
Địa chỉ 8: 0x50 (0101 0000) – Đặt r0 = 0

Chức năng: Đặt giá trị 0 vào r0.
Cập nhật STATUS: Zero nếu giá trị là 0 (có trong trường hợp này).
Địa chỉ 9: 0x41 (0100 0001) – Chuyển r3 vào r1

Chức năng: r1 = r3 (đặt lại bộ đếm trong vòng lặp nội bộ).
Cập nhật STATUS: Zero nếu r1 = 0.
Địa chỉ 10: 0xA1 (1010 0001) – So sánh r1 == 0

Chức năng: So sánh r1 với 0.
Cập nhật STATUS: Zero nếu r1 == 0.
Địa chỉ 11: 0x5C (0101 1100) – Đặt r0 = 12

Chức năng: Đặt giá trị 12 vào r0.
Cập nhật STATUS: Zero nếu giá trị là 0 (không trong trường hợp này).
Địa chỉ 12: 0xE1 (1110 0001) – Conditional Branch tới r1 nếu Zero

Chức năng: Nếu cờ Zero được thiết lập (r1 == 0), nhảy đến địa chỉ trong r0 = 12 (End).
Cập nhật STATUS: Không.
Địa chỉ 13: 0x62 (0110 0010) – Add (r0 + r2 → r0)

Chức năng: r0 = r0 + r2 (tính tổng để nhân).
Cập nhật STATUS: Zero nếu r0 = 0, Overflow nếu tràn số.
Địa chỉ 14: 0x42 (0100 0010) – Chuyển r0 vào r2

Chức năng: r2 = r0 (cập nhật kết quả giai thừa).
Cập nhật STATUS: Zero nếu r2 = 0.
Địa chỉ 15: 0x51 (0101 0001) – Đặt r0 = 1

Chức năng: Đặt giá trị 1 vào r0.
Cập nhật STATUS: Zero nếu giá trị là 0 (không trong trường hợp này).
Địa chỉ 16: 0x71 (0111 0001) – Subtract (r1 - r0 → r1)

Chức năng: r1 = r1 - r0 (trừ 1 từ bộ đếm).
Cập nhật STATUS: Zero nếu r1 = 0, Overflow nếu tràn số.
Địa chỉ 17: 0xF1 (1111 0001) – Unconditional Branch tới r1

Chức năng: Nhảy không điều kiện tới địa chỉ trong r1.
Cập nhật STATUS: Không.
Địa chỉ 18: 0xC2 (1100 0010) – Output từ r2

Chức năng: Xuất giá trị của r2 ra UART.
Cập nhật STATUS: Không.
Địa chỉ 19: 0x5C (0101 1100) – Đặt r0 = 12

Chức năng: Đặt giá trị 12 vào r0.
Cập nhật STATUS: Zero nếu giá trị là 0 (không trong trường hợp này).
Địa chỉ 20: 0xF2 (1111 0010) – Unconditional Branch tới r2

Chức năng: Nhảy không điều kiện tới địa chỉ trong r2 (12), tạo vòng lặp vô hạn để kết thúc chương trình.
Cập nhật STATUS: Không.

Dưới đây là phân tích chi tiết cách chương trình tính giai thừa hoạt động:

Nhập N vào r1 (Địa chỉ 0: 0xD1):

Hành động: Người dùng nhập một số từ UART và lưu vào r1.
Kết quả: r1 = N.
Đặt r0 = 1 (Địa chỉ 1: 0x51):

Hành động: Đặt giá trị 1 vào r0.
Kết quả: r0 = 1.
Chuyển r0 vào r2 (Địa chỉ 2: 0x42):

Hành động: r2 = r0 = 1.
Kết quả: r2 = 1 (Khởi tạo accumulator cho giai thừa).
Chuyển r1 vào r3 (Địa chỉ 3: 0x43):

Hành động: r3 = r1 = N.
Kết quả: r3 = N (Bộ đếm cho vòng lặp tính giai thừa).
So sánh r3 == 0 (Địa chỉ 4: 0xA0):

Hành động: So sánh r3 với 0.
Kết quả: Nếu r3 == 0, cờ Zero được thiết lập.
Đặt r0 = 12 (Địa chỉ 5: 0x5C):

Hành động: Đặt giá trị 12 vào r0, đây là địa chỉ "End" của chương trình.
Kết quả: r0 = 12.
Conditional Branch tới r0 nếu Zero (Địa chỉ 6: 0xE0):

Hành động: Nếu cờ Zero được thiết lập (r3 == 0), nhảy tới địa chỉ r0 = 12 (End).
Kết quả:
Nếu r3 == 0: PC = 12.
Nếu r3 != 0: Tiếp tục chương trình.
Chuyển r2 vào r0 (Địa chỉ 7: 0x20):

Hành động: r0 = r2 = 1.
Kết quả: r0 = 1.
Đặt r0 = 0 (Địa chỉ 8: 0x50):

Hành động: Đặt giá trị 0 vào r0.
Kết quả: r0 = 0.
Chuyển r3 vào r1 (Địa chỉ 9: 0x41):

Hành động: r1 = r3 = N.
Kết quả: r1 = N (Bộ đếm cho vòng lặp nội bộ).
So sánh r1 == 0 (Địa chỉ 10: 0xA1):

Hành động: So sánh r1 với 0.
Kết quả: Nếu r1 == 0, cờ Zero được thiết lập.
Đặt r0 = 12 (Địa chỉ 11: 0x5C):

Hành động: Đặt giá trị 12 vào r0.
Kết quả: r0 = 12.
Conditional Branch tới r1 nếu Zero (Địa chỉ 12: 0xE1):

Hành động: Nếu cờ Zero được thiết lập (r1 == 0), nhảy tới địa chỉ r0 = 12 (End).
Kết quả:
Nếu r1 == 0: PC = 12.
Nếu r1 != 0: Tiếp tục chương trình.
Add (r0 + r2 → r0) (Địa chỉ 13: 0x62):

Hành động: r0 = r0 + r2 = 0 + 1 = 1.
Kết quả: r0 = 1.
Cập nhật STATUS: Nếu r0 > 255, cờ Overflow được thiết lập (không xảy ra trong trường hợp này).
Chuyển r0 vào r2 (Địa chỉ 14: 0x42):

Hành động: r2 = r0 = 1.
Kết quả: r2 = 1.
Đặt r0 = 1 (Địa chỉ 15: 0x51):

Hành động: Đặt giá trị 1 vào r0.
Kết quả: r0 = 1.
Subtract (r1 - r0 → r1) (Địa chỉ 16: 0x71):

Hành động: r1 = r1 - r0 = N - 1.
Kết quả: r1 = N - 1.
Cập nhật STATUS: Zero nếu r1 = 0, Overflow nếu tràn số.
Unconditional Branch tới r1 (Địa chỉ 17: 0xF1):

Hành động: Nhảy không điều kiện tới địa chỉ trong r1 (giá trị N - 1).
Kết quả: PC = giá trị trong r1 (N - 1).
End - Output từ r2 (Địa chỉ 18: 0xC2):

Hành động: Xuất giá trị của r2 ra UART (kết quả giai thừa).
Cập nhật STATUS: Không.
Đặt r0 = 12 (Địa chỉ 19: 0x5C):

Hành động: Đặt giá trị 12 vào r0.
Kết quả: r0 = 12.
Unconditional Branch tới r2 (Địa chỉ 20: 0xF2):

Hành động: Nhảy không điều kiện tới địa chỉ trong r2 = 12 (End).
Kết quả: PC = 12 (tạo vòng lặp vô hạn).
6. Sơ Đồ Vòng Lặp Tính Giai Thừa
Để dễ hiểu hơn, đây là sơ đồ vòng lặp của chương trình:

Khởi Tạo:

r2 = 1 (accumulator)
r3 = N (counter)
Vòng Lặp Chính:

So sánh r3 == 0:
Nếu đúng, nhảy tới "End".
Nhân r2 với r3:
r0 = r2
r2 = 0
r1 = r3
Vòng lặp nội bộ:
So sánh r1 == 0:
Nếu đúng, thoát vòng lặp nội bộ.
Cộng r0 vào r2:
r0 = r0 + r2
r2 = r0
Trừ 1 từ r1:
r1 = r1 - 1
Lặp lại vòng lặp nội bộ.
Trừ 1 từ r3:
r3 = r3 - 1
Lặp lại vòng lặp chính.
End:

Xuất kết quả r2 ra UART.
Tạo vòng lặp vô hạn để kết thúc chương trình.


Nhập N vào r1 (Địa chỉ 0: 0xD1):

Mã lệnh: 0xD1
Chức năng: Yêu cầu người dùng nhập một số từ UART và lưu vào thanh ghi r1.
Kết quả: r1 = N (số cần tính giai thừa).
Đặt r0 = 1 (Địa chỉ 1: 0x51):

Mã lệnh: 0x51
Chức năng: Đặt giá trị 1 vào thanh ghi r0.
Kết quả: r0 = 1.
Chuyển r0 vào r2 (Địa chỉ 2: 0x42):

Mã lệnh: 0x42
Chức năng: r2 = r0 = 1.
Kết quả: r2 = 1 (Khởi tạo accumulator cho giai thừa).
Chuyển r1 vào r3 (Địa chỉ 3: 0x43):

Mã lệnh: 0x43
Chức năng: r3 = r1 = N.
Kết quả: r3 = N (Bộ đếm cho vòng lặp tính giai thừa).
So sánh r3 == 0 (Địa chỉ 4: 0xA0):

Mã lệnh: 0xA0
Chức năng: So sánh r3 với 0.
Kết quả: Nếu r3 == 0, cờ Zero được thiết lập.
Đặt r0 = 12 (Địa chỉ 5: 0x5C):

Mã lệnh: 0x5C
Chức năng: Đặt giá trị 12 vào r0. Đây là địa chỉ "End" của chương trình.
Kết quả: r0 = 12.
Conditional Branch tới r0 nếu Zero (Địa chỉ 6: 0xE0):

Mã lệnh: 0xE0
Chức năng: Nếu cờ Zero được thiết lập (r3 == 0), nhảy tới địa chỉ r0 = 12 (End).
Kết quả:
Nếu r3 == 0: PC = 12.
Nếu r3 != 0: Tiếp tục chương trình.
Chuyển r2 vào r0 (Địa chỉ 7: 0x20):

Mã lệnh: 0x20
Chức năng: Chuyển giá trị của r2 vào địa chỉ được chỉ định bởi r0. Trong chương trình này, lệnh này không thực sự cần thiết và có thể được bỏ qua hoặc sử dụng cho mục đích khác.
Kết quả: Không thay đổi trong chương trình hiện tại.
Đặt r0 = 0 (Địa chỉ 8: 0x50):

Mã lệnh: 0x50
Chức năng: Đặt giá trị 0 vào r0.
Kết quả: r0 = 0.
Chuyển r3 vào r1 (Địa chỉ 9: 0x41):

Mã lệnh: 0x41
Chức năng: r1 = r3 = N.
Kết quả: r1 = N (Đặt lại bộ đếm cho vòng lặp nội bộ).
So sánh r1 == 0 (Địa chỉ 10: 0xA1):

Mã lệnh: 0xA1
Chức năng: So sánh r1 với 0.
Kết quả: Nếu r1 == 0, cờ Zero được thiết lập.
Đặt r0 = 12 (Địa chỉ 11: 0x5C):

Mã lệnh: 0x5C
Chức năng: Đặt giá trị 12 vào r0.
Kết quả: r0 = 12.
Conditional Branch tới r1 nếu Zero (Địa chỉ 12: 0xE1):

Mã lệnh: 0xE1
Chức năng: Nếu cờ Zero được thiết lập (r1 == 0), nhảy tới địa chỉ r0 = 12 (End).
Kết quả:
Nếu r1 == 0: PC = 12 (End).
Nếu r1 != 0: Tiếp tục chương trình.
Add (r0 + r2 → r0) (Địa chỉ 13: 0x62):

Mã lệnh: 0x62
Chức năng: r0 = r0 + r2 = 0 + 1 = 1.
Kết quả: r0 = 1.
Cập nhật STATUS: Zero nếu r0 == 0, Overflow nếu tràn số (không xảy ra trong trường hợp này).
Chuyển r0 vào r2 (Địa chỉ 14: 0x42):

Mã lệnh: 0x42
Chức năng: r2 = r0 = 1.
Kết quả: r2 = 1.
Đặt r0 = 1 (Địa chỉ 15: 0x51):

Mã lệnh: 0x51
Chức năng: Đặt giá trị 1 vào r0.
Kết quả: r0 = 1.
Subtract (r1 - r0 → r1) (Địa chỉ 16: 0x71):

Mã lệnh: 0x71
Chức năng: r1 = r1 - r0 = N - 1.
Kết quả: r1 = N - 1.
Cập nhật STATUS: Zero nếu r1 == 0, Overflow nếu tràn số.
Unconditional Branch tới r1 (Địa chỉ 17: 0xF1):

Mã lệnh: 0xF1
Chức năng: Nhảy không điều kiện tới địa chỉ trong r1.
Kết quả: PC = giá trị trong r1 (N - 1).
End - Output từ r2 (Địa chỉ 18: 0xC2):

Mã lệnh: 0xC2
Chức năng: Xuất giá trị của r2 ra UART (kết quả giai thừa).
Kết quả: Hiển thị giá trị giai thừa trên UART.
Đặt r0 = 12 (Địa chỉ 19: 0x5C):

Mã lệnh: 0x5C
Chức năng: Đặt giá trị 12 vào r0.
Kết quả: r0 = 12.
Unconditional Branch tới r2 (Địa chỉ 20: 0xF2):

Mã lệnh: 0xF2
Chức năng: Nhảy không điều kiện tới địa chỉ trong r2 = 12 (End).
Kết quả: PC = 12 (End), tạo vòng lặp vô hạn để kết thúc chương trình.

Hãy xem xét quy trình tính giai thừa của số N:

Khởi Tạo:

r2 = 1 (accumulator).
r3 = N (counter).
Vòng Lặp Chính (Bắt đầu tại địa chỉ 4):

So sánh r3 == 0:
Nếu r3 == 0, nhảy tới "End" (địa chỉ 12).
Nếu không, tiếp tục.
Nhân r2 với r3:

Đặt r0 = r2: Chuẩn bị để cộng lặp lại.
Đặt r2 = 0: Reset accumulator cho kết quả nhân.
Đặt r1 = r3: Sử dụng r1 làm bộ đếm cho vòng lặp nội bộ.
Vòng Lặp Nội Bộ (Bắt đầu tại địa chỉ 10):
So sánh r1 == 0:
Nếu r1 == 0, nhảy tới sau vòng lặp nội bộ.
Nếu không, tiếp tục.
Cộng r0 vào r2: Tích lũy kết quả nhân.
Trừ 1 từ r1: Giảm bộ đếm.
Nhảy lại vào vòng lặp nội bộ.
Kết thúc Vòng Lặp Nội Bộ:
r2 đã chứa kết quả r2 * r3.
Trừ 1 từ r3:

Trừ r0 = 1 từ r3: Giảm bộ đếm chính.
Lặp Lại Vòng Lặp Chính:

Quay lại bước 2 với r3 đã giảm.
Kết Thúc:

Khi r3 == 0, xuất kết quả từ r2 ra UART.
Tạo vòng lặp vô hạn để kết thúc chương trình.
10. Lưu Ý Khi Triển Khai
Địa Chỉ Lệnh:

Đảm bảo rằng địa chỉ các lệnh được đặt đúng. Trong chương trình này, địa chỉ 12 và 20 là địa chỉ "End".
Giới Hạn Số:

Vì chúng ta sử dụng thanh ghi 8-bit, giá trị N phải nhỏ hơn hoặc bằng 8 để tránh tràn số trong phép nhân (vì 8-bit có thể lưu trữ tối đa 255).
Cờ Overflow (OF):

Trong các phép tính cộng và trừ, nếu có tràn số, cờ OF được thiết lập. Bạn có thể thêm các kiểm tra bổ sung để xử lý trường hợp tràn số nếu cần.
Chức Năng Chưa Sử Dụng:

Một số lệnh như 0x20 (0010 0000) không được sử dụng trong chương trình này. Bạn có thể bỏ qua hoặc sử dụng cho các mục đích khác trong tương lai.
Chức Năng Shift Left và Shift Right:

Trong chương trình tính giai thừa hiện tại, chúng ta không sử dụng các lệnh dịch trái và dịch phải. Tuy nhiên, bạn có thể mở rộng chương trình để sử dụng các lệnh này nếu cần thiết.
*/




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
    // 4. Vòng lặp:
    //    a. So sánh r3 == 0
    //       - Nếu đúng, nhảy đến end
    //    b. Multiply r2 = r2 * r3 bằng cách sử dụng phép cộng
    //       i. Đặt r0 = r2
    //       ii. Đặt r2 = 0
    //       iii. Đặt r1 = r3 (sử dụng r1 làm inner loop counter)
    //       iv. Inner Loop:
    //           - So sánh r1 == 0
    //               + Nếu đúng, nhảy đến sau inner loop
    //           - Cộng r0 vào r2
    //           - Trừ 1 từ r1
    //           - Nhảy lại vào inner loop
    //    c. Đặt r3 = r3 - 1
    //    d. Nhảy lại vào vòng lặp
    // 5. End:
    //    a. Xuất r2 (kết quả) ra UART
    //    b. Nhảy vô hạn để kết thúc chương trình

    // Mã lệnh đã mã hóa:
    uint8_t factorial_program[] = {
        // 1. Địa chỉ 0: Input N vào r1
        0xD1, // 1101 0001: Nhập vào r1

        // 2. Địa chỉ 1: Đặt r0 = 1
        0x51, // 0101 0001: Đặt r0 = 1

        // 3. Địa chỉ 2: Chuyển r0 vào r2
        0x42, // 0100 0010: Chuyển r0 vào r2

        // 4. Địa chỉ 3: Chuyển r1 vào r3
        0x43, // 0100 0011: Chuyển r1 vào r3

        // 5. Địa chỉ 4: So sánh r3 == 0
        0xA0, // 1010 0000: So sánh r3 == 0

        // 6. Địa chỉ 5: Đặt r0 = 12 (địa chỉ end)
        0x5C, // 0101 1100: Đặt r0 = 12

        // 7. Địa chỉ 6: Conditional Branch tới r0 nếu Zero
        0xE0, // 1110 0000: Conditional Branch tới r0 nếu Zero

        // 8. Địa chỉ 7: Chuyển r2 vào r0
        0x20, // 0010 0000: Chuyển r0 vào địa chỉ trong r2 (không sử dụng trong chương trình này)

        // 9. Địa chỉ 8: Đặt r0 = 0
        0x50, // 0101 0000: Đặt r0 = 0

        // 10. Địa chỉ 9: Chuyển r3 vào r1
        0x41, // 0100 0001: Chuyển r3 vào r1

        // 11. Địa chỉ 10: So sánh r1 == 0
        0xA1, // 1010 0001: So sánh r1 == 0

        // 12. Địa chỉ 11: Đặt r0 = 12 (địa chỉ end)
        0x5C, // 0101 1100: Đặt r0 = 12

        // 13. Địa chỉ 12: Conditional Branch tới r1 nếu Zero
        0xE1, // 1110 0001: Conditional Branch tới r1 nếu Zero

        // 14. Địa chỉ 13: Add (r0 + r2 → r0)
        0x62, // 0110 0010: Add r0 + r2 → r0

        // 15. Địa chỉ 14: Chuyển r0 vào r2
        0x42, // 0100 0010: Chuyển r0 vào r2

        // 16. Địa chỉ 15: Đặt r0 = 1
        0x51, // 0101 0001: Đặt r0 = 1

        // 17. Địa chỉ 16: Subtract (r1 - r0 → r1)
        0x71, // 0111 0001: Subtract r0 từ r1 → r1

        // 18. Địa chỉ 17: Unconditional Branch tới r1
        0xF1, // 1111 0001: Unconditional Branch tới r1

        // 19. Địa chỉ 18: Output từ r2
        0xC2, // 1100 0010: Output từ r2

        // 20. Địa chỉ 19: Đặt r0 = 12 (địa chỉ end)
        0x5C, // 0101 1100: Đặt r0 = 12

        // 21. Địa chỉ 20: Unconditional Branch tới r2 (vòng lặp vô hạn)
        0xF2  // 1111 0010: Unconditional Branch tới r2 (12)
    };

    // Load the factorial program into program_rom
    memcpy(cpu->program_rom, factorial_program, sizeof(factorial_program));

    // Fill the rest of ROM with NOPs hoặc các giá trị không sử dụng (tuỳ chọn)
    for (int i = sizeof(factorial_program); i < ROM_SIZE; i++) {
        cpu->program_rom[i] = 0x00; // Giả sử 0x00 là NOP hoặc không sử dụng
    }

    uart_puts(UART_ID, "Factorial program loaded into ROM.\n\r");
}

// Fetch Instruction from ROM
void fetch_instruction(CPU_State *cpu, uint8_t *instruction) {
    if (cpu->PC >= ROM_SIZE) {
        uart_puts(UART_ID, "Program Counter out of ROM bounds.\n\r");
        *instruction = 0xFF; // Undefined opcode để dừng chương trình
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
            // 0110 xxyy: Add (r0 + r2 → r0)
            {
                uint8_t rx = (operand & 0xC) >> 2;
                uint8_t ry = operand & 0x3;
                instr_add(cpu, rx, ry);
            }
            break;
        case 0x7:
            // 0111 xxyy: Subtract (r1 - r0 → r1)
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
            // 1110 ddxx: Conditional Branch (nếu Zero = 1, PC ← giá trị trong rx)
            {
                uint8_t rx = operand & 0x3;
                instr_cond_branch(cpu, rx, end_flag);
            }
            break;
        case 0xF:
            // 1111 ddxx: Unconditional Branch (PC ← giá trị trong rx)
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

// 5. 0110 xxyy: Add (r0 + r2 → r0)
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

// 6. 0111 xxyy: Subtract (r1 - r0 → r1)
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
    char buffer[6]; // Đủ để chứa "255\r\n"
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
    while (idx < 3) { // Max 3 digits cho 0-255
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            if (c == '\n' || c == '\r') {
                break;
            }
            if (isdigit(c)) {
                uart_putc(UART_ID, c); // Echo ký tự
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

// 12. 1110 ddxx: Conditional Branch (nếu Zero = 1, PC ← giá trị tron
