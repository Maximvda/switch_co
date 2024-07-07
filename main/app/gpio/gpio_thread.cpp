#include "gpio_thread.hpp"

#include "esp_log.h"

#include "bool_actuator.hpp"
#include "config.hpp"
#include "gpio.hpp"

using app::GpioTask;
using utils::Message;

const static char* TAG = "gpio thread";

void GpioTask::onStart() {
    const auto sensor_list =
        driver::config::loadVector<std::tuple<uint32_t, ginco::SensorType>>(driver::ConfigKey::SENSOR_LIST);
    for (const auto& [index, type] : sensor_list) {
        addSensor(index, type);
    }
}

void GpioTask::onTimeout() {
    /* Iterate over the sensors and perform their update function. */
    for (auto& sensor_variant : sensors_) {
        std::visit([](auto&& sensor) { sensor.sensorUpdate(); }, sensor_variant);
    }
}

void GpioTask::handle(Message& message) {
    switch (message.event<Event>()) {
        case Event::SET_DEVICE_ID: {
            if (auto value = message.uint32Value()) {
                for (auto& sensor_variant : sensors_) {
                    std::visit([&value](auto&& sensor) { sensor.deviceId(value); }, sensor_variant);
                }
            }
            break;
        }
        case Event::CONFIGURE_SENSOR: {
            if (auto value = message.takeValue<std::tuple<uint32_t, ginco::SensorType>>()) {
                const auto& [index, type] = *value.get();
                addSensor(index, type);
                driver::config::storeVector(driver::ConfigKey::SENSOR_LIST, sensors_);
            }
        }
        default:
            assert(0);
    }
}

void GpioTask::addSensor(const uint32_t& index, const ginco::SensorType& type) {
    if (type == ginco::SensorType::EVENT_INPUT) {
        sensors_.emplace_back(ginco::EventSensor(index));
    } else if (type == ginco::SensorType::BINARY_INPUT) {
        /* TODO */
    }
    ESP_LOGI(TAG, "Added sensor: %lu as type %lu", index, static_cast<uint32_t>(type));
}