#include "device.hpp"

/* esp includes */
#include "esp_log.h"
#include "esp_random.h"

#include "can.hpp"
#include "concurrent.hpp"
#include "supervisor.hpp"

using app::Device;
using data::ActionFunction;
using data::ConfigFunction;
using data::FeatureType;
using driver::ConfigKey;
using utils::Milliseconds;

static const char* TAG = {"Device"};

void Device::init() {
    rng_address_req_ = esp_random();
    id_ = config_.getKey<uint8_t>(ConfigKey::DEVICE_ID);
    ESP_LOGI(TAG, "id %u", id_);
    ginco_mes_.source(id_);
    const esp_app_desc_t* description = esp_app_get_description();
    char* str = (char*)description->version;
    char* end = (char*)description->version;
    for (uint32_t i = 0; i < 3; i++) {
        version_numbers_[i] = strtol(str, &end, 10);
        while (*end == '.') {
            end++;
        }
        str = end;
    }
}

void Device::periodic() {
    if (upgrade_handler_.upgrading()) return;
    if (id_ == 0) {
        requestNewId();
        return;
    }
    ginco_mes_.function(ConfigFunction::HEARTBEAT);
    /* encode the version number in the heartbeat */
    ginco_mes_.data<uint16_t>(static_cast<uint16_t>(version_numbers_[0]));
    ginco_mes_.data<uint16_t, true>(static_cast<uint16_t>(version_numbers_[1]), 1);
    ginco_mes_.data<uint32_t, true>(version_numbers_[2], 1);
    ginco_mes_.send();
}

void Device::requestNewId() {
    /* Random delay for when modules boot simultaniously */
    vTaskDelay(Milliseconds(esp_random() & 0xFF).toTicks());
    ginco_mes_.function(ConfigFunction::REQUEST_ADDRESS);
    rng_address_req_ = esp_random();
    ginco_mes_.data<uint32_t>(rng_address_req_);
    ginco_mes_.send();
    ESP_LOGI(TAG, "New id requested %lu", ginco_mes_.data<uint32_t>());
}

void Device::handleConfig(GincoMessage& message) {
    switch (message.function<ConfigFunction>()) {
        case ConfigFunction::SET_ADDRESS: {
            if (message.data<uint32_t>() == rng_address_req_) {
                id_ = message.data<uint32_t, true>(1);
                ginco_mes_.source(id_);
                config_.setKey(ConfigKey::DEVICE_ID, id_);
                /* Update can driver to only listen for this address */
                can_driver_.address(id_);
                app::taskFinder().gpio().updateAddress();
                ESP_LOGI(TAG, "received id %u", id_);
                message.ack();
            }
            break;
        }
        case ConfigFunction::RESET_ADDRESS: {
            id_ = 0;
            config_.setKey(ConfigKey::DEVICE_ID, static_cast<uint8_t>(0));
            break;
        }
        case ConfigFunction::UPGRADE: {
            upgrade_handler_.init(message);
            return;
        }
        case ConfigFunction::FW_IMAGE: {
            upgrade_handler_.handle(message);
            return;
        }
        case ConfigFunction::UPGRADE_FINISHED: {
            upgrade_handler_.fail();
            return;
        }
        default:
            break;
    }
}

void Device::handleAction(GincoMessage& message) {
    switch (message.function<ActionFunction>()) {
        case ActionFunction::OUTPUT_SET: {
            app::taskFinder().gpio().setOutput(message.data<uint8_t>());
            break;
        }
        case ActionFunction::OUTPUT_CLEAR: {
            app::taskFinder().gpio().clearOutput(message.data<uint8_t>());
            break;
        }
        case ActionFunction::OUTPUT_TOGGLE: {
            app::taskFinder().gpio().toggleOutput(message.data<uint8_t>());
            break;
        }
        default:
            break;
    }
}