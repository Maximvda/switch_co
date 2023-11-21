#pragma once

/* freertos includes */
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

/* ginco includes */
#include "standard_task.hpp"
#include "ginco_types.hpp"
#include "concurrent.hpp"
#include "device.hpp"
#include "events.h"

using utils::StandardTask;
using utils::Milliseconds;
using data::GincoMessage;
using app::Device;

namespace app {

    class GincoTask : public StandardTask {

    private:
        Device ginco_dev_;
        TimerHandle_t timer_;
        void handle(utils::Message&) override;
        void onStart() override;
        void tick() override;

    public:

        GincoTask(uint32_t priority) : StandardTask(priority) {}

        const char * name() const override { return "ginco"; }

        bool frameReady(const GincoMessage& message)
        {
            return post(EVENT_CAN_RECEIVED, std::make_unique<GincoMessage>(message));
        }

        bool canReady(){return post(EVENT_CAN_READY);}

        bool secondEvent(){return post(EVENT_SECOND);}
    };

}
