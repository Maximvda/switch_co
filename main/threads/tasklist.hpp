#pragma once

#include "ginco_thread.hpp"
#include "gpio_thread.hpp"
#include "standard_task.hpp"

using utils::SupervisedTask;

namespace app {

    class TaskList {
       private:
        app::GpioTask gpio_ {1};
        app::GincoTask ginco_ {2};

        const std::vector<SupervisedTask*> tasks_ {&gpio_, &ginco_};

        TaskList() = default;

       public:
        // only Supervisor can create the TaskList
        friend class Supervisor;

        TaskList(const TaskList&) = delete;

        auto& tasks() { return tasks_; }

        auto& gpio() { return gpio_; }

        auto& ginco() { return ginco_; }
    };
}  // namespace app
