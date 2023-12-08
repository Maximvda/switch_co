#pragma once

#include "ginco_types.hpp"
#include "gpio.hpp"

using data::OutputState;
using data::GincoMessage;
using driver::GpioDriver;

namespace modules
{

    namespace output {
        enum class Effect{
            FADE,
            FLICKER,
            PULSE
        };
    }

    class Output {
        private:
            uint8_t id_;
            OutputState state_;
            GpioDriver& driver_;

        public:
            explicit Output(const uint8_t id, GpioDriver& driver);

            void handleMessage(GincoMessage& message);
            void runEffect(output::Effect effect);
            void set(){state_.high = true; driver_.setOutput(id_, state_.high);}
            void clear(){state_.high = false; driver_.setOutput(id_, state_.high);}
            void toggle(){state_.high = !state_.high; driver_.setOutput(id_, state_.high);}
    };

} // namespace module


