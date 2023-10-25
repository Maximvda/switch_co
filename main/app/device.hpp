#pragma once

#include "input.hpp"
#include "output.hpp"
#include "gpio.hpp"
#include "config.hpp"
#include "ginco_types.hpp"

using modules::Output;
using modules::Input;
using data::GincoMessage;

namespace app
{
    class Device {
        private:
            Output outputs_[driver::GpioDriver::total_gpio];
            Input inputs_[driver::GpioDriver::total_gpio];
            uint8_t id_ {0};

            driver::ConfigDriver config_driver_;

        public:
            void init();
            void handleMessage(GincoMessage& message);
            // void heartbeat();
            // void toggle_switch(uint8_t switch_id);
            // void double_press_switch(uint8_t switch_id);
            // void hold_switch(uint8_t switch_id);
            // void release_switch(uint8_t switch_id);
    };

} // namespace app;

