#pragma once

// #include <stdint.h>
// #include "esp_partition.h"
// #include "mqtt_client.h"
// #include "esp_ota_ops.h"
#include "ginco_types.hpp"

using data::GincoMessage;


namespace driver
{
    class UpgradeHandler
    {
        private:
            const esp_partition_t* ota_partition_;
	        esp_ota_handle_t update_handle_;
            uint64_t image_size_;

            void end();
        public:
            void init(const GincoMessage& message);
            void handle(const GincoMessage& message);
    };

} // namespace driver

// class Receiver {
// public:
// 	virtual bool init(Ginco__Command* command) = 0;
// 	virtual bool receive(const uint8_t * data, uint32_t len) = 0;
// 	virtual void complete() = 0;
// 	virtual void fail() = 0;
// 	virtual ~Receiver() {};
// };
