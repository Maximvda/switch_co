#ifndef __GPIO_DRIVER_H_
#define __GPIO_DRIVER_H_

#include "stdint.h"
#include "driver/gpio.h"

#define TOTAL_GPIO 7

namespace driver {

    namespace gpio {

        bool init(void (*_callback)(uint8_t id));
        void set_output(uint8_t id, uint32_t value);
        bool change_output(uint8_t id, bool pwm, uint32_t level);
        void set_level(uint8_t id, uint32_t level);

        void task(void* pxptr);

    }

}

#endif