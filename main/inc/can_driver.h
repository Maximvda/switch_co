#ifndef __CAN_DRIVER_H_
#define __CAN_DRIVER_H_

#include "stdint.h"

namespace driver {

    namespace can {

        struct message_t{
            uint32_t id;
            uint8_t source_id;
            bool event {false};
            bool linked;
            bool ack;
            uint8_t feature_type;
            uint8_t index;
            uint8_t function_address;
            uint8_t buffer_size;
            uint64_t data;
            uint32_t get_can_id(){
                return (
                    (event ? 1 << 26 : 0) +
                    (source_id << 18) + 
                    (linked ? 1 << 17 : 0) + 
                    (ack ? 1 << 16 : 0) + 
                    (feature_type << 13) + 
                    (index << 8) + 
                    function_address
                );
            }
        };

        void init(void (*_callback)(can::message_t message));
        bool transmit(const can::message_t message, const uint8_t length, const uint8_t* data);

        void receive_task(void* pxptr);
    }

}

#endif