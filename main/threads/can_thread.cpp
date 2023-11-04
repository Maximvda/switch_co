#include "can_thread.hpp"

#include "esp_log.h"
#include "supervisor.hpp"

using namespace app;
using utils::Message;
using data::Function;

const static char* TAG = "can thread";

void CanTask::onStart()
{
    ESP_LOGI(TAG, "started.");
    can_driver_.init(
        [this](const GincoMessage& mes){
            this->handleCanMes(mes);
        }
    );
}

void CanTask::tick()
{
    can_driver_.tick();
}

void CanTask::handleCanMes(const GincoMessage& message)
{
    /*TODO: Handle can frame */
    /*TODO: CHECK IF FRAME IS FOR THIS DEVICE OR DIFFERENT DEVICE */
    if (message.function == Function::UPGRADE)
    {
        upgrade_handler_.init(message);
        return;
    }
    if (message.function == Function::FW_IMAGE)
    {
        upgrade_handler_.handle(message);
        return;
    }
    app::taskFinder().ginco().frameReady(message);
}

void CanTask::handle(Message& message)
{
    switch (message.event()) {
    case EVENT_CAN_RECEIVED:
    {
        if (auto mes = message.takeValue<GincoMessage>())
        {

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
    case EVENT_CAN_TICK:
    {
        can_driver_.tick();
        break;
    }
    default:
        assert(0);
    }

}