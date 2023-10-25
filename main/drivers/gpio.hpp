#pragma once

#include "stdint.h"
#include "driver/gpio.h"

#include <variant>

namespace driver {

    struct OutputState
    {
        /* Is the output in pwm mode */
        bool pwm_mode {false};
        /* Is the output high or low (only valid for not pwm mode) */
        bool high {false};
        /* Indicate sthe pwm level */
        uint8_t pwm_level {0};
    };

    class GpioDriver
    {
    static constexpr uint8_t total_gpio {7};

    private:
        gpio_num_t inputs_[total_gpio] {GPIO_NUM_15,GPIO_NUM_33,GPIO_NUM_26,GPIO_NUM_27,GPIO_NUM_13,GPIO_NUM_4,GPIO_NUM_16};
        gpio_num_t outputs_[total_gpio] {GPIO_NUM_23,GPIO_NUM_25,GPIO_NUM_14,GPIO_NUM_12,GPIO_NUM_19,GPIO_NUM_18,GPIO_NUM_17};

        /* Remember in which state the output is. PWM or GPIO */
        OutputState output_states_[total_gpio] {};

        bool initInput();
        bool initOutput();
        bool initLed();

        bool changeOutput(const uint8_t id);
    public:

        bool init();
        bool setOutput(uint8_t id, std::variant<bool, uint8_t> value);
    };

}