dưới đây là chương trình cpu tôi đã cấu hình được: Phần load_program tôi chưa làm được (để nạp chương trình factorial): 
// #ifndef PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS

// #define PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS (-1)

// #endif



#include <stdio.h>

#include "pico/stdlib.h"

#include <stdint.h>

#include <string.h>

#include <ctype.h>

#include <stdlib.h>



// #define UART_ID         uart0

// #define BAUD_RATE       9600

// #define UART_TX_PIN     0

// #define UART_RX_PIN     1



#define ROM_SIZE        128

#define RAM_SIZE        128



#define STATUS_ZERO     0x01   // ZF specified by LSB bit

#define STATUS_OF       0x02   // OF specified by the second LSB bit 

struct cpu_state{

    uint8_t PC;

    uint8_t r0;

    uint8_t r1;

    uint8_t r2;

    uint8_t r3;

    uint8_t status;

    uint8_t program_rom[ROM_SIZE];

    uint8_t program_ram[RAM_SIZE];

};



//----------------------------------------------

// Global variables declare

struct cpu_state *cpu = NULL;

struct cpu_state cpu_instance;

uint8_t *instruction = NULL;

uint8_t instruction_1;       

bool *end_flag = NULL;

bool end_flag_value = false;

// uint8_t num_input;

//----------------------------------

// Function prototype



// CPU implementer function

void initialize_cpu();                            

void load_addition_program();                     

void fetch_instruction();   

void execute_instruction(bool *end_flag);



// Instructor function

void instr_mem_to_r0(uint8_t rx);

void instr_r0_to_mem(uint8_t rx);

void instr_transfer_reg(uint8_t rx, uint8_t ry);

void instr_immediate_set(uint8_t value);

void instr_add(uint8_t rx, uint8_t ry);

void instr_sub(uint8_t rx, uint8_t ry);

void instr_shift_left(uint8_t rx);

void instr_shift_right(uint8_t rx);

void instr_compare(uint8_t rx);

void instr_output(uint8_t rx);

void instr_input(uint8_t rx);

void instr_cond_branch(uint8_t rx, bool *end_flag);

void instr_uncond_branch(uint8_t rx, bool *end_flag);



uint8_t get_register_value(uint8_t reg);

void set_register_value(uint8_t reg, uint8_t value);

void update_zero_flag(uint8_t result);

// ------------------------------------ 

// Main function

int main() {



    stdio_usb_init();

    while(!stdio_usb_connected());

    printf("Init sucessfully!\n");

    end_flag = &end_flag_value;

    cpu = &cpu_instance;

    instruction = &instruction_1;



    initialize_cpu(); 

    load_addition_program();

    while (!(*end_flag)){



        fetch_instruction();



        printf("Instruction code: 0x%x\n", *instruction);



        execute_instruction(end_flag);

    }

    return 0;

}



void initialize_cpu(){

    cpu->PC = 0;

    cpu->r0 = 0;

    cpu->r1 = 0;

    cpu->r2 = 0;

    cpu->r3 = 0;

    cpu->status = 0;

    memset(cpu->program_rom, 0, ROM_SIZE);

    memset(cpu->program_ram, 0, RAM_SIZE);

    printf("CPU is initialized!\n");

}



void load_addition_program() {

    uint8_t addition_program[] = {

        // need to design

    };

    memcpy(cpu->program_rom, addition_program, sizeof(addition_program));

    // Fill the rest of ROM with zero

    for (int i = sizeof(addition_program); i < ROM_SIZE; i++) {

        cpu->program_rom[i] = 0;    

    }

    printf("Program is loaded!!\n");

}



void fetch_instruction() {

    // stop the program when PC is over the ROM_SIZE

    if (cpu->PC >= ROM_SIZE) {

        printf("Program Counter out of ROM bounds.\n\r");

        *instruction = 0xB0;      // instruction for stop program

    }

    *instruction = cpu->program_rom[cpu->PC];  // start from 0

    printf("Fetching is complete!\n");

}



