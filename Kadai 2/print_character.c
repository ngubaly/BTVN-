#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdint.h>

// Parameter declaration 
#define BUTTON_PIN          14
#define CHATTER_THRESHOLD   50  

// Function declaration
void initial_input_pin(unsigned int pin);
bool prevent_chatter(unsigned int pin);
void character_print_on_pin(unsigned int pin);

// Function definition
void initial_input_pin(unsigned int pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
}
/// The character is printed based on the state of button
bool prevent_chatter(unsigned int pin) {
    static int last_time = 0;
    static bool last_state;             // save the previous state of button to obser the change of state
    static bool stable_state;           // save the state of button after button is stable
    static bool first_press = true;     // initialization the first press

    int current_time = to_ms_since_boot(get_absolute_time());
    bool current_state = gpio_get(pin);

    // From the first_press, determine the initial pin state
    if(first_press){
        last_state = current_state; 
        stable_state = current_state; 
        // perform the inversion process
        first_press = false;
        return false;       // ignore the first press 
    }

    if (current_state != last_state)         // state is not maintained
     {
        last_time = current_time;
        last_state = current_state;
    }
    else if ((current_time - last_time > CHATTER_THRESHOLD) && (current_state != stable_state))  // state is maintained in a certain period while button is pressed
    {
            stable_state = current_state;
            if(!stable_state){          // button pressed 
                return true;            // valid press
            }
    }
    return false;       // no valid press      
}

void character_print(unsigned int pin){
    if(prevent_chatter(pin)){
        for(int i = 'A'; i <= 'Z'; i++){
            printf("%c ", i);
        }
    }
}
int main(){
    stdio_init_all();
    initial_input_pin(BUTTON_PIN);
    while (1)
    {   
        character_print(BUTTON_PIN);
        sleep_ms(10);
    }
    return 0;
}