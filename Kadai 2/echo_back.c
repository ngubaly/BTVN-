#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include<stdint.h>

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1


// Function declaration 
void initial_pin(unsigned int pin, bool pin_mode);
// Function definition 
void initial_pin(unsigned int pin, bool pin_mode){
    gpio_init(pin);
    gpio_set_dir(pin, pin_mode);
}
int main() {
    stdio_init_all();
    char received = 0;
    // Set up UART
    // Init UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);
    // Specify GPIO pins as UART pins
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, 0));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, 1));
    while (1) {
        bool readable = uart_is_readable(UART_ID);
        // Wait for input data on UART RX of Pico from Computer
        if (readable) {
            received = uart_getc(UART_ID);
            uart_putc_raw(UART_ID, received); // write the received character to UART for transmission
        }
        sleep_ms(10);
    }
    return 0;
}