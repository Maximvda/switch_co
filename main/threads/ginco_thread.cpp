#include "ginco_thread.hpp"

#include "esp_log.h"
#include "can.hpp"

using app::GincoTask;
using utils::Message;

const static char* TAG = "ginco thread";

void GincoTask::onStart()
{
    ESP_LOGI(TAG, "CAN thread started!!");
}

void GincoTask::onTimeout()
{
}

void GincoTask::handle(Message& message)
{
	switch (message.event()) {
	case EVENT_CAN_RECEIVED:
	{
		if (auto mes = message.takeValue<twai_message_t>())
		{
			/*TODO: Handle can frame */
		}
		break;
	}
	default:
		assert(0);
	}

}
