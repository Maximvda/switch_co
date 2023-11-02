#pragma once

#include "standard_task.hpp"
#include "ginco_types.hpp"
#include  "concurrent.hpp"
#include "events.h"
#include "device.hpp"

using utils::StandardTask;
using utils::Milliseconds;
using data::GincoMessage;
using app::Device;

namespace app {

    class GincoTask : public StandardTask {

    private:
        Device ginco_dev_;
        void handle(utils::Message&) override;
        void onStart() override;
        void onTimeout() override;

    public:

        GincoTask(uint32_t priority) : StandardTask(priority) {}

        const char * name() const override { return "ginco"; }

        bool frameReady(const GincoMessage& message)
        {
            return post(EVENT_CAN_RECEIVED, std::make_unique<GincoMessage>(message));
        }

    };

}
