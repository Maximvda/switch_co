#include "ginco_thread.hpp"

#include "esp_log.h"

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
	default:
		assert(0);
	}

}
