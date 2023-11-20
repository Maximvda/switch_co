#include "config.hpp"

#include "nvs_flash.h"
#include "esp_log.h"

const static char* TAG = "config";

using driver::ConfigDriver;

ConfigDriver& ConfigDriver::instance()
{
    static ConfigDriver instance;
    return instance;
}

ConfigDriver::ConfigDriver(){
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    ESP_LOGI(TAG, "init start");
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated and needs to be erased");
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle_);
    ESP_LOGI(TAG, "handle opened");
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    /* Initialise unique strings for each key!! */
    // key_names_[static_cast<uint16_t>(ConfigKey::WIFI_CONFIGURED)] = "CONFIGURED";

    ESP_LOGI(TAG, "loading keys");
    /* Load all values from NVS */
    // getString(ConfigKey::MQTT_URL);
    ESP_LOGI(TAG, "keys loaded");
}

void ConfigDriver::setKey(const ConfigKey key, data_variant data)
{
    auto guard = m_.guard();
    /* Key should always be defined in our map*/
    assert(config_data_.find(key) != config_data_.end());
    if (std::holds_alternative<bool>(data))
    {
        config_data_[key] = data;
        setUint8(key, static_cast<uint8_t>(std::get<bool>(data)));
    }
    else if (std::holds_alternative<std::string>(data))
    {
        config_data_[key] = data;
        setString(key, std::get<std::string>(data));
    }
    else if (std::holds_alternative<std::uint8_t>(data))
    {
        config_data_[key] = data;
        setUint8(key, std::get<uint8_t>(data));
    }
}

void ConfigDriver::getString(const ConfigKey key)
{
    size_t required_size;
    const char* key_name = key_names_[static_cast<uint16_t>(key)].data();
    nvs_get_str(nvs_handle_, key_name, nullptr, &required_size);
    char data[required_size];
    nvs_get_str(nvs_handle_, key_name, data, &required_size);
    config_data_[key] = std::string(data);
}

void ConfigDriver::setString(const ConfigKey key, std::string value){
    const char* key_name = key_names_[static_cast<uint16_t>(key)].data();
    nvs_set_str(nvs_handle_, key_name, value.data());
    nvs_commit(nvs_handle_);
}

void ConfigDriver::getBool(const ConfigKey key)
{
    const char* key_name = key_names_[static_cast<uint16_t>(key)].data();
    uint8_t value {0};
    nvs_get_u8(nvs_handle_, key_name, &value);
    config_data_[key] = value == 1;
}

void ConfigDriver::setUint8(const ConfigKey key, uint8_t value){
    const char* key_name = key_names_[static_cast<uint16_t>(key)].data();
    nvs_set_i8(nvs_handle_, key_name, value);
    nvs_commit(nvs_handle_);
}

void ConfigDriver::getUint8(const ConfigKey key)
{
    const char* key_name = key_names_[static_cast<uint16_t>(key)].data();
    uint8_t value {0};
    nvs_get_u8(nvs_handle_, key_name, &value);
    config_data_[key] = value;
}