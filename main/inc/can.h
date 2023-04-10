#ifndef __CAN_DRIVER_H_
#define __CAN_DRIVER_H_

#include "stdint.h"

namespace driver {

    namespace can {
        void init();
        bool transmit(const uint32_t id, const uint8_t length, const uint8_t* data);
        void receive_task();
    }

}

#endif