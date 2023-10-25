#include "config.hpp"

#include "nvs_flash.h"
#include "esp_log.h"

const static char* TAG = "config";
static nvs_handle_t my_handle;

using driver::ConfigDriver;

void ConfigDriver::init(){
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated and needs to be erased");
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    ESP_LOGI(TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
}

uint8_t ConfigDriver::getKey(const char* key){
    int8_t value {0};
    nvs_get_i8(my_handle, key, &value);
    return static_cast<uint8_t>(value);
}

void ConfigDriver::setKey(const char* key, uint8_t value){
    nvs_set_i8(my_handle, key, value);
    nvs_commit(my_handle);
}

char* ConfigDriver::getString(const char* key, size_t length){
    char* savedData = NULL;
    length = sizeof(savedData);
    nvs_get_str(my_handle, key, savedData, &length);
    length = sizeof(savedData);
    return savedData;
}

void ConfigDriver::setString(const char* key, const char* value){
    nvs_set_str(my_handle, key, value);
    nvs_commit(my_handle);
}

uint8_t ConfigDriver::deviceId()
{
    return getKey("device_id");
}

bool ConfigDriver::outputPwmMode(uint8_t id)
{
    char config_key[6];
    sprintf(config_key, "mode%c", id);
    return getKey(config_key) == 1;
}

uint8_t ConfigDriver::outputLevel(uint8_t id)
{
    char config_key[7];
    sprintf(config_key, "level%c", id);
    return getKey(config_key);
}