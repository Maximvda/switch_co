#include "device.hpp"

/* esp includes */
#include "esp_log.h"
#include "esp_random.h"

#include "can.hpp"
#include "concurrent.hpp"
#include "config.hpp"
#include "supervisor.hpp"

using app::Device;
using driver::ConfigKey;
using ginco::ActuatorFunction;
using ginco::ConfigFunction;
using ginco::GincoMessage;
using utils::Milliseconds;

static const char* TAG = {"Device"};

void Device::init(std::function<void(uint32_t)> cb_new_address) {
    cb_new_address_ = cb_new_address;
    rng_address_req_ = esp_random();
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

    const auto actuators =
        driver::config::loadVector<std::tuple<uint32_t, ginco::ActuatorType>>(driver::ConfigKey::ACTUATOR_LIST);
    for (const auto& [index, type] : actuators) {
        addActuator(index, type);
    }
    ESP_LOGI(TAG, "with id %u ready", id_);
}

void Device::periodic() {
    if (upgrade_handler_.upgrading()) return;
    if (id_ == 0) {
        requestNewId();
        return;
    }
    ginco::GincoMessage ginco_mes(id_);
    ginco_mes.function(ConfigFunction::HEARTBEAT);
    /* encode the version number in the heartbeat */
    ginco_mes.data<uint16_t>(static_cast<uint16_t>(version_numbers_[0]));
    ginco_mes.data<uint16_t, true>(static_cast<uint16_t>(version_numbers_[1]), 1);
    ginco_mes.data<uint32_t, true>(version_numbers_[2], 1);
    ginco_mes.send();
}

void Device::requestNewId() {
    /* Random delay for when modules boot simultaniously */
    vTaskDelay(Milliseconds(esp_random() & 0xFF).toTicks());
    ginco::GincoMessage ginco_mes(id_);
    ginco_mes.function(ConfigFunction::REQUEST_ADDRESS);
    rng_address_req_ = esp_random();
    ginco_mes.data<uint32_t>(rng_address_req_);
    ginco_mes.send();
    ESP_LOGI(TAG, "New id requested %lu", ginco_mes.data<uint32_t>());
}

void Device::handleConfig(GincoMessage& message) {
    switch (message.function<ConfigFunction>()) {
        case ConfigFunction::SET_ADDRESS: {
            if (message.data<uint32_t>() == rng_address_req_) {
                id_ = message.data<uint32_t, true>(1);
                cb_new_address_(id_);
                /* Update can driver to only listen for this address */
                ESP_LOGI(TAG, "received id %u", id_);
                message.acknowledge();
            }
            break;
        }
        case ConfigFunction::RESET_ADDRESS: {
            id_ = 0;
            break;
        }
        case ConfigFunction::UPGRADE: {
            upgrade_handler_.init(message);
            break;
        }
        case ConfigFunction::FW_IMAGE: {
            upgrade_handler_.handle(message);
            break;
        }
        case ConfigFunction::UPGRADE_FINISHED: {
            upgrade_handler_.fail();
            break;
        }
        case ConfigFunction::REQUEST_ACTUATORS: {
            for (auto& act_variant : actuators_) {
                std::visit(
                    [&message](auto&& actuator) {
                        message.index(actuator.index());
                        message.data(actuator.type());
                    },
                    act_variant
                );
                message.send();
            }
            break;
        }
        case ConfigFunction::ADD_ACTUATOR:
            addActuator(message.index(), message.data<ginco::ActuatorType>());
            driver::config::storeVector(ConfigKey::ACTUATOR_LIST, actuators_);
            break;

        case ConfigFunction::REQUEST_SENSORS:
            app::taskFinder().gpio().inspect([&message]() {
                auto& sensors = app::taskFinder().gpio().getSensors();
                for (auto& sensor_variant : sensors) {
                    std::visit(
                        [&message](auto&& sensor) {
                            message.index(sensor.index());
                            message.data(sensor.type());
                        },
                        sensor_variant
                    );
                    message.send();
                }
            });
            break;

        case ConfigFunction::ADD_SENSOR:
            app::taskFinder().gpio().configureSensor(message.index(), message.data<ginco::SensorType>());
            break;

        default:
            break;
    }
}

void Device::handleActuator(GincoMessage& message) {
    auto act_it = std::find_if(actuators_.begin(), actuators_.end(), [idx = message.index()](const auto& obj) {
        return obj.index() == idx;
    });
    /* actuator wasn't found */
    if (act_it == actuators_.end()) {
        ESP_LOGW(TAG, "Actuator:%u not found.", message.index());
        return;
    }

    std::visit(
        [&](auto&& arg) { arg.handleFunction(message.function<ActuatorFunction>(), message.data<uint32_t>()); }, *act_it
    );
}

void Device::addActuator(const uint32_t& index, const ginco::ActuatorType& type) {
    if (type == ginco::ActuatorType::BOOL_OUTPUT) {
        actuators_.emplace_back(ginco::BoolActuator(index));
    } else if (type == ginco::ActuatorType::PWM_OUTPUT) {
        actuators_.emplace_back(ginco::BoolActuator(index));
    }
    ESP_LOGI(TAG, "Added actuator: %lu as type %lu", index, static_cast<uint32_t>(type));
}