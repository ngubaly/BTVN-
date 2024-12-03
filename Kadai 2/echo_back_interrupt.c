#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include <stdint.h>
#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

// Global variable initilization
static char received = 0;

// Function declaration 
void initial_pin(unsigned pin, bool pin_mode);
void uart_rx_handler(void);

// Function definition 
void initial_pin(unsigned pin, bool pin_mode){
    gpio_init(pin);
    gpio_set_dir(pin, pin_mode);
}
// Interrupt handler for UART RX
// If the interrupt event is fired, pico reads and echoes the received character on UART connection
void uart_rx_handler(){
    bool readable = uart_is_readable(UART_ID);         
    while (readable)
    {
        received = uart_getc(UART_ID);
        uart_putc_raw(UART_ID, received);  // write the received character to UART for transmission
    }
}
int main() {
    stdio_init_all();
    // Set up UART
    // Init UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);
    // Specify GPIO pins as UART pins
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, 0));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, 1));

    // Install the interrupt handler
    // Set the consider handler becom the exclusive handler on the executing core
    irq_set_exclusive_handler(20, uart_rx_handler);
    // Enable the UART interrupt on the executing core
    irq_set_enabled(20, true);
    // Enable UART interrupt for RX
    uart_set_irqs_enabled(UART_ID, true, false);
    while (1) {
        sleep_ms(10);
    }
    return 0;
}