void execute_instruction(bool *end_flag) {



    uint8_t opcode = (*instruction) >> 4;      // 4 bits MSB give the opcode

    uint8_t operand = (*instruction) & 0x0F;    // 4 bits LSB give the operand

    printf("Value of opcode is %d, value of operand is %d\n", opcode, operand);

    switch (opcode) {

        case 0x0: // set the value of r0 to the value of ram at place where is value of rx

            instr_mem_to_r0(operand & 0x03);

            break;

        case 0x2: // set the value at r0 to ram[value(rx)]

            instr_r0_to_mem(operand & 0x03);

            break;

        case 0x4: // transfer value from ry to rx

            {

                uint8_t rx = (operand & 0x0C) >> 2;

                uint8_t ry = operand & 0x03;

                instr_transfer_reg(rx, ry);

            }

            break;

        case 0x5: // set the value for rx, rx equal the value of 4 bits LSB

            instr_immediate_set(operand);

            break;

        case 0x6: // add rx and ry then store the result in rx

            {   printf("-->Instruction is addition!\n");

                uint8_t rx = (operand & 0x0C) >> 2;

                uint8_t ry = operand & 0x03;

                instr_add(rx, ry);

            }

            break;

        case 0x7: // substract rx and ry then store the result in rx

            {

                uint8_t rx = (operand & 0x0C) >> 2;

                uint8_t ry = operand & 0x03;

                instr_sub(rx, ry);

            }

            break;

        case 0x8: // shift left

            instr_shift_left(operand & 0x03);

            break;

        case 0x9: // shift right

            instr_shift_right(operand & 0x03);

            break;

        case 0xA: // compare rx with zero

            instr_compare(operand & 0x03);

            break;

        case 0xC: // output external

            printf("-->Instruction is output external!\n");

            instr_output(operand & 0x03);

            break;

        case 0xD:  // input external

            printf("-->Instruction is input external!\n");

            instr_input(operand & 0x03);

            break;

        case 0xE: // conditioanl branching

            instr_cond_branch(operand & 0x03, end_flag);

            break;

        case 0xF: // unconditioanl branching

            instr_uncond_branch(operand & 0x03, end_flag);

            break;

        default:

            printf("Undefined opcode encountered.\n\r");

            *end_flag = true;

            printf(" --> Program is completed successfully!!\n");

    }

    printf("Execute is completed!!\n");

    

}





// transfer data from RAM to r0

void instr_mem_to_r0(uint8_t rx) {

    // set the value of r0 to the value of ram at place where is value of rx

    uint8_t index_storage_data_on_RAM = get_register_value(rx);

    uint8_t value = cpu->program_ram[index_storage_data_on_RAM];

    set_register_value(0, value);

    update_zero_flag(value);

    cpu->PC += 1;

}

// transfer data from r0 to RAM

void instr_r0_to_mem(uint8_t rx) {

    uint8_t index_storage_data_on_RAM = get_register_value(rx);   

    uint8_t value_r0 = get_register_value(0);

    cpu->program_ram[index_storage_data_on_RAM] = value_r0;

    update_zero_flag(value_r0);

    cpu->PC += 1;

}

// transfer data between 2 registers

void instr_transfer_reg(uint8_t rx, uint8_t ry) {

    uint8_t value_rx = get_register_value(rx);

    set_register_value(ry, value_rx);

    update_zero_flag(value_rx);

    cpu->PC += 1;

}

// set immediate value into r0

void instr_immediate_set(uint8_t value) {

    set_register_value(0, value);

    update_zero_flag(value);

    cpu->PC += 1;

}

// Addition

void instr_add(uint8_t rx, uint8_t ry) {

    uint8_t value_rx = get_register_value(rx);

    uint8_t value_ry = get_register_value(ry);

    uint16_t result = value_rx + value_ry;

    set_register_value(rx, result & 0xFF);

    update_zero_flag(result & 0xFF);

    // update Overflow flag:

    if(result > 0xFF){

        cpu->status |= STATUS_OF;   // on the OF's bit

    }

    else{

        cpu->status &= ~STATUS_OF;  // off the OF's bit

    }

    cpu->PC += 1;

}

// Substraction 

void instr_sub(uint8_t rx, uint8_t ry) {

    uint8_t value_rx = get_register_value(rx);

    uint8_t value_ry = get_register_value(ry);

    int16_t result = value_rx - value_ry;

    set_register_value(rx, result & 0xFF);

    printf("The result of addition is stoaraged in the register %d: \n", rx);

    update_zero_flag(result & 0xFF);

    // update Overflow flag

    if(result < 0){

        cpu->status |= STATUS_OF;   // on the OF's bit

    }

    else{

        cpu->status &= ~STATUS_OF;  // off the OF's bit

    }

    cpu->PC += 1;

}

// shift left

void instr_shift_left(uint8_t rx) {

    uint8_t value_rx = get_register_value(rx);

    value_rx <<= 4;

    set_register_value(rx, value_rx);

    // no status update

    cpu->PC += 1;

}

// shift right

void instr_shift_right(uint8_t rx) {

    uint8_t value_rx = get_register_value(rx);

    value_rx >>= 4;

    set_register_value(rx, value_rx);

    // no status update

    cpu->PC += 1;

}

// compare with zero

void instr_compare(uint8_t rx) {

    uint8_t value_rx = get_register_value( rx);

    if (value_rx == 0) {

        cpu->status |= STATUS_ZERO;

    } else {

        cpu->status &= ~STATUS_ZERO;

    }

    cpu->PC += 1;

}

// external output

void instr_output(uint8_t rx) {

    uint8_t value_rx = get_register_value(rx);

    printf("The result is storage in register %d is: %d\n", rx, value_rx);

    cpu->PC += 1;

}

// external input

void instr_input(uint8_t rx) {

    printf("Enter a number (0-255) (this number is storage in register %d):\n ", rx);

    char input_buffer[4];

    int idx = 0;

    while (idx < 3){

        int c =  getchar();

        putchar(c);

        input_buffer[idx++] = c;

        }

    input_buffer[idx] = '\0';

    uint8_t value = (uint8_t)atoi(input_buffer);  // typecast to uint_8 

    set_register_value(rx, value);

    update_zero_flag(value);

    printf("\r\n");

    cpu->PC += 1;

    

}

