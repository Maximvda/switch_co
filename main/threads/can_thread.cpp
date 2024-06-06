#include "can_thread.hpp"

#include "esp_log.h"
#include "supervisor.hpp"

using namespace app;
using data::ConfigFunction;
using utils::Message;

const static char* TAG = "can thread";

void CanTask::onStart() {
    can_driver.init([](std::unique_ptr<GincoMessage> message) {
        if (!app::taskFinder().ginco().frameReady(std::move(message))) {
            ESP_LOGI(TAG, "QUEUE FULL!");
        }
    });
    app::taskFinder().ginco().canReady();
    ESP_LOGI(TAG, "started.");
}

void CanTask::tick() { can_driver.tick(); }

void CanTask::handle(Message& message) {
    switch (message.event()) {
        case EVENT_CAN_TRANSMIT: {
            if (auto mes = message.takeValue<GincoMessage>()) {
                can_driver.transmit(*mes.get());
            }
            break;
        }
        case EVENT_ADDRESS_UPDATE: {
            if (auto value = message.uint32Value()) {
                can_driver.address(value);
            }
            break;
        }
        /* USED to check incomming CAN messages */
        case EVENT_WAKE: {
            break;
        }
        default:
            assert(0);
    }
}