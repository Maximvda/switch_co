#ifndef __CAN_DRIVER_H_
#define __CAN_DRIVER_H_

#include "stdint.h"

namespace driver {

    namespace can {
        void init();
        bool transmit(const uint32_t id, const uint8_t length, const uint8_t* data);
        void receive_task(void* pxptr);

        typedef struct {
            uint32_t id;
            bool event;
            uint8_t source_id;
            bool linked;
            bool ack;
            uint8_t feature_type;
            uint8_t index;
            uint8_t function_address;
            uint8_t buffer_size;
            uint64_t data;
        } message_t;

    }

}

#endif