#include "can_thread.hpp"

#include "esp_log.h"
#include "ginco_types.hpp"
#include "supervisor.hpp"

using namespace app;
using utils::Message;
using data::GincoMessage;
using data::Function;

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
    case EVENT_CAN_RECEIVED:
    {
        if (auto mes = message.takeValue<GincoMessage>())
        {
            /*TODO: Handle can frame */
            /*TODO: CHECK IF FRAME IS FOR THIS DEVICE OR DIFFERENT DEVICE */
            auto message = mes.get();
            if (message->function == Function::UPGRADE)
            {
                upgrade_handler_.init(*message);
                return;
            }
            if (message->function == Function::FW_IMAGE)
            {
                upgrade_handler_.handle(*message);
                return;
            }
            app::taskFinder().ginco().frameReady(*message);
        }
        break;
    }
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
