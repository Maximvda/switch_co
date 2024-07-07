#pragma once

#include <variant>

/* ginco includes */
#include "bool_actuator.hpp"
#include "can.hpp"
#include "config.hpp"
#include "ginco_message.hpp"
#include "sensor.hpp"
#include "upgrade.hpp"

namespace app {

    using ActuatorVariant = std::variant<ginco::BoolActuator>;

    class Device {
       private:
        uint8_t id_;
        uint32_t rng_address_req_;
        driver::UpgradeHandler upgrade_handler_;
        uint32_t version_numbers_[3];
        std::vector<ActuatorVariant> actuators_;
        std::function<void(uint32_t)> cb_new_address_;

        void requestNewId();
        void addActuator(const uint32_t& index, const ginco::ActuatorType& type);

       public:
        explicit Device(uint8_t id): id_(id) {}

        void init(std::function<void(uint32_t)> cb_new_address);
        void periodic();
        void handleConfig(ginco::GincoMessage& message);
        void handleActuator(ginco::GincoMessage& message);

        uint8_t id() const { return id_; }
    };

}  // namespace app
