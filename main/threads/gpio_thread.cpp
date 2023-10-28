#include "gpio_thread.hpp"

#include "esp_log.h"

using app::GpioTask;
using utils::Message;

const static char* TAG = "gpio thread";

void GpioTask::onStart()
{
    ESP_LOGI(TAG, "started.");
	gpio_handler.init();
}

void GpioTask::onTimeout()
{
	gpio_handler.gpio_driver_.inputCheck();
}

void GpioTask::handle(Message& message)
{
	switch (message.event()) {
	case EVENT_GPIO_TOGGLE:
	{
		if (auto value = message.takeValue<std::pair<uint8_t, bool>>())
		{
			gpio_handler.cbGpioChanged(value.get()->first, value.get()->second);
		}
		break;
	}
	default:
		assert(0);
	}

}