// condition branch (if ZF set then PC -> PC[rx])

void instr_cond_branch(uint8_t rx, bool *end_flag) {

    if (cpu->status & STATUS_ZERO) {

        uint8_t index_branching = get_register_value(rx);

        if(index_branching < ROM_SIZE){

            cpu->PC = index_branching;

        }

        else {

            printf("Branch location out of bounds.\\n\r");

            *end_flag = true;

        }

    }

    else{

        cpu->PC +=1;

    }

}



// unconditional branch (PC -> PC[rx])

void instr_uncond_branch(uint8_t rx, bool *end_flag) {

    uint8_t index_branching = get_register_value(rx);

    if(index_branching < ROM_SIZE){

        cpu->PC = index_branching;

    }

    else{

        printf("Branch location out of bounds.\\n\r");

        *end_flag = true;

    }

}



uint8_t get_register_value(uint8_t reg) {

    switch (reg) {

        case 0: return cpu->r0;

        case 1: return cpu->r1;

        case 2: return cpu->r2;

        case 3: return cpu->r3;

        default: return 0;

    }

}



void set_register_value(uint8_t reg, uint8_t value) {

    switch (reg) {

        case 0: cpu->r0 = value; break;

        case 1: cpu->r1 = value; break;

        case 2: cpu->r2 = value; break;

        case 3: cpu->r3 = value; break;

    }

}



void update_zero_flag(uint8_t result) {

    if (result == 0) {

        cpu->status |= STATUS_ZERO;

    } else {

        cpu->status &= ~STATUS_ZERO;

    }

}

Đối với phần load_program, tôi thiết kế như sau: 
Below is the C program for factorial calculations:

int factorial(int N)

{

    // variable to store the final factorial

    int ans = N;

    // Outer loop

    for (int i = N - 1; i > 0; i--) {

        int sum = 0;

        // Inner loop

        for (int j = i; j >0; j--)

            sum += ans;

        ans = sum;

    }

    return ans;

}

Step 1: Initialization: r2 = N, r3 = N-1

-	Initialize r2 = N: Input external to r2: r2  N

-	Initialize r3 = N-1: 

+ Transfer data from r2 to r3: r3  r2  (Now the value of r3 is N)

+ Set the immediate value for r0 equal 1: r0  1 (Now the value of r0 is 1)

+ Substract r0 by r3: r3  r3 – r0  (Now the value of r3 is N-1)

Step 2: Outer loop: the counter for outer loop will be specified by the value of r3 (this counter will run from r3 = N-1 till r3 = 1)

-	Compare r3 with 0 for checking the ZF status: cmp(r3, 0)   (1) (this location has PC value for start outer loop)

-	Set the immediate value for r0 (this value assign the end of outer loop value for PC if ZF is set on): r0  end_outer_loop_PC_value  (at (4))

-	Jump the PC to PC[end_outer_loop_PC_value] if r3 == 0 (ZF is set)

Step 3: Inner loop: the counter for inner loop will be specified by the value of r1 (this counter will run from r1 = r3 till r1 = 1)

-	Transfer data from r3 to r1: r1  r3

-	Compare r1 with 0 for checking the ZF status: cmp(r1,0)  (2) (this location has PC value for start inner loop)

-	Set the immediate value for r0 (this value assign the end of inner loop value for PC if ZF is set on): r0  end_inner_loop_PC_value  (at (3))

-	Jump the PC to PC[end_inner_loop_PC_value] if r1 == 0 (ZF is set)

-	Accumulation for r0:

+ Set the value r0 by the value store in RAM[r3]: r0  RAM[r3]

+ Accumulate r0 by r2: r0 += r2

+ Store the value r0 into RAM[r3] by transfer data from r0 to RAM[r3]: RAM[r3]r0

-	Decrement counter r1:

+ Set the immediate value for r0 by 1: r0  1

+ Substract r0 by r1: r1  r1 – r0 

-	Jump again to start inner loop:

+ Set the immediate r0 to the PC value at (2)

+ Jump with unconditional branch to PC[r0]

Step 4: The rest of outer loop

-	Get the accumulated value at RAM[r3] to r2, by transfer data from RAM[r3] to r0 then transfer from r0 to r2

+ r0  RAM[r3]   (3)  (this location is PC value for end inner loop)

+ r2  r0 

-	Decrementing the counter of outer loop:

+ Set the immediate value r0 to 1: r0  1

+ Substract r0 by r3: r3  r3 – r0

-	Jump again to location start outer loop:

+ Set the immediate r0 to the PC value at (1)

+ Jump with unconditional branch to PC[r0]

Step 5: Output external the result:

-	Output external the result stored in r2: r2  output  (4) (this location is PC value for end outer loop)

-	Instruction code for exist: 0xB0



  Phần này sẽ khó ở chỗ: bởi vì câu lệnh set immediate cho r0 chỉ là 4 bits LSB, tức là có giá trị không vượt quá 15, vì vậy nên khi sử dụng các lệnh jump, chúng ta không nhảy được đến những vị trí PC_value > 15, hãy giúp tôi 
