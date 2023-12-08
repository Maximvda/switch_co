#include "gpio_thread.hpp"

#include "esp_log.h"

using app::GpioTask;
using utils::Message;

const static char* TAG = "gpio thread";

void GpioTask::onStart()
{
    gpio_handler.init();
    io_expander_.init();
    ESP_LOGI(TAG, "started.");
}

void GpioTask::onTimeout()
{
    gpio_handler.gpio_driver_.inputCheck();
}

void GpioTask::handle(Message& message)
{
    switch (message.event()) {
    case EVENT_ADDRESS_UPDATE:
    {
        for (auto& input : gpio_handler.inputs_)
        {
            input.updateAddress();
        }
        break;
    }
    case EVENT_OUTPUT_SET:
    {
        if (auto value = message.uint32Value())
        {
            gpio_handler.outputs_[value].set();
        }
        break;
    }
    case EVENT_OUTPUT_CLEAR:
    {
        if (auto value = message.uint32Value())
        {
            gpio_handler.outputs_[value].clear();
        }
        break;
    }
    case EVENT_OUTPUT_TOGGLE:
    {
        if (auto value = message.uint32Value())
        {
            gpio_handler.outputs_[value].toggle();
        }
        break;
    }
    default:
        assert(0);
    }

}
