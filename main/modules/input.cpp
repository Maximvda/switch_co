#include "input.hpp"

/* esp includes */
#include "esp_log.h"

/* ginco includes */
#include "ginco_types.hpp"
#include "config.hpp"

#define PRESS_DELAY 0.4*1000*1000
//#define PERIODIC_DELAY 0.01*1000*1000
#define PERIODIC_DELAY 0.4*1000*1000

static void staticHoldCallback(void* arg);
static void staticPressCallback(void* arg);

const static char* TAG = {"input"};

using modules::Input;
using data::GincoMessage;
using driver::ConfigDriver;
using driver::ConfigKey;

Input::Input(uint8_t new_id, bool button)
{
    id = new_id;
    button_ = button;
    message_.source(ConfigDriver::instance().getKey<uint8_t>(ConfigKey::DEVICE_ID));
    if (button)
        message_.feature(data::FeatureType::BUTTON);
    else
        message_.feature(data::FeatureType::SWITCH);
}

void Input::updateAddress()
{
    message_.source(ConfigDriver::instance().getKey<uint8_t>(ConfigKey::DEVICE_ID));
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

void Input::pressCallback()
{
    switch (current_press_){
        case 2:
        {
            ESP_LOGI(TAG, "First press");
            message_.function<data::ButtonFunction>(data::ButtonFunction::PRESS);
            break;
        }
        case 4:
        {
            ESP_LOGI(TAG, "Second press");
            message_.function<data::ButtonFunction>(data::ButtonFunction::DOUBLE_PRESS);
            break;
        }
        case 6:
        {
            ESP_LOGI(TAG, "Third press");
            message_.function<data::ButtonFunction>(data::ButtonFunction::TRIPPLE_PRESS);
            break;
        }
        default:
            break;
    }
    current_press_ = 0; // Reset the press count
    message_.send();
}

void Input::holdCallback()
{
    message_.function<data::ButtonFunction>(data::ButtonFunction::HOLD);
    message_.send();
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

    if (esp_timer_is_active(hold_timer_)){
        esp_timer_stop(hold_timer_);
    } else {
        esp_timer_start_periodic(hold_timer_, PERIODIC_DELAY);
    }
}

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