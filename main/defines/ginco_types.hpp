#pragma once

/* std includes */
#include <string.h>
#include <type_traits>

#include "driver/twai.h"

namespace data
{

/* ONLY 3 bits so maximum value of 7*/
    enum class FeatureType
    {
        CONFIG,
        BUTTON,
        SWITCH,
        LIGHT,
        SENSOR,
        ACTION
    };

    /* All functions are 8 bits so maximum value of 255 */
    enum class ConfigFunction
    {
        REQUEST_ADDRESS,
        SET_ADDRESS,
        UPGRADE,
        FW_IMAGE,
        UPGRADE_FINISHED,
        HEARTBEAT
    };

    enum class ButtonFunction
    {
        PRESS,
        DOUBLE_PRESS,
        TRIPPLE_PRESS,
        HOLD,
    };

    enum class ActionFunction
    {
        OUTPUT_TOGGLE,
        OUTPUT_SET,
        OUTPUT_CLEAR,
        SCENE
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
        twai_message_t can_message_;

    public:
        GincoMessage(){
            can_message_.identifier = 0;
            can_message_.extd = 1;
            can_message_.rtr = 0;
            can_message_.ss = 0;
            can_message_.self = 0;
            can_message_.dlc_non_comp = 0;
            can_message_.data_length_code = 0;
        }
        GincoMessage(const twai_message_t& message){can_message_ = message;}
        GincoMessage(const GincoMessage& o){can_message_ = o.can_message_;}

        uint32_t id() const {return can_message_.identifier;}
        uint8_t length() const {return can_message_.data_length_code;}
        uint8_t source() const {return static_cast<uint8_t>(can_message_.identifier >> 18);}
        void source(uint8_t src_id)
        {
            can_message_.identifier &= ~(0xFF << 18); /* First clear the bits of src */
            can_message_.identifier |= (src_id << 18); /* Now set them */
        }
        bool linked() const {return can_message_.identifier & (1 << 17);}

        FeatureType feature() const {return static_cast<FeatureType>((can_message_.identifier >> 13) & 0x07);}
        void feature(FeatureType value)
        {
            can_message_.identifier &= ~(0x07 << 13);
            can_message_.identifier |= (static_cast<uint8_t>(value) << 13);
        }

        uint8_t index() const {return (can_message_.identifier >> 8) & 0x1F;}
        void index(uint8_t value)
        {
            can_message_.identifier &= ~(0x1F << 8);
            can_message_.identifier |= (value << 8);
        }

        template<typename T>
        T function() const {return static_cast<T>(can_message_.identifier & 0xFF);}

        template<typename T>
        void function(T value)
        {
            can_message_.identifier &= ~0xFF;
            can_message_.identifier |= static_cast<uint8_t>(value);
        }

        twai_message_t& message() {return can_message_;};

        bool send(bool acknowledge=false);

        template<typename T, bool OFFSET=false >
        void data(T value, uint8_t size = 0)
        {
            if constexpr(std::is_unsigned_v<T>)
            {
                if constexpr(OFFSET)
                {
                    can_message_.data_length_code = sizeof(T)*(size+1);
                    void* d_ptr = can_message_.data + sizeof(T)*size;
                    memcpy(d_ptr, &value, sizeof(T));
                }
                else
                {
                    can_message_.data_length_code = sizeof(T);
                    memcpy(can_message_.data, &value, sizeof(T));
                }
            }
            else if constexpr(std::is_pointer_v<T>)
            {
                assert(sizeof(T) != 1); /* Can implement this but multiplication required */
                can_message_.data_length_code = size;
                memcpy(can_message_.data, value, size);
            }
        }

        template<typename T, bool OFFSET=false >
        T data(uint8_t offset = 0)
        {
            if constexpr(std::is_unsigned_v<T>)
            {
                T value;
                if constexpr(OFFSET)
                {
                    void* d_ptr = can_message_.data + sizeof(T)*offset;
                    memcpy(&value, d_ptr, sizeof(T));
                }
                else
                    memcpy(&value, can_message_.data, sizeof(T));
                return value;
            }
            else if constexpr(std::is_pointer_v<T>)
            {
                return can_message_.data;
            }
        }

    }; // class GincoMessage

} // namespace data