#include "input.hpp"

#include "esp_log.h"

#include "supervisor.hpp"

#define PRESS_DELAY 0.4*1000*1000
//#define PERIODIC_DELAY 0.01*1000*1000
#define PERIODIC_DELAY 0.4*1000*1000

const static char* TAG = {"input"};

using modules::Input;
using data::Function;

Input::Input(uint8_t new_id, bool button)
{
    id = new_id;
    button_ = button;
}

void Input::tick()
{
    if (current_press_ == 0)
        return;
    uint64_t current_time = esp_timer_get_time();
    ESP_LOGI(TAG, "%u: Current press %u , time %llu last time %llu and state %i", id, current_press_, current_time, last_press_time_, state_);
    /* When button is unpressed and hsa been long since previous press*/
    if (((current_time - last_press_time_) > PRESS_DELAY) ) //&& !state_)
    {
        pressCallback();
        return;
    }

    /* Check if hold callback is required */
    if ((current_time - hold_time_) > PERIODIC_DELAY)
    {
        hold_time_ = current_time;
        holdCallback();
    }
}

void Input::handleMessage(GincoMessage& message)
{
    switch(message.function)
    {
        case Function::REQUEST_STATE:
            break;
        default:
            break;
    }
    // uint8_t value = static_cast<uint8_t>(driver::gpio::get_level(id));

    // message.acknowledge(1, &value);
}

void Input::pressCallback()
{
    ESP_LOGI(TAG, "Really in callback %u", current_press_);
    switch (current_press_){
        case 2:
            ESP_LOGI(TAG, "First press");
            break;
        case 4:
            ESP_LOGI(TAG, "Second press");
            break;
        case 6:
            ESP_LOGI(TAG, "Third press");
        default:
            break;
    }
    current_press_ = 0; // Reset the press count
}

void Input::holdCallback()
{
    ESP_LOGI(TAG, "Hold callback %u", this->id);
}

void Input::onToggle(bool state){
    ESP_LOGI(TAG, "%u: toggle %u", id, current_press_);
    current_press_++;
    if(!button_){
        //TODO: What should happen in this case?!
        return;
    }
    state_ = state;
    last_press_time_ = esp_timer_get_time();
}

// void Input::heartbeat(){

// };

// void Input::set_button(bool value){
//     char config_key[9] = {"inputx"};
//     sprintf(config_key, "input%i", id);
//     config::set_key(config_key, value ? 1 : 0);
// }