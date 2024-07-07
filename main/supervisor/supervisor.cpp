#include "supervisor.hpp"

#include <optional>
#include <variant>

#include "gpio.hpp"

namespace app {

    std::optional<Supervisor> Supervisor::instanceHolder;

    int32_t Supervisor::findIndex(SupervisedTask* task) {
        auto& tasks = task_list_.tasks();
        auto it = std::find(tasks.begin(), tasks.end(), task);
        if (it != tasks.end()) {
            return std::distance(tasks.begin(), it);
        } else {
            assert(0);
            return -1;
        }
    }

    /*
     * perform intialization that requires the scheduler to run, but before the other tasks are started
     */
    void Supervisor::initialize() { /* Initialise the gpio pins */ driver::gpio::initGpio(); }

    void Supervisor::run() {
        initialize();
        constexpr TickType_t max_ticks = Milliseconds(9500).toTicks();
        constexpr Milliseconds timeout = 500;

        uint32_t all_alive = (1 << startTasks()) - 1;
        uint32_t alive = 0;
        uint32_t last_tick = 0;

        for (;;) {
            if (auto maybe = queue_.tryPop(timeout)) {
                uint32_t index = findIndex(*maybe);
                alive |= (1 << index);
                if (alive == all_alive) {
                    last_tick = xTaskGetTickCount();
                    alive = 0;
                }
            }

            if (xTaskGetTickCount() - last_tick < max_ticks) {
            } else {
                // assert(0);
            }
        }
    }

    uint32_t Supervisor::startTasks() {
        assert(task_list_.tasks().size() > 0);
        assert(task_list_.tasks().size() <= 32);
        uint32_t id = 0;
        for (auto& each : task_list_.tasks()) {
            if (each->start([this](SupervisedTask& task) { return alive(task); })) {
                ++id;
            } else {
                // task failed to return a taskHandle
                assert(0);
            }
        }
        return id;
    }

};  // namespace app
