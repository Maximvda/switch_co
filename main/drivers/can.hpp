#pragma once

#include "stdint.h"
#include <functional>
#include "driver/twai.h"
#include "ginco_types.hpp"

using data::GincoMessage;

namespace driver {

    class CanDriver
    {
    private:
        using MessageCb = std::function<void(const GincoMessage& mes)>;
        MessageCb message_cb_;
    public:
        void init(MessageCb cb_fnc);
        void tick();

        bool transmit(const twai_message_t& message);
    };

}