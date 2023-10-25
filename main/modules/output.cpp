#include "output.hpp"

#include "esp_log.h"

const static char* TAG = {"Output"};

using modules::Output;
using data::Function;

Output::Output(const uint8_t id, const OutputState state){
    id_ = id;
    state_ = state;
    /*TODO: SET GPIO OUTPUT THROUGH THREAD */
}

void Output::handleMessage(GincoMessage& message){
    switch(message.function){
        case Function::TURN_OFF:
            // set_state(false);
            break;
        case Function::TURN_ON:
        {
            /*TODO: Turn on !!*/
            break;
        }
        case Function::TOGGLE:
            // toggle();
            break;
        case Function::EFFECT:
        {
            runEffect(static_cast<output::Effect>(message.data));
            break;
        }
        case Function::PAUSE_EFFECT:
            break;
        case Function::REQUEST_STATE:
            break;
        default:
            break;
    }
    // Always aknowledge and return state
    message.acknowledge();
}

void Output::runEffect(output::Effect effect){
    ESP_LOGW(TAG, "effects still to be implemented");
}