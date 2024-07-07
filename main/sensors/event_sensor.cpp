#include "event_sensor.hpp"

#include "ginco_message.hpp"
#include "gpio.hpp"

using ginco::EventSensor;

EventSensor::EventSensor(const uint32_t& index): BaseSensor<EventSensor>(index) {
    /* link the callback functions */
    press_timer_.callback_fn = [this]() { this->pressEvent(); };
    hold_timer_.callback_fn = [this]() { this->holdEvent(); };
}

void EventSensor::sensorUpdate() {
    bool state = driver::gpio::getInputState(index());
    // value counts from 0 to samples_ for an inactive to active transition. A 0 resets value.
    // and from 2*samples_ to samples_ for an active to inactive transition. A 1 resets value.
    state_counter_ = state ? (state_counter_ < SAMPLES ? state_counter_ + 1 : 2 * SAMPLES) :
                             (state_counter_ > SAMPLES ? state_counter_ - 1 : 0);
    if (state_counter_ == SAMPLES) {
        stateChange(state);
        state_counter_ = state ? 2 * SAMPLES : 0;
    }
}

void EventSensor::stateChange(bool state) {
    current_press_++;
    if (current_press_ % 2 == 0) {
        press_timer_.stop();
    }

    press_timer_.start(PRESS_DELAY);
    hold_timer_.restartPeriodic(HOLD_DELAY);
}

void EventSensor::pressEvent() {
    /* Only handle events when button press is finished (push down and up). */
    if (current_press_ % 2 == 0) {
        ginco::GincoMessage message(deviceId());
        message.index(index());

        switch (current_press_) {
            case 2:
                message.function(SensorFunction::BUTTON_PRESS);
                break;
            case 4:
                message.function(SensorFunction::BUTTON_DOUBLE_PRESS);
                break;
            case 6:
                message.function(SensorFunction::BUTTON_TRIPPLE_PRESS);
                break;
            default:
                break;
        }
        message.send();
    }
    current_press_ = 0;  // Reset the press count
}

void EventSensor::holdEvent() {
    ginco::GincoMessage message(deviceId());
    message.index(index());
    message.function(SensorFunction::BUTTON_HOLD);
    message.send();
}