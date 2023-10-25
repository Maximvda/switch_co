#include "gpio_thread.hpp"

#include "esp_log.h"

using app::GpioTask;
using utils::Message;

const static char* TAG = "gpio thread";

void GpioTask::onStart()
{
    ESP_LOGI(TAG, "CAN thread started!!");
}

void GpioTask::onTimeout()
{
}

void GpioTask::handle(Message& message)
{
	switch (message.event()) {
	case EVENT_GPIO_TOGGLE:
	{

		break;
	}
	default:
		assert(0);
	}

}
