#pragma once

#include "actuator.hpp"

namespace ginco {

    class PwmActuator: public BaseActuator<PwmActuator> {
       public:
        explicit PwmActuator(const uint32_t& index): BaseActuator<PwmActuator>(index) {}

        ActuatorType type() const { return ActuatorType::PWM_OUTPUT; }

        void handleFunction(const ActuatorFunction& function, uint32_t data);
    };
}  // namespace ginco