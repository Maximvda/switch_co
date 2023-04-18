#ifndef __DEVICE_H_
#define __DEVICE_H_

#include "input.h"
#include "gpio.h"

namespace device {
    void init();
}


class Device {
    private:
        Input inputs[TOTAL_GPIO];

    public:
        Device();
        void init();
        void heartbeat();
        static void toggle_switch(uint8_t switch_id);
        void double_press_switch(uint8_t switch_id);
        void hold_switch(uint8_t switch_id);
        void release_switch(uint8_t switch_id);
};

#endif