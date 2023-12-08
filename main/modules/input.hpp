#pragma once

#include "esp_timer.h"
#include "ginco_types.hpp"

using data::GincoMessage;

namespace modules
{

    class Input {
    private:
        bool button_ {false};
        bool state_ {false};
        esp_timer_handle_t press_timer_;
        esp_timer_handle_t hold_timer_;
        uint8_t current_press_ {0};
        GincoMessage message_;

    public:
        uint8_t id {0};
        Input(){};
        Input(uint8_t id, bool button);
        void createTimers();

        void onToggle(bool state);
        void set_button(bool value);
        void holdCallback();
        void pressCallback();
        void updateAddress();
};

} // namespace modules
