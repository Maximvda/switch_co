#pragma once

#include "ginco_types.hpp"

using data::OutputState;
using data::GincoMessage;

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

        public:
            Output() {};
            explicit Output(const uint8_t id);

            void handleMessage(GincoMessage& message);
            void runEffect(output::Effect effect);
    };

} // namespace module


