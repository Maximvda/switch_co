#pragma once

// #include <stdint.h>
// #include "esp_partition.h"
// #include "mqtt_client.h"
// #include "esp_ota_ops.h"
#include "esp_ota_ops.h"

#include "ginco_message.hpp"

namespace driver {
    class UpgradeHandler {
       private:
        const esp_partition_t* ota_partition_;
        esp_ota_handle_t update_handle_;
        uint64_t image_size_ {0};
        uint32_t mes_received_ {0};

        void complete();
        bool partitionValid();

       public:
        bool init(ginco::GincoMessage& message);
        bool handle(ginco::GincoMessage& message);
        void fail();

        bool upgrading() { return image_size_ != 0; };
    };

}  // namespace driver
