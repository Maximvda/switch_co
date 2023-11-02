#include "ginco_thread.hpp"

#include "esp_log.h"
#include "can.hpp"
#include "esp_timer.h"

#include "supervisor.hpp"

using app::GincoTask;
using utils::Message;

const static char* TAG = "ginco thread";

void GincoTask::onStart()
{
    ESP_LOGI(TAG, "started.");
}

void GincoTask::onTimeout()
{
    ginco_dev_.init();
}

void GincoTask::handle(Message& message)
{
    switch (message.event()) {
    default:
        assert(0);
    }

}
