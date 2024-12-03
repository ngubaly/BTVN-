#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include <stdint.h>
#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define GPIO_PIN_1 16
#define GPIO_PIN_2 17
#define GPIO_PIN_3 18

// Function declaration 
void initial_pin(unsigned pin, bool pin_mode);


// Function definition 
void initial_pin(unsigned pin, bool pin_mode){
    gpio_init(pin);
    gpio_set_dir(pin, pin_mode);
}


// This program will excute with the condition below:
// From serial monitor, let enter one character "x" to derive led
/// x == '1': One specified LED will turns on 
/// x == '2': Two specified LEDs will turn on 
/// x == '1': All of specified LEDs will turn off 

int main() {
    stdio_init_all();
    // initialize variable to store the received character
    char received = 0;

    // Set up for GPIO LED output
    initial_pin(GPIO_PIN_1, GPIO_OUT);
    initial_pin(GPIO_PIN_2, GPIO_OUT);
    initial_pin(GPIO_PIN_3, GPIO_OUT);

    // Set up UART
    // Init UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);

    // Specify GPIO pins as UART pins
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, 0));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, 1));

    while (1) {
        // Wait for input data on UART RX of Pico from Computer
        bool readable = uart_is_readable(UART_ID);
        if (readable) {
            received = uart_getc(UART_ID);
            // case of control LED
            switch (received)
            {
            case '1':   // Turn ON LED_1 and the rest are OFF !
                gpio_put(GPIO_PIN_1, 1);
                gpio_put(GPIO_PIN_2, 0);
                gpio_put(GPIO_PIN_3, 0);        
                break;
            case '2': // Turn ON LED_2 and the rest are OFF !
                gpio_put(GPIO_PIN_1, 0);
                gpio_put(GPIO_PIN_2, 1);
                gpio_put(GPIO_PIN_3, 0);
                break;
            case '3': // Turn ON LED_3 and the rest are OFF !
                gpio_put(GPIO_PIN_1, 0);
                gpio_put(GPIO_PIN_2, 0);
                gpio_put(GPIO_PIN_3, 1);
                break;     
            default: // turn off all LED with another character received
                gpio_put(GPIO_PIN_1, 0);
                gpio_put(GPIO_PIN_2, 0);
                gpio_put(GPIO_PIN_3, 0);                   
                break;
            }
        }
        sleep_ms(10);
    }
    return 0;
}