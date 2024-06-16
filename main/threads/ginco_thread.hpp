#pragma once

/* freertos includes */
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

/* ginco includes */
#include "can.hpp"
#include "concurrent.hpp"
#include "device.hpp"
#include "ginco_types.hpp"
#include "standard_task.hpp"

using app::Device;
using data::GincoMessage;
using utils::Milliseconds;
using utils::StandardTask;

namespace app {

    class GincoTask: public StandardTask {
       private:

        enum events {
            SECOND_EVENT,
        };

        TimerHandle_t timer_;
        driver::CanDriver can_driver_;
        Device ginco_dev_ {can_driver_};
        void handle(utils::Message&) override;
        void onStart() override;
        void tick() override;

        Milliseconds queueTimeout() override { return 10; }

       public:

        GincoTask(uint32_t priority): StandardTask(priority, 25) {}

        void handleMessage(GincoMessage& data);

        const char* name() const override { return "ginco"; }

        bool secondEvent() { return post(SECOND_EVENT); }

        bool transmit(GincoMessage& message) { return can_driver_.transmit(message); }
    };

}  // namespace app
