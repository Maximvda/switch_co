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
    state = value;
}

void Output::set_pwm(uint32_t level){
    // Not in pwm mode so switch this
    if (!pwm){
        driver::gpio::change_output(id, true, level);
        pwm = true;
    }
    uint32_t value = 81.91 * static_cast<float>(level);
    driver::gpio::set_level(id, value);
}

void Output::set_state(bool on){
    state = on;
    uint32_t value = state ? 1 : 0;
    if (pwm){
        driver::gpio::change_output(id, false, value);
    }
    driver::gpio::set_output(id, value);
}

void Output::toggle(){
    set_state(!state);
}
