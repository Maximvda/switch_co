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

        enum class Event {
            EVENT_PERIODIC,
            EVENT_CAN_TRANSMIT
        };

        TimerHandle_t timer_;
        driver::CanDriver can_driver_;
        Device ginco_dev_ {can_driver_};
        void handle(utils::Message&) override;
        void onStart() override;
        void tick() override;

        Milliseconds queueTimeout() override { return 10; }

       public:

        GincoTask(uint32_t priority): StandardTask(priority, 100) {}

        void handleCanMessage(GincoMessage& data);

        const char* name() const override { return "ginco"; }

        bool periodicEvent() { return post(Event::EVENT_PERIODIC); }

        bool transmit(GincoMessage& message) {
            return post(Event::EVENT_CAN_TRANSMIT, std::make_unique<GincoMessage>(message), 25);
        }
    };

}  // namespace app
