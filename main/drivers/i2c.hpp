#pragma once

/* esp includes */
#include "driver/i2c.h"

namespace driver
{
    class I2C
    {
        public:
            void init(uint32_t clk_speed);
            void read(uint8_t address, uint8_t* buffer, uint8_t length);
            void write(uint8_t address, uint8_t* buffer, uint8_t length);
    };

} // namespace driver