#pragma once

/* ginco includes */
#include "can.hpp"
#include "concurrent.hpp"
#include "device.hpp"
#include "ginco_message.hpp"
#include "os_timer.hpp"
#include "standard_task.hpp"

using app::Device;
using utils::Milliseconds;
using utils::StandardTask;

namespace app {

    class GincoTask: public StandardTask {
       private:

        enum class Event {
            EVENT_PERIODIC,
            EVENT_CAN_TRANSMIT
        };

        utils::OSTimer timer_;
        driver::CanDriver can_driver_;
        std::unique_ptr<Device> ginco_dev_ {nullptr};
        void handle(utils::Message&) override;
        void onStart() override;
        void tick() override;

        Milliseconds queueTimeout() override { return 10; }

        void setNewAddress(uint32_t id);

       public:

        GincoTask(uint32_t priority): StandardTask(priority, 100) {}

        void handleCanMessage(ginco::GincoMessage& data);

        const char* name() const override { return "ginco"; }

        bool periodicEvent() { return post(Event::EVENT_PERIODIC); }

        bool transmit(ginco::GincoMessage& message) {
            return post(Event::EVENT_CAN_TRANSMIT, std::make_unique<ginco::GincoMessage>(message), 25);
        }
    };

}  // namespace app
