#pragma once

/* ginco includes */
#include "config.hpp"
#include "ginco_types.hpp"

using data::GincoMessage;

namespace app
{
    class Device {
        private:
            uint8_t id_ {0};
            driver::ConfigDriver& config_;
            uint32_t rng_address_req_;
            GincoMessage ginco_mes_;

            void requestNewId();

        public:
            Device() : config_(driver::ConfigDriver::instance()){};
            void init();
            void secondTick();
            void handleConfig(GincoMessage& message);
    };

} // namespace app;

