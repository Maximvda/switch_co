#pragma once

// #include <stdint.h>
// #include "esp_partition.h"
// #include "mqtt_client.h"
// #include "esp_ota_ops.h"
#include "ginco_types.hpp"
#include "esp_ota_ops.h"

using data::GincoMessage;


namespace driver
{
    class UpgradeHandler
    {
        private:
            const esp_partition_t* ota_partition_;
            esp_ota_handle_t update_handle_;
            uint64_t image_size_ {0};
            uint32_t mes_received_ {0};

            void complete();
            bool partitionValid();
        public:
            bool init(const GincoMessage& message);
            bool handle(const GincoMessage& message);
            void fail();
    };

} // namespace driver
