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

static void cbTimer(TimerHandle_t xTimer) { app::taskFinder().ginco().periodicEvent(); }

void GincoTask::onStart() {
    timer_ = xTimerCreate("t_second_", Milliseconds(10000).toTicks(), pdTRUE, nullptr, cbTimer);
    ginco_dev_.init();
    ESP_LOGI(TAG, "started.");
    can_driver_.init([this](GincoMessage data) { this->handleCanMessage(data); });
    xTimerStart(timer_, 0);
}

void GincoTask::tick() { can_driver_.tick(); }

void GincoTask::handleCanMessage(GincoMessage& data) {
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
    switch (message.event<Event>()) {
        case Event::EVENT_PERIODIC: {
            ginco_dev_.periodic();
            break;
        }
        case Event::EVENT_CAN_TRANSMIT: {
            if (auto mes = message.takeValue<GincoMessage>()) {
                can_driver_.transmit(*mes.get());
            }
            break;
        }
        default:
            assert(0);
    }
}
