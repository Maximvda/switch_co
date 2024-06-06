#pragma once

/* ginco includes */
#include "config.hpp"
#include "ginco_types.hpp"
#include "upgrade.hpp"

namespace app {
    class Device {
       private:
        uint8_t id_ {0};
        driver::ConfigDriver& config_;
        uint32_t rng_address_req_;
        data::GincoMessage ginco_mes_;
        driver::UpgradeHandler upgrade_handler_;

        void requestNewId();

       public:
        Device(): config_(driver::ConfigDriver::instance()) {};
        void init();
        void secondTick();
        void handleConfig(GincoMessage& message);
        void handleAction(GincoMessage& message);
    };

}  // namespace app
