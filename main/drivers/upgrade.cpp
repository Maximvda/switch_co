#include "upgrade.hpp"

#include "esp_log.h"

#include "supervisor.hpp"

using driver::UpgradeHandler;
using data::GincoMessage;

const char * TAG = "Upgrade";

bool UpgradeHandler::init(GincoMessage& message)
{
    if (image_size_ != 0)
    {
        return false;
    }
    ota_partition_ = esp_ota_get_next_update_partition(NULL);
    if (ota_partition_ == NULL){
        ESP_LOGE(TAG, "Passive OTA partition not found");
        return false;
    }
    image_size_ = message.data<uint32_t>();
    if (image_size_ > ota_partition_->size){
        ESP_LOGE(TAG, "Image size too large");
        return false;
    }

    ESP_ERROR_CHECK(esp_ota_begin(ota_partition_, image_size_, &update_handle_));
    ESP_LOGI(TAG, "ota begin ok: size %llu", image_size_);
    return true;
};

bool UpgradeHandler::handle(GincoMessage& message)
{
    mes_received_++;
    image_size_ -=  message.length();
    if ((mes_received_ % 1000) == 0)
    {
        ESP_LOGI(TAG, "prog %lu | %llu", mes_received_, image_size_);
    }
    ESP_ERROR_CHECK(esp_ota_write(update_handle_, message.data<uint8_t*>(), message.length()));
    if (image_size_ == 0)
    {
        complete();
    }
    return true;
};

void UpgradeHandler::complete(){
    ESP_LOGI(TAG, "Transfer complete");
    ESP_ERROR_CHECK(esp_ota_end(update_handle_));
    if (!partitionValid())
        return;

    ESP_ERROR_CHECK(esp_ota_set_boot_partition(ota_partition_));
    esp_restart();
};

bool UpgradeHandler::partitionValid()
{
    esp_app_desc_t new_description;
    const esp_app_desc_t * current_description = esp_app_get_description();
    esp_err_t err = esp_ota_get_partition_description(ota_partition_, &new_description);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "ota get description returned %d ", err);
        return false;
    }
    if (strcmp(current_description->project_name, new_description.project_name)) {
        ESP_LOGW(TAG, "Invalid image name %s, should be %s", new_description.project_name, current_description->project_name);
        return false;
    }
    return true;
};

void UpgradeHandler::fail(){
    image_size_ = 0;
    if(update_handle_)
        esp_ota_end(update_handle_);
};
