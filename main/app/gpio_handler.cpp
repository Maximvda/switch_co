#include "gpio_handler.hpp"

using app::GpioHandler;

void GpioHandler::init()
{
    for(uint8_t i=0; i < driver::GpioDriver::total_gpio; i++){
        OutputState state;
        inputs_[i] = Input(i, true);
        outputs_[i] = Output(i);
        inputs_[i].createTimers();
    }
    gpio_driver_.init();
    gpio_driver_.enableInputCheck(
        [this](uint8_t id, bool state){
            this->onPress(id);
        }
    );
}

void GpioHandler::onPress(uint8_t id)
{
    inputs_[id].onToggle();
}