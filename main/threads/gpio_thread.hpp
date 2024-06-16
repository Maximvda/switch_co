#pragma once

#include "PCF8575.hpp"
#include "concurrent.hpp"
#include "gpio_handler.hpp"
#include "standard_task.hpp"

using app::GpioHandler;
using component::PCF8575;
using utils::Milliseconds;
using utils::StandardTask;

namespace app {

    class GpioTask: public StandardTask {
       private:
        enum events {
            EVENT_ADDRESS_UPDATE,
            EVENT_OUTPUT_SET,
            EVENT_OUTPUT_CLEAR,
            EVENT_OUTPUT_TOGGLE
        };

        PCF8575 io_expander_;
        void handle(utils::Message&) override;
        void onStart() override;
        void onTimeout() override;

        Milliseconds queueTimeout() override { return 10; };

       public:
        GpioHandler gpio_handler;

        GpioTask(uint32_t priority): StandardTask(priority) {}

        const char* name() const override { return "Gpio"; }

        bool updateAddress() { return post(EVENT_ADDRESS_UPDATE); }

        bool setOutput(uint8_t id) { return post(EVENT_OUTPUT_SET, id); }

        bool clearOutput(uint8_t id) { return post(EVENT_OUTPUT_CLEAR, id); }

        bool toggleOutput(uint8_t id) { return post(EVENT_OUTPUT_TOGGLE, id); }
    };

}  // namespace app
