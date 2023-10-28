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
		Input inputs_[GpioDriver::TOTAL_GPIO] =
    {
      Input(0, true),
      Input(1, true),
      Input(2, true),
      Input(3, true),
      Input(4, true),
      Input(5, true),
      Input(6, true),
    };

    public:
        void init();
        friend class GpioTask;

        void cbGpioChanged(uint8_t id, bool value);

    };
} // namespace app