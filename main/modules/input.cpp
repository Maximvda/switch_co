#include "input.h"

#include <string>

#include "config.h"
#include "esp_log.h"

#define PRESS_DELAY 0.4*1000*1000
#define HOLD_DELAY PRESS_DELAY*3
#define PERIODIC_DELAY 0.01*1000*1000

const static char* TAG = {"input"};

Input::Input(){};

Input::Input(uint8_t new_id){
    id = new_id;
    char config_key[9] = {"inputx"};
    sprintf(config_key, "input%i", new_id);
    if (config::get_key(config_key) == 1){
        button = true;
        ESP_LOGI(TAG, "Input %i is configured as button", id);
    }
}

void Input::press_callback(void* args){

}

void Input::hold_callback(void * args){

}


void Input::toggle(){
    if(!button){
        press_callback(0);
        return;
    }
    // Timer already active so we are handling second or third press
    if (esp_timer_is_active(press_timer)){
        esp_timer_stop(press_timer);
        switch (current_press)
        {
        case 0: // Single press -> schedule second press
            current_press++;
            break;
        case 1: // Double press already scheduled but got thrid press
            current_press++; // Reset and execute third press action
            press_callback(&current_press);
        default:
            current_press = 0;
            break;
        }
    }
    // Hold timer active so we held button and stop now -> stop timer
    if (esp_timer_is_active(hold_timer)){
        esp_timer_stop(hold_timer);
    }
    // Start timers when button switch
    const esp_timer_create_args_t timer_args = {
            .callback = &press_callback,
            .arg = &current_press
    };
    esp_timer_create(&timer_args, &press_timer);
    esp_timer_start_once(press_timer, PRESS_DELAY);
    // Only start hold timer when current press is 0 (first press)
    if (current_press == 0){
        const esp_timer_create_args_t periodic_timer_args = {
                .callback = &start_periodic
        };
        esp_timer_create(&periodic_timer_args, &hold_timer);
        esp_timer_start_once(hold_timer, HOLD_DELAY);
    }
}

void Input::start_periodic(void* args){
    esp_timer_delete(hold_timer);
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &hold_callback
    };
    esp_timer_create(&periodic_timer_args, &hold_timer);
    esp_timer_start_periodic(hold_timer, PERIODIC_DELAY);
}
void Input::heartbeat(){

};
