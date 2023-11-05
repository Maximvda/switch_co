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
    can_driver.init(
        [this](const GincoMessage& mes){
            this->handleCanMes(mes);
        }
    );
}

void CanTask::tick()
{
    can_driver.tick();
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
    if (message.function == Function::UPGRADE_FINISHED)
    {
        upgrade_handler_.fail();
        return;
    }
    app::taskFinder().ginco().frameReady(message);
}

void CanTask::handle(Message& message)
{
    switch (message.event()) {
    case EVENT_CAN_TRANSMIT:
    {
        if (auto mes = message.takeValue<GincoMessage>())
        {
            can_driver.transmit(*mes.get());
        }
        break;
    }
    default:
        assert(0);
    }

}