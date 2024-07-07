#pragma once

#include "esp_timer.hpp"

#include "sensor.hpp"

namespace ginco {

    class EventSensor: public BaseSensor<EventSensor> {
       private:
        /* Some constexpr static definitions */
        static constexpr uint32_t SAMPLES {16};
        static constexpr uint64_t PRESS_DELAY {static_cast<uint64_t>(0.4 * 1000 * 1000)};
        static constexpr uint64_t HOLD_DELAY {static_cast<uint64_t>(0.4 * 1000 * 1000)};

        /* Timer objects to handle press events */
        utils::EspTimer press_timer_;
        utils::EspTimer hold_timer_;

        /* State tracker variables */
        uint32_t state_counter_ {0};
        uint32_t current_press_;

        void stateChange(bool state);

        void pressEvent();
        void holdEvent();

       public:
        explicit EventSensor(const uint32_t& index);

        SensorType type() const { return SensorType::EVENT_INPUT; }

        void sensorUpdate();
    };
}  // namespace ginco