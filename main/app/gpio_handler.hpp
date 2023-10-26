#pragma once

#include "gpio.hpp"
#include "input.hpp"
#include "output.hpp"

using modules::Output;
using modules::Input;
using driver::GpioDriver;


namespace app
{
    /* Forward delcaration othwerise circular imports*/
    class GpioTask;

    class GpioHandler
    {
    private:
		GpioDriver gpio_driver_;
		Output outputs_[GpioDriver::total_gpio];
		Input inputs_[GpioDriver::total_gpio];

    public:
        void init();
        friend class GpioTask;

        void onPress(uint8_t id);

    };
} // namespace app