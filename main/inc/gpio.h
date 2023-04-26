#ifndef __GPIO_DRIVER_H_
#define __GPIO_DRIVER_H_

#include "stdint.h"
#include "driver/gpio.h"

#define TOTAL_GPIO 7

namespace driver {

    namespace gpio {

        bool init(void (*_callback)(uint8_t id));
        void set_output(uint8_t id, uint8_t value);

        enum inputs {
            INPUT_0,
            INPUT_1,
            INPUT_2,
            INPUT_3,
            INPUT_4,
            INPUT_5,
            INPUT_6
        };

        enum outputs {
            OUTPUT_0,
            OUTPUT_1,
            OUTPUT_2,
            OUTPUT_3,
            OUTPUT_4,
            OUTPUT_5,
            OUTPUT_6
        };
    }

}

#endif