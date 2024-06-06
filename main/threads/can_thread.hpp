#pragma once

#include "standard_task.hpp"

#include "can.hpp"
#include "concurrent.hpp"
#include "events.h"
#include "ginco_types.hpp"

using data::GincoMessage;
using driver::CanDriver;
using utils::Milliseconds;
using utils::StandardTask;

namespace app {

    class CanTask: public StandardTask {
       private:

        void handleCanMes(GincoMessage& mes);

        void handle(utils::Message&) override;
        void onStart() override;
        void tick() override;

        Milliseconds queueTimeout() override { return 20; };

       public:
        CanDriver can_driver;

        CanTask(uint32_t priority): StandardTask(priority) {}

        const char* name() const override { return "Can"; }

        bool transmit(GincoMessage& message) {
            return post(EVENT_CAN_TRANSMIT, std::make_unique<GincoMessage>(message));
        }

        bool address(uint32_t id) { return post(EVENT_ADDRESS_UPDATE, id); }

        bool wake() { return post(EVENT_WAKE); }
    };

}  // namespace app
