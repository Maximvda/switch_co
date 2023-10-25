#pragma once

#include "input.hpp"
#include "output.hpp"
#include "gpio.hpp"
#include "config.hpp"

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
            void heartbeat();
            void toggle_switch(uint8_t switch_id);
            void double_press_switch(uint8_t switch_id);
            void hold_switch(uint8_t switch_id);
            void release_switch(uint8_t switch_id);
            void on_can_message(driver::can::message_t message);
    };

} // namespace app;

