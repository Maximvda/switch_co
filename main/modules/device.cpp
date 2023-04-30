#include "device.h"
#include "input.h"
#include "output.h"
#include "gpio.h"
#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static Device switch_module;

static const char* TAG = {"Device"};

static void toggle_switch(uint8_t switch_id){
    switch_module.toggle_switch(switch_id);
}

static void hold_callback(void* arg){
    int id = *(int*) arg;
    switch_module.inputs[id].hold_callback();
}

static void press_callback(void* arg){
    int id = *(int*) arg;
    switch_module.inputs[id].press_callback();
}

void device::init(){
    driver::gpio::init(toggle_switch);
    switch_module.init();
}

Device::Device(){};

void Device::init(){
    for(uint8_t i=0; i < TOTAL_GPIO; i++){
        inputs[i] = Input(i, press_callback, hold_callback);
        outputs[i] = Output(i);
    }
}

void Device::toggle_switch(uint8_t switch_id){
    inputs[switch_id].toggle();
}
