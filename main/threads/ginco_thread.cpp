#include "ginco_thread.hpp"

/* esp includes */
#include "esp_log.h"
#include "esp_timer.h"

/* ginco includes */
#include "supervisor.hpp"
#include "can.hpp"

using app::GincoTask;
using utils::Message;
using data::FeatureType;
using utils::Milliseconds;

const static char* TAG = "ginco thread";

static void cbTimer(TimerHandle_t xTimer)
{
    app::taskFinder().ginco().secondEvent();
}

void GincoTask::onStart()
{
    timer_ =  xTimerCreate("t_second_", Milliseconds(10000).toTicks(), pdTRUE, nullptr, cbTimer);
    ginco_dev_.init();
    ESP_LOGI(TAG, "started.");
}

void GincoTask::tick()
{
}

void GincoTask::handle(Message& message)
{
    switch (message.event()) {
    case EVENT_CAN_RECEIVED:
    {
        /*TODO: handle can messages */
        if (auto mes = message.takeValue<GincoMessage>())
        {
            auto& data = *mes.get();
            switch (data.feature())
            {
            case FeatureType::CONFIG:
            {
                ginco_dev_.handleConfig(data);
                break;
            }
            case FeatureType::LIGHT:
            {

                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case EVENT_SECOND:
    {
        ginco_dev_.secondTick();
        break;
    }
    case EVENT_CAN_READY:
    {
        xTimerStart(timer_, 0);
        break;
    }
    default:
        assert(0);
    }

}
