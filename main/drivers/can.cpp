#include "can.hpp"

/* esp includes */
#include "esp_log.h"

/* ginco includes */
#include "config.hpp"

using driver::CanDriver;
using driver::ConfigKey;
using ginco::GincoMessage;

const static char* TAG = "can driver";

void CanDriver::init(uint8_t id, MessageCb cb_fnc) {
    id_ = id;
    message_cb_ = cb_fnc;
    start();
}

void CanDriver::start() {
    // Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_35, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = {
        .acceptance_code = static_cast<uint32_t>(id_ << 18),
        .acceptance_mask = 0x3ffff,
        .single_filter = false
    };

    g_config.intr_flags = ESP_INTR_FLAG_IRAM;
    g_config.rx_queue_len = 25;

    // Install CAN driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) ESP_LOGI(TAG, "installed");

    // Start CAN driver
    if (twai_start() == ESP_OK) ESP_LOGI(TAG, "started.");
}

void CanDriver::address(uint8_t id) {
    id_ = id;
    twai_stop();
    twai_driver_uninstall();
    start();
}

void CanDriver::tick() {
    twai_message_t message;
    /* As long as frames are ready process them and aknowledge them! */
    while (twai_receive(&message, 10) == ESP_OK) {
        /* Handle the message */
        message_cb_(GincoMessage(message));
    }
}

bool CanDriver::transmit(GincoMessage& can_mes) {
    esp_err_t res = twai_transmit(&can_mes.message(), 50);
    if (res != ESP_OK) {
        ESP_LOGW(TAG, "transmit failed %i", res);
    }
    /* Restart CAN controller when bus errors */
    if (res == ESP_ERR_INVALID_STATE) {
        address(id_);
    }
    return res == ESP_OK;
}
