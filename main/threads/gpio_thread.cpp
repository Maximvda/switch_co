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
    io_expander_.set(0, false);
}

void GpioTask::onTimeout()
{
    gpio_handler.gpio_driver_.inputCheck();
}

void GpioTask::handle(Message& message)
{
    switch (message.event()) {
    default:
        assert(0);
    }

}
