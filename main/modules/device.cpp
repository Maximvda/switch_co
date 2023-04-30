#include "device.h"
#include "input.h"
#include "output.h"
#include "gpio.h"
#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "can_driver.h"
#include "config.h"

static Device switch_module;

static const char* TAG = {"Device"};

// Callback functions
static void toggle_switch(uint8_t switch_id){
    switch_module.toggle_switch(switch_id);
}
static void on_can_message(driver::can::message_t message){
    switch_module.on_can_message(message);
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
    driver::can::init(on_can_message);
    switch_module.init();
}

Device::Device(){};

void Device::init(){
    id = config::get_key("device_id");
    for(uint8_t i=0; i < TOTAL_GPIO; i++){
        inputs[i] = Input(i, press_callback, hold_callback);
        outputs[i] = Output(i);
    }
}

void Device::toggle_switch(uint8_t switch_id){
    inputs[switch_id].toggle();
}

void Device::on_can_message(driver::can::message_t can_mes){
    // Not the id of this module so immediately return
    if (can_mes.id != id)
        return;
    
    switch(can_mes.feature_type){
        case 1:
            outputs[can_mes.index].handle_message(can_mes);
            break;
        case 3:
            inputs[can_mes.index].handle_message(can_mes);
            break;
        default:
            break;
    }
}