#pragma once

/* std includes */
#include <functional>
#include <memory>

/* Esp includes */
#include "driver/twai.h"

/* Ginco includes */
#include "ginco_types.hpp"

using data::GincoMessage;

namespace driver {

    class CanDriver {
       private:
        using MessageCb = std::function<void(std::unique_ptr<GincoMessage> mes)>;
        MessageCb message_cb_;
        /* Id of the module to filter out can messages */
        uint8_t id_ {0};
        /* Start the can driver */
        void start();

       public:
        /* Init module with callback function to process messages */
        void init(MessageCb cb_fnc);

        /* Check if messages have been received */
        void tick();

        /**
         * @brief Transmit message over CAN bus
         *
         * @param message Message to be transmitten
         * @return true When queued successfully
         * @return false When error occured during queing
         */
        bool transmit(GincoMessage &message);

        /**
         * @brief Set address of module
         *
         * @param id New id of the module, used to filter can messages
         */
        void address(uint8_t id);
    };

}  // namespace driver