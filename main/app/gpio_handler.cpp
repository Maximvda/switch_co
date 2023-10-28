#include "gpio_handler.hpp"

#include "supervisor.hpp"

using app::GpioHandler;

void GpioHandler::init()
{
    for(uint8_t i=0; i < driver::GpioDriver::TOTAL_GPIO; i++){
        OutputState state;
        // inputs_[i] = Input(i, true);
        outputs_[i] = Output(i);
        inputs_[i].createTimers();
    }
    gpio_driver_.init();
    gpio_driver_.enableInputCheck(
        // [](uint8_t id, bool state){app::taskFinder().gpio().gpioToggle(id, state);});
        [this](uint8_t id, bool state){
            this->cbGpioChanged(id, state);
        }
    );
}

void GpioHandler::cbGpioChanged(uint8_t id, bool value)
{
    inputs_[id].onToggle(value);
}