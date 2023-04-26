#include "device.h"
#include "input.h"
#include "output.h"
#include "gpio.h"
#include "esp_log.h"

static Device switch_module;

static const char* TAG = {"Device"};

void device::init(){
    switch_module.init();
    driver::gpio::init(switch_module.toggle_switch);
}

Device::Device(){};

void Device::init(){
    if (esp_timer_init() != ESP_OK){
        ESP_LOGE(TAG, "Failed to initialise esp timer");
    }
    for(uint8_t i=0; i < TOTAL_GPIO; i++){
        inputs[i] = Input(i);
        outputs[i] = Output(i);
    }
}

void Device::toggle_switch(uint8_t switch_id){
    ESP_LOGI(TAG, "Yes got a callback form gpio %i", switch_id);
}
