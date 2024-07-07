#pragma once

#include "actuator.hpp"

namespace ginco {

    class BoolActuator: public BaseActuator<BoolActuator> {
       private:
        bool active;

       public:
        explicit BoolActuator(const uint32_t& index);

        ActuatorType type() const { return ActuatorType::BOOL_OUTPUT; }

        void handleFunction(const ActuatorFunction& function, uint32_t data);
    };
}  // namespace ginco