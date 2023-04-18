#include "device.h"
#include "input.h"
#include "gpio.h"
#include "esp_log.h"

static Device switch_module;
static driver::Gpio gpio;

static const char* TAG = {"Device"};

void device::init(){
    switch_module.init();
	gpio = driver::Gpio(switch_module.toggle_switch);
    gpio.init();
}

Device::Device(){};

void Device::init(){
    for(uint8_t i=0; i < TOTAL_GPIO; i++){
        inputs[i] = Input(i);
    }
}

void Device::toggle_switch(uint8_t switch_id){
    ESP_LOGI(TAG, "Yes got a callback form gpio %i", switch_id);
}
