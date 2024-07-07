#include "bool_actuator.hpp"

#include "gpio.hpp"

using ginco::BoolActuator;

BoolActuator::BoolActuator(const uint32_t& index): BaseActuator<BoolActuator>(index) {
    active = driver::gpio::getOutputState(index);
}

void BoolActuator::handleFunction(const ActuatorFunction& function, uint32_t data) {
    switch (function) {
        case ActuatorFunction::SET_OUTPUT: {
            active = true;
            driver::gpio::setOutput(index(), true);
            break;
        }
        case ActuatorFunction::CLEAR_OUTPUT: {
            active = false;
            driver::gpio::setOutput(index(), false);
            break;
        }
        case ActuatorFunction::TOGGLE_OUTPUT: {
            active = !active;
            driver::gpio::setOutput(index(), active);
            break;
        }

        default:
            break;
    }
}
