#pragma once

#include <string.h>
#include "driver/twai.h"

namespace data
{

    enum class Function
    {
        TURN_ON,
        TURN_OFF,
        TOGGLE,
        EFFECT,
        PAUSE_EFFECT,
        UPGRADE,
        FW_IMAGE,
        REQUEST_ADDRESS,
        REQUEST_STATE = 0xFF,
    };

    struct OutputState
    {
        /* Is the output in pwm mode */
        bool pwm_mode {false};
        /* Is the output high or low (only valid for not pwm mode) */
        bool high {false};
        /* Indicate sthe pwm level */
        uint8_t pwm_level {0};
    };

    class GincoMessage
    {
    private:
        bool event_ {false};
        bool linked_ {false};
        bool ack_ {false};
        uint8_t feature_type_ {0};
        uint8_t index_ {0};
        twai_message_t can_message_;

        uint32_t id() const
        {
            return (
                (event_ ? 1 << 26 : 0) +
                (source_id << 18) +
                (linked_ ? 1 << 17 : 0) +
                (ack_ ? 1 << 16 : 0) +
                (feature_type_ << 13) +
                (index_ << 8) +
                static_cast<uint8_t>(function));
        }

    public:
        uint8_t source_id {0};
        Function function;
        uint64_t data {0};
        uint8_t data_length {0};
        GincoMessage(){};

        GincoMessage(twai_message_t message) {
            source_id = (message.identifier >> 18) & 0xFF;
            linked_ = (message.identifier >> 17) & 0x01;
            ack_ = (message.identifier >> 16) & 0x01;
            feature_type_ = (message.identifier >> 13) & 0x07;
            index_ = (message.identifier >> 8) & 0x1F;
            function = static_cast<Function>(message.identifier & 0xFF);
            data_length = message.data_length_code;
            memcpy(&data, message.data, message.data_length_code);
        };

        twai_message_t& canMessage()
        {
            can_message_.identifier = id();
            can_message_.extd = 1;
            can_message_.data_length_code = data_length;
            memcpy(can_message_.data, &data, data_length);
            return can_message_;
        }

        bool operator==(GincoMessage &lhs)
        {
            return lhs.id() == id();
        }

        GincoMessage acknowledge();

        bool isAcknowledge(GincoMessage &other)
        {
            // invert the other message and compare id
            other.ack_ = !other.ack_;
            bool resp = other == *this;
            other.ack_ = !other.ack_;
            return resp;
        }

        bool send(bool acknowledge=false);
    };

}