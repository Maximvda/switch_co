#include "config.hpp"

#include "esp_log.h"
#include "nvs_flash.h"

const static char* TAG = "config";

using driver::ConfigDriver;

ConfigDriver& ConfigDriver::instance() {
    static ConfigDriver instance;
    return instance;
}

ConfigDriver::ConfigDriver() {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    ESP_LOGI(TAG, "init start");
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated and needs to be erased");
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle_);
    ESP_LOGI(TAG, "handle opened");
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    /* Initialise unique strings for each key!! */
    key_names_[static_cast<uint32_t>(ConfigKey::DEVICE_ID)] = "DEVICE_ID";

    ESP_LOGI(TAG, "loading keys");
    /* Load all values from NVS */
    get<uint8_t>(ConfigKey::DEVICE_ID);
    ESP_LOGI(TAG, "keys loaded");
}

void ConfigDriver::setKey(const ConfigKey key, data_variant data) {
    auto guard = m_.guard();
    /* Key should always be defined in our map*/
    assert(config_data_.find(key) != config_data_.end());
    if (std::holds_alternative<bool>(data)) {
        config_data_[key] = data;
        set<uint8_t>(key, static_cast<uint8_t>(std::get<bool>(data)));
    } else if (std::holds_alternative<std::string>(data)) {
        config_data_[key] = data;
        set<std::string>(key, std::get<std::string>(data));
    } else if (std::holds_alternative<std::uint8_t>(data)) {
        config_data_[key] = data;
        set<uint8_t>(key, std::get<uint8_t>(data));
    }
}