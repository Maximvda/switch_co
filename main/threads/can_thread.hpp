#pragma once

#include "standard_task.hpp"

#include "can.hpp"
#include "upgrade.hpp"
#include "concurrent.hpp"
#include "events.h"
#include "ginco_types.hpp"

using utils::StandardTask;
using utils::Milliseconds;
using driver::CanDriver;
using driver::UpgradeHandler;
using data::GincoMessage;

namespace app {

    class CanTask : public StandardTask {

    private:
        UpgradeHandler upgrade_handler_;

        void handleCanMes(GincoMessage& mes);

        void handle(utils::Message&) override;
        void onStart() override;
        void tick() override;
        Milliseconds queueTimeout() override { return 10; };

    public:
        CanDriver can_driver;

        CanTask(uint32_t priority) : StandardTask(priority) {}

        const char * name() const override { return "Can"; }

        bool transmit(GincoMessage &message)
        {
            return post(EVENT_CAN_TRANSMIT, std::make_unique<GincoMessage>(message));
        }

        bool address(uint32_t id){return post(EVENT_ADDRESS_UPDATE, id);}
    };

}
