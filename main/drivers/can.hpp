#pragma once

/* std includes */
#include <functional>

/* Esp includes */
#include "driver/twai.h"

/* Ginco includes */
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

        bool transmit(GincoMessage &message, bool blocking = false);
    };

}