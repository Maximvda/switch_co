#pragma once

#include <cstdint>
#include <optional>
#include <variant>

#include "tasklist.hpp"
#include "concurrent.hpp"
#include "standard_task.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


using utils::Queue;
using utils::Milliseconds;

namespace app
{
    class Supervisor {
    private:

        static constexpr uint32_t queue_size = 10;
        static std::optional<Supervisor> instanceHolder;
        Queue<SupervisedTask *> queue_ {queue_size};
        TaskList task_list_;

        static void launch(void *) { instanceHolder.emplace().run(); }

        int32_t findIndex(SupervisedTask * task);
        /*
         * perform intialization that requires the scheduler to run, but before the other tasks are started
         */
        void initialize();

        [[noreturn]] void run();

        /*
         * returns the number of started tasks
         */
        uint32_t startTasks();

        // call back for SupervisedTasks
        // private as we wrap it in a fuction object
        bool alive(SupervisedTask& task) { return queue_.tryPush(&task); }

    public:

        static void createTask()
        {
            constexpr uint32_t stack_size = 1024;
            auto result = xTaskCreate(launch, "manager", stack_size, nullptr, configMAX_PRIORITIES - 1, nullptr);
            assert(result);
            (void)result;
        }

        static Supervisor& instance()
        {
            // if this assert fails you probably have used taskFinder in the constructor initialization of the taskList.
            assert(instanceHolder);
            return *instanceHolder;
        }

        TaskList& taskList() { return task_list_; };

    };

    //
    // utility function outside thee class name scope to avoid calling static method on Supervisor directly.
    // we might make Supervisor a template class to remove the dependency on TaskList
    // and move it to namespace util.
    // So we don't want the Supervisor name spread all over the code base.
    //

    inline TaskList& taskFinder()
    {
        return Supervisor::instance().taskList();
    }

}
