#pragma once

#include "i2c.hpp"

using driver::I2C;

namespace component
{

    class PCF8575
    {
        private:
            I2C driver_;
            uint16_t io_map_ {0};
        public:
            void init();
            void set(uint8_t pin, bool value);
            bool get(uint8_t pin);
    };

} // namespace component