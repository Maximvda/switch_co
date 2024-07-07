#pragma once

#include <variant>
#include <vector>

#include "concurrent.hpp"
#include "standard_task.hpp"

#include "event_sensor.hpp"

namespace app {

    using SensorVariant = std::variant<ginco::EventSensor>;

    class GpioTask: public utils::StandardTask {
       private:
        enum class Event {
            SET_DEVICE_ID,
        };

        void handle(utils::Message&) override;
        void onStart() override;
        void onTimeout() override;

        utils::Milliseconds queueTimeout() override { return 10; };

        std::vector<SensorVariant> sensors_;

        void addSensor(const uint32_t& index, const ginco::SensorType& type);

       public:

        GpioTask(uint32_t priority): utils::StandardTask(priority) {}

        const char* name() const override { return "Gpio"; }

        bool setDeviceId(uint8_t id) { return post(Event::SET_DEVICE_ID, id); }

        auto& getSensors() const { return sensors_; }
    };

}  // namespace app
