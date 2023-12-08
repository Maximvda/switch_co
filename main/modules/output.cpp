#include "output.hpp"

#include "esp_log.h"

const static char* TAG = {"Output"};

using modules::Output;

Output::Output(const uint8_t id, GpioDriver& driver) : driver_(driver){
    id_ = id;
    state_.high = driver_.getOutputLevel(id);
}

void Output::handleMessage(GincoMessage& message){
}

void Output::runEffect(output::Effect effect){
    ESP_LOGW(TAG, "effects still to be implemented");
}