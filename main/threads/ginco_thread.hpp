#pragma once

/* freertos includes */
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

/* ginco includes */
#include "concurrent.hpp"
#include "device.hpp"
#include "events.h"
#include "ginco_types.hpp"
#include "standard_task.hpp"

using app::Device;
using data::GincoMessage;
using utils::Milliseconds;
using utils::StandardTask;

namespace app {

    class GincoTask: public StandardTask {
       private:
        Device ginco_dev_;
        TimerHandle_t timer_;
        void handle(utils::Message&) override;
        void onStart() override;
        void tick() override;

       public:

        GincoTask(uint32_t priority): StandardTask(priority, 25) {}

        const char* name() const override { return "ginco"; }

        bool frameReady(std::unique_ptr<GincoMessage> message) {
            return post(EVENT_CAN_RECEIVED, std::move(message), 30);
        }

        bool canReady() { return post(EVENT_CAN_READY); }

        bool secondEvent() { return post(EVENT_SECOND); }
    };

}  // namespace app
