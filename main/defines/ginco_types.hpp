#pragma once

#include <string.h>
#include "driver/twai.h"

#include "supervisor.hpp"

namespace data
{

    enum class Function
    {
        TURN_ON,
        TURN_OFF,
        TOGGLE,
        EFFECT,
        PAUSE_EFFECT,
        REQUEST_STATE = 0xFF,
    };


    class GincoMessage
    {
    private:
        uint8_t source_id_;
        bool event_ {false};
        bool linked_;
        bool ack_;
        uint8_t feature_type_;
        uint8_t index_;
        uint8_t data_length_;

        uint32_t getId()
        {
            return (
                (event_ ? 1 << 26 : 0) +
                (source_id_ << 18) +
                (linked_ ? 1 << 17 : 0) +
                (ack_ ? 1 << 16 : 0) +
                (feature_type_ << 13) +
                (index_ << 8) +
                static_cast<uint8_t>(function)
            );
        };

        twai_message_t getMessage()
        {
            twai_message_t message;
            message.identifier = getId();
            message.extd = 1;
            message.data_length_code = data_length_;
            memcpy(message.data, &data, data_length_);
            return message;
        };

    public:
        Function function;
        uint64_t data;

        GincoMessage(twai_message_t message) {
            source_id_ = (message.identifier >> 18) & 0xFF;
            linked_ = (message.identifier >> 17) & 0x01;
            ack_ = (message.identifier >> 16) & 0x01;
            feature_type_ = (message.identifier >> 13) & 0x07;
            index_ = (message.identifier >> 8) & 0x1F;
            function = static_cast<Function>(message.identifier & 0xFF);
            data_length_ = message.data_length_code;
            data = message.data[0];
        };

        bool acknowledge()
        {
            ack_ = true;
            return app::taskFinder().can().transmit(getMessage());
        }

    };

}