#include "output.hpp"

#include "esp_log.h"

const static char* TAG = {"Output"};

using modules::Output;

Output::Output(const uint8_t id){
    id_ = id;
    /*TODO: Get state */
    /*TODO: SET GPIO OUTPUT THROUGH THREAD */
}

void Output::handleMessage(GincoMessage& message){
}

void Output::runEffect(output::Effect effect){
    ESP_LOGW(TAG, "effects still to be implemented");
}