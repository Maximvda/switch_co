#include "ginco_thread.hpp"

/* esp includes */
#include "esp_log.h"
#include "esp_timer.h"

/* ginco includes */
#include "can.hpp"
#include "supervisor.hpp"

using app::GincoTask;
using driver::ConfigKey;
using ginco::FunctionType;
using ginco::GincoMessage;
using utils::Message;
using utils::Milliseconds;

const static char* TAG = "ginco thread";

void GincoTask::onStart() {
    timer_.createTimer(10000, []() { app::taskFinder().ginco().periodicEvent(); });
    const auto device_id = driver::config::getNvsKey<uint8_t>(ConfigKey::DEVICE_ID);
    app::taskFinder().gpio().setDeviceId(device_id);
    ginco_dev_ = std::make_unique<Device>(device_id);
    ginco_dev_->init([this](uint32_t id) { this->setNewAddress(id); });
    ESP_LOGI(TAG, "started.");
    can_driver_.init(device_id, [this](GincoMessage data) { this->handleCanMessage(data); });
}

void GincoTask::tick() { can_driver_.tick(); }

void GincoTask::handleCanMessage(GincoMessage& data) {
    data.source(ginco_dev_->id());
    switch (data.functionType()) {
        case FunctionType::CONFIG: {
            ginco_dev_->handleConfig(data);
            break;
        }
        case FunctionType::ACTUATOR: {
            ginco_dev_->handleActuator(data);
            break;
        }
        default:
            break;
    }
}

void GincoTask::handle(Message& message) {
    switch (message.event<Event>()) {
        case Event::EVENT_PERIODIC: {
            ginco_dev_->periodic();
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

void GincoTask::setNewAddress(uint32_t id) {
    driver::config::setNvsKey(ConfigKey::DEVICE_ID, static_cast<uint8_t>(id));
    can_driver_.address(id);
    app::taskFinder().gpio().setDeviceId(id);
}