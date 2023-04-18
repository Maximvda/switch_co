#include "input.h"

#include <string>

#include "config.h"
#include "esp_log.h"

const static char* TAG = {"input"};

Input::Input(){};

Input::Input(uint8_t new_id){
    id = new_id;
    char config_key[9] = {"input0"};
    sprintf(config_key, "input%i", new_id);
    if (config::get_key(config_key) == 1){
        button = true;
        ESP_LOGI(TAG, "Input %i is configured as button", id);
    }
}

void Input::heartbeat(){

};
