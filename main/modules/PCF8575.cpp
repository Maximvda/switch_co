#include "PCF8575.hpp"

#include "esp_log.h"

using component::PCF8575;

void PCF8575::init() { driver_.init(400000); }

void PCF8575::set(uint8_t pin, bool value) {
    if (value) {
        io_map_ |= 1 << pin;
    } else {
        io_map_ &= ~(1 << pin);
    }
    driver_.write(0x20, reinterpret_cast<uint8_t*>(&io_map_), sizeof(io_map_));
}

bool PCF8575::get(uint8_t pin) {
    uint8_t buffer[2];
    driver_.read(0x20, buffer, sizeof(buffer));
    uint16_t data = buffer[0] | (buffer[1] << 8);
    ESP_LOGI("PCF", "data %u", data);
    return data & (1 << pin);
}