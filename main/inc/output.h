#ifndef __OUTPUT_H_
#define __OUTPUT_H_

#include "stdint.h"
#include "can_driver.h"

namespace output {
    enum class Effect{
        FADE,
        FLICKER,
        PULSE
    };
}

class Output {
    private:
        uint8_t id;
        bool pwm {false};
        bool state;
        uint8_t brightness {0};

    public:
        Output();
        explicit Output(uint8_t id);

        void handle_message(driver::can::message_t can_mes);

        void toggle(void);
        void set_state(bool on);
        void set_pwm(uint32_t level);
        void run_effect(output::Effect effect);
};

#endif