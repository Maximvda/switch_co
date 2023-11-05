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
        UPGRADE_FINISHED,
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

        uint32_t id() const;

    public:
        uint8_t source_id {0};
        Function function;
        uint64_t data {0};
        uint8_t data_length {0};

        GincoMessage(){};
        GincoMessage(const twai_message_t& message);

        twai_message_t& canMessage();
        GincoMessage acknowledge();

        bool isAcknowledge(GincoMessage &other);

        bool send(bool acknowledge=false);

        bool operator==(GincoMessage &lhs)
        {
            return lhs.id() == id();
        }
    };

}