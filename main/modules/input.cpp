#include "input.hpp"

#include "esp_log.h"

#define PRESS_DELAY 0.4*1000*1000
//#define PERIODIC_DELAY 0.01*1000*1000
#define PERIODIC_DELAY 0.4*1000*1000

static void staticHoldCallback(void* arg);
static void staticPressCallback(void* arg);

const static char* TAG = {"input"};

using modules::Input;
using data::Function;

Input::Input(uint8_t new_id, bool button)
{
    id = new_id;
    button_ = button;
}

void Input::createTimers()
{
    // Creating the timers
    const esp_timer_create_args_t timer_args = {
        staticPressCallback,
        this,
        ESP_TIMER_TASK,
        nullptr,
        true
    };
    esp_timer_create(&timer_args, &press_timer_);
    const esp_timer_create_args_t periodic_timer_args = {
        staticHoldCallback,
        this,
        ESP_TIMER_TASK,
        nullptr,
        true
    };
    esp_timer_create(&periodic_timer_args, &hold_timer_);
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
    ESP_LOGI(TAG, "Toggling input %u, with value %u", id, current_press_);
    current_press_++;
    if(!button_){
        //TODO: What should happen in this case?!
        return;
    }
    // Timer already active so we are handling second or third press
    if (esp_timer_is_active(press_timer_) and current_press_ % 2 == 0 ){
        esp_timer_stop(press_timer_);
    }

    // Start timers when button switch
    esp_timer_start_once(press_timer_, PRESS_DELAY);
    // Only start hold timer when current press is 0 (first press)
    if (current_press_ == 1 and !hold_active_){
        hold_active_ = true;
        esp_timer_start_periodic(hold_timer_, PERIODIC_DELAY);
    }
    // Hold timer active so we held button and stop now -> stop timer
    else if (esp_timer_is_active(hold_timer_)){
        esp_timer_stop(hold_timer_);
        hold_active_ = false;
    }
}

// void Input::heartbeat(){

// };

// void Input::set_button(bool value){
//     char config_key[9] = {"inputx"};
//     sprintf(config_key, "input%i", id);
//     config::set_key(config_key, value ? 1 : 0);
// }

static void staticHoldCallback(void* arg)
{
    if (arg != nullptr)
    {
        Input* input = reinterpret_cast<Input*>(arg);
        input->holdCallback();
    }
}
static void staticPressCallback(void* arg)
{
    if (arg != nullptr)
    {
        Input* input = reinterpret_cast<Input*>(arg);
        input->pressCallback();
    }
}