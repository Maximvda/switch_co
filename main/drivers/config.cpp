#include "config.h"
#include "nvs_flash.h"
#include "esp_log.h"

const static char* TAG = "config";
static nvs_handle_t my_handle;

void config::init(){
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

uint8_t config::get_key(const char* key){
    int8_t value {0};
    nvs_get_i8(my_handle, key, &value);
    return static_cast<uint8_t>(value);
}

void config::set_key(const char* key, uint8_t value){
    nvs_set_i8(my_handle, key, value);
    nvs_commit(my_handle);
}