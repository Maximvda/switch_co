#include "ginco_thread.hpp"

/* esp includes */
#include "esp_log.h"
#include "esp_timer.h"

/* ginco includes */
#include "can.hpp"
#include "supervisor.hpp"

using app::GincoTask;
using data::FeatureType;
using utils::Message;
using utils::Milliseconds;

const static char* TAG = "ginco thread";

static void cbTimer(TimerHandle_t xTimer) { app::taskFinder().ginco().secondEvent(); }

void GincoTask::onStart() {
    timer_ = xTimerCreate("t_second_", Milliseconds(10000).toTicks(), pdTRUE, nullptr, cbTimer);
    ginco_dev_.init();
    ESP_LOGI(TAG, "started.");
    can_driver_.init([this](GincoMessage data) { this->handleMessage(data); });
}

void GincoTask::tick() { can_driver_.tick(); }

void GincoTask::handleMessage(GincoMessage& data) {
    switch (data.feature()) {
        case FeatureType::CONFIG: {
            ginco_dev_.handleConfig(data);
            break;
        }
        case FeatureType::LIGHT: {
            break;
        }
        default:
            break;
    }
}

void GincoTask::handle(Message& message) {
    switch (message.event()) {
        case EVENT_SECOND: {
            ginco_dev_.secondTick();
            break;
        }
        case EVENT_CAN_READY: {
            xTimerStart(timer_, 0);
            break;
        }
        case EVENT_CAN_TRANSMIT: {
            if (auto mes = message.takeValue<GincoMessage>()) {
                can_driver_.transmit(*mes.get());
            }
            break;
        }
        case EVENT_ADDRESS_UPDATE: {
            if (auto value = message.uint32Value()) {
                can_driver_.address(value);
            }
            break;
        }
        default:
            assert(0);
    }
}
