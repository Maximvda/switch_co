#pragma once

#include "stdint.h"
#include "ginco_types.hpp"
#include "driver/gpio.h"

/* std includes */
#include <cstdint>
#include <map>
#include <vector>
#include <functional>

#include <variant>

using data::OutputState;

namespace driver {

    class GpioDriver
    {
    public:
        static constexpr uint8_t TOTAL_GPIO {7};

    private:
        using StateChangeCb = std::function<void(uint8_t, bool)>;
        gpio_num_t outputs_[TOTAL_GPIO] {GPIO_NUM_23,GPIO_NUM_25,GPIO_NUM_14,GPIO_NUM_12,GPIO_NUM_19,GPIO_NUM_18,GPIO_NUM_17};
        std::pair<gpio_num_t, uint32_t> inputs_[TOTAL_GPIO] {
            std::make_pair(GPIO_NUM_15, 0),
            std::make_pair(GPIO_NUM_33, 0),
            std::make_pair(GPIO_NUM_26, 0),
            std::make_pair(GPIO_NUM_27, 0),
            std::make_pair(GPIO_NUM_13, 0),
            std::make_pair(GPIO_NUM_4, 0),
            std::make_pair(GPIO_NUM_16, 0),
        };

        /* Remember in which state the output is. PWM or GPIO */
        OutputState output_states_[TOTAL_GPIO] {};

        bool checking_input_ {false};
        /**
         * @brief This callback is run when input state has changed
         */
        StateChangeCb cb_state_change_ {};

        bool initInput();
        bool initOutput();
        bool initLed();

        bool changeOutput(const uint8_t id);
    public:
        bool init();

        /**
         * @brief Enable input gpio checking
         * @note When enabling input check one must not forget to reguraly call the Inputcheck function +- every 10ms
         *
         *
         */
        void enableInputCheck(StateChangeCb cb_state_change);

        bool setOutput(uint8_t id, std::variant<bool, uint8_t> value);
        void inputCheck();
        bool getLevel(uint8_t id);
    };

}