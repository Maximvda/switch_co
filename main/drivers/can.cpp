#include "can.hpp"

#include "esp_log.h"

using driver::can::CanDriver;

const static char* TAG = "can driver";

void CanDriver::init(){
    //Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        GPIO_NUM_5,
        GPIO_NUM_35,
        TWAI_MODE_NORMAL
    );
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    f_config.acceptance_code = 0x0;
    f_config.acceptance_mask = 0x3ffffff; // Don't accept events

    g_config.intr_flags = ESP_INTR_FLAG_IRAM;
    //g_config.alerts_enabled = TWAI_ALERT_ALL;

    //Install CAN driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        ESP_LOGI(TAG, "installed");
    } else {
        ESP_LOGI(TAG, "installation failed.");
        return;
    }

    //Start CAN driver
    if (twai_start() == ESP_OK) {
        ESP_LOGI(TAG, "started.");
        return;
    }
    ESP_LOGI(TAG, "failure starting");
}

void CanDriver::tick()
{
    twai_message_t message;
    if (!twai_receive(&message, 0) == ESP_OK)
    {
        /* No frame received */
        return;
    }
    /* Frame received so send to correct place */
    //TODO: THIS SHIT
}

bool CanDriver::transmit(const twai_message_t& can_mes){
    //esp_err_t res = twai_read_alerts(&alert, pdMS_TO_TICKS(5000));
    //ESP_LOGI(TAG, "Alert %lu from transmit %d", alert, res);
    esp_err_t res = twai_transmit(&can_mes, 0);
    ESP_LOGI(TAG, "Response from transmit %d", res);
    return res == ESP_OK;
}



