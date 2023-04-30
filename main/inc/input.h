#ifndef __INPUT_H_
#define __INPUT_H_

#include "can_driver.h"
#include "esp_timer.h"

class Input {
    private:
        uint8_t id {0};
        bool button {false};
        bool state {false};
        esp_timer_handle_t press_timer;
        esp_timer_handle_t hold_timer;
        volatile uint8_t current_press {0};
        volatile bool hold_active {false};

    public:
        Input();
        explicit Input(uint8_t id, void (*_callback)(void* arg), void (*_hold_callback)(void* arg));

        void handle_message(driver::can::message_t can_mes);

        void heartbeat();
        void toggle();
        void set_button(bool value);
        void press_callback();
        void hold_callback();
};

#endif