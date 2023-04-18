#ifndef __GPIO_DRIVER_H_
#define __GPIO_DRIVER_H_

#include "stdint.h"
#include "driver/gpio.h"

#define TOTAL_GPIO 7

namespace driver {

    namespace gpio {
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

        static void task(void* pxptr);
    }

    class Gpio {
        public:
            Gpio();
            explicit Gpio(void (*_callback)(uint8_t id));
            bool init();
        private:
            bool init_input();
            bool init_output();
            gpio_num_t input_arr[TOTAL_GPIO] {GPIO_NUM_15,GPIO_NUM_33,GPIO_NUM_26,GPIO_NUM_27,GPIO_NUM_13,GPIO_NUM_4,GPIO_NUM_16};
            gpio_num_t output_arr[TOTAL_GPIO] {GPIO_NUM_23,GPIO_NUM_25,GPIO_NUM_14,GPIO_NUM_12,GPIO_NUM_19,GPIO_NUM_18,GPIO_NUM_17};
            bool input_state[TOTAL_GPIO] {false};
    };

}

#endif