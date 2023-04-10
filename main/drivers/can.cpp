#include "can_driver.h"
#include "driver/twai.h"
#include "esp_log.h"

const char* TAG = "can driver";
static twai_message_t rec_message;

void driver::can::init(){
    //Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

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

bool driver::can::transmit(const uint32_t id, const uint8_t length, const uint8_t* data){
    //Configure message to transmit
    twai_message_t message;
    message.identifier = id;
    message.extd = 1; // Extended ID
    message.data_length_code = length;
    for (int i=0; i < length; i++) {
        message.data[i] = data[i];
    }

    //Queue message for transmission
    return (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK);
}

void driver::can::receive_task(void){
    while (1){
        twai_message_t message;
        if (twai_receive(&message, pdMS_TO_TICKS(10000)) == ESP_OK){
            rec_message = message;
            on_receive(rec_message);
        }
    }
}





