#ifndef __UPGRADE_HEADER_H_
#define __UPGRADE_HEADER_H_

#include <stdint.h>
#include "esp_partition.h"
#include "mqtt_client.h"
#include "esp_ota_ops.h"
#include "ginco.pb-c.h"

namespace upgrade {

    class Handler {
        public:
            Handler();
            void init(const esp_mqtt_event_t& event);
            void handle(const esp_mqtt_event_t& event);
            void end(void);
    };
};

class Receiver {
public:
	virtual bool init(Ginco__Command* command) = 0;
	virtual bool receive(const uint8_t * data, uint32_t len) = 0;
	virtual void complete() = 0;
	virtual void fail() = 0;
	virtual ~Receiver() {};
};

#endif