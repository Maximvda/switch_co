#include "can.hpp"

#include "esp_log.h"

using driver::CanDriver;

const static char* TAG = "can driver";

void CanDriver::init(MessageCb cb_fnc){
    message_cb_ = cb_fnc;
    //Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        GPIO_NUM_5,
        GPIO_NUM_35,
        TWAI_MODE_NORMAL
    );
    g_config.rx_queue_len = 200;
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
    /* As long as frames are ready process them and aknowledge them! */
    while(twai_receive(&message, 50) == ESP_OK)
    {
        /* Frame received, always aknowledge! TODO: Optimise? */
        GincoMessage mes_data(message);
        auto ack = mes_data.acknowledge();
        transmit(ack); /* Aknowledge sent */
        /* Handle the message */
        message_cb_(mes_data);
    }
}

bool CanDriver::transmit(GincoMessage &can_mes, bool blocking)
{
    esp_err_t res = twai_transmit(&can_mes.canMessage(), 50);
    if (blocking && res == ESP_OK)
    {
        twai_message_t message;
        if(twai_receive(&message, 50) == ESP_OK)
        {
            auto compare = GincoMessage(message);
            return compare.isAcknowledge(can_mes);
        }
        ESP_LOGW(TAG, "Failed to receive acknowledge!");
        return false;
    }
    if (res != ESP_OK)
    {
        ESP_LOGW(TAG, "transmit failed %i", res);
    }
    return res == ESP_OK;
}

