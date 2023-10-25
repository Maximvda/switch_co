#pragma once

#include "stdint.h"
#include "driver/twai.h"

namespace driver::can {

    class CanDriver
    {
    public:
        void init();
        void tick();

        bool transmit(const twai_message_t& message);
    };

}