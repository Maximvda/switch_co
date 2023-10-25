#pragma once

#include "gpio.hpp"
#include "ginco_types.hpp"


namespace module
{

    namespace output {
        enum class Effect{
            FADE,
            FLICKER,
            PULSE
        };
    }

    using driver::OutputState;
    using data::GincoMessage;

    class Output {
        private:
            uint8_t id_;
            OutputState state_;

        public:
            Output() {};
            explicit Output(const uint8_t id, const OutputState state);

            void handleMessage(GincoMessage& message);
            void runEffect(output::Effect effect);
    };

} // namespace module


