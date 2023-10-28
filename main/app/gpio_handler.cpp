#include "gpio_handler.hpp"

using app::GpioHandler;

void GpioHandler::init()
{
    for(uint8_t i=0; i < driver::GpioDriver::TOTAL_GPIO; i++){
        OutputState state;
        inputs_[i] = Input(i, true);
        outputs_[i] = Output(i);
        inputs_[i].createTimers();
    }
    gpio_driver_.init();
    gpio_driver_.enableInputCheck(
        [this](uint8_t id, bool state){
            this->cbGpioChanged(id, state);
        }
    );
    for (uint8_t i=0; i < GpioDriver::TOTAL_GPIO; i++)
    {
        gpio_driver_.registerInterrupt(i, (void*)&inputs_[i]);
    }
}

void GpioHandler::cbGpioChanged(uint8_t id, bool value)
{
    inputs_[id].onToggle(value);
}