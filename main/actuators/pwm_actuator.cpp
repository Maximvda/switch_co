#include "pwm_actuator.hpp"

#include "supervisor.hpp"

using ginco::PwmActuator;

void PwmActuator::handleFunction(const ActuatorFunction& function, uint32_t data) {
    switch (function) {
        case ActuatorFunction::SET_DUTY_CYCLE:
            /*TODO: FIXME*/
            break;

        default:
            break;
    }
}