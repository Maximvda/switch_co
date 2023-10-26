#include "can_thread.hpp"

#include "esp_log.h"

using namespace app;
using utils::Message;

const static char* TAG = "can thread";

void CanTask::onStart()
{
    ESP_LOGI(TAG, "started.");
	can_driver_.init();
}

void CanTask::onTimeout()
{
	can_driver_.tick();
}

void CanTask::handle(Message& message)
{
	switch (message.event()) {
	case EVENT_CAN_TRANSMIT:
	{
		if(auto mes = message.takeValue<twai_message_t>())
		{
			can_driver_.transmit(*mes.get());
		}
		break;
	}
	default:
		assert(0);
	}

}
