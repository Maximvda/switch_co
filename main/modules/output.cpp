#include "output.h"
#include "gpio.h"

#include <string>

#include "config.h"
#include "esp_log.h"


const static char* TAG = {"Output"};

Output::Output(){};

Output::Output(uint8_t new_id){
    id = new_id;
    char config_key[10] = {"outputx"};
    sprintf(config_key, "output%i", new_id);
    if (config::get_key(config_key) == 1){
        pwm = true;
        ESP_LOGI(TAG, "Output %i is configured as pwm", id);
    }
    // Update state to last state
    sprintf(config_key, "pstate%i", new_id);
    uint8_t value = config::get_key(config_key);
    driver::gpio::set_output(id, value);
}
