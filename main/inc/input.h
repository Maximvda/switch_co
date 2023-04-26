#ifndef __INPUT_H_
#define __INPUT_H_

#include "can_driver.h"
#include "esp_timer.h"

class Input {
    private:
        uint8_t id {0};
        bool button {false};
        bool state {false};
        static void hold_callback(void* arg);
        static void press_callback(void* arg);
        static void start_periodic(void* arg);
        esp_timer_handle_t press_timer;
        static esp_timer_handle_t hold_timer;
        uint8_t current_press {0};

    public:
        Input();
        explicit Input(uint8_t id);
        void heartbeat();
        void toggle();
};

#endif