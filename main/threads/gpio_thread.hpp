#pragma once

#include "standard_task.hpp"
#include  "concurrent.hpp"
#include "events.h"
#include "gpio_handler.hpp"
#include "PCF8575.hpp"

using utils::StandardTask;
using utils::Milliseconds;
using app::GpioHandler;
using component::PCF8575;

namespace app {

    class GpioTask : public StandardTask {

    private:
        PCF8575 io_expander_;
        void handle(utils::Message&) override;
        void onStart() override;
        void onTimeout() override;
        Milliseconds queueTimeout() override { return 10;};

    public:
        GpioHandler gpio_handler;

        GpioTask(uint32_t priority) : StandardTask(priority) {}

        const char * name() const override { return "Gpio"; }

        bool updateAddress(){return post(EVENT_ADDRESS_UPDATE);}

        bool setOutput(uint8_t id){return post(EVENT_OUTPUT_SET, id);}
        bool clearOutput(uint8_t id){return post(EVENT_OUTPUT_CLEAR, id);}
        bool toggleOutput(uint8_t id){return post(EVENT_OUTPUT_TOGGLE, id);}

    };

}
