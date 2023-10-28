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
		Output outputs_[GpioDriver::TOTAL_GPIO];
		Input inputs_[GpioDriver::TOTAL_GPIO];

    public:
        void init();
        friend class GpioTask;

        void cbGpioChanged(uint8_t id, bool value);

    };
} // namespace app