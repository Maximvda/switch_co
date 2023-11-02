
#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <functional>



#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"

#include "concurrent.hpp"

namespace utils
{
    /*
     *  Abstract class for supervised tasks.
     *
     *  Supervised tasks should not terminate.
     *
     *  Basic responsibilities:
     *
     *  - Starting the FreeRtos task
     *  - Provide a wrapper for the FreeRtos task functions
     *  - Provide a sort of task watchdog infrastructure
     *
     *  Currently the Supervisor implementation uses a message queue on which Supervised Tasks frequently post a message.
     *  If any of the supervised tasks fail to do so, the Supervisor will perform a system watchdog reset.
     *  We use a function object to break the dependency between SuperVisor and SupervisedTask.
     *
     *  An ESP32 implementation could use the task watchdog support provided with ESP IDF.
     *
     *  SupervisedTask objects cannot be copied or moved.
     *
     *  Implementation note:
     *
     *  The task constructor is called immediately after the scheduler is started,
     *  but just before initialization of the Supervisor is finished.
     *  Constructors of taks objects and its subobjects should not use taskFinder(),
     *  but can safely use the FreeRtos API and perform dynamic allocation if needed.
     *
     */
    class SupervisedTask {
    public:
        using Watchdog = std::function<bool(SupervisedTask&)>;

    private:
        static constexpr uint32_t default_stack_size = 4096;

        TaskHandle_t handle_ = nullptr;
        const uint32_t priority_;
        Watchdog watchdog_;

    protected:
        /*
         * hit the task watchdog
         * current use by Supervisor call alive(*this) on Supervisor
         */
        bool sendAlive() { return watchdog_ ? watchdog_(*this) : false;    }

        /*
         * the body of the task
         */
        [[noreturn]] virtual void run() = 0;

    public:
        SupervisedTask(uint32_t priority) : priority_(priority) {};
        SupervisedTask(const SupervisedTask&) = delete;
        virtual ~SupervisedTask() = default;

        /**
         * Task name limited to (configMAX_TASK_NAME_LEN - 1) characters
         */
        virtual const char * name() const = 0;
        /**
         * Stack size in words (4 bytes on ARM Cortex Mx)
         * Note: Task size on ESP32 IDF is in bytes
         *
         * Derived classes can override the default value
         */
        virtual uint32_t stackSize() const { return default_stack_size; }

        /*
         * SupervisedTask does not use queue, but most derived classes do
         * To avoid down casting member is defined here
         */
        virtual const QueueStats * queueStats() const { return nullptr; }

        /*
         * this method is NOT intended as an alternative for vTaskDelay
         * it just uses the common wait/notify naming for inter thread communication
         *
         * very useful to wait on an interrupt handler's data ready notifcation after requesting an i/o operation
         * The timing of the two are independent, that is the notify can happen before or after the wait.
         *
         */
        bool wait(Milliseconds timeout = Milliseconds::maxDelay(), UBaseType_t slot = 0)
        {
            assert(!xPortInIsrContext());
            // do not call from other task
            assert(xTaskGetCurrentTaskHandle() == handle_);
            assert (slot < configTASK_NOTIFICATION_ARRAY_ENTRIES);

            return ulTaskNotifyTakeIndexed(slot, pdTRUE, timeout.toTicks());
        }

        bool notify(UBaseType_t slot = 0)
        {
            assert(!xPortInIsrContext());
            assert (slot < configTASK_NOTIFICATION_ARRAY_ENTRIES);

            return xTaskNotifyGiveIndexed(handle_, slot);
        }

        void notifyFromISR(UBaseType_t slot = 0)
        {
            assert(xPortInIsrContext());
            assert (slot < configTASK_NOTIFICATION_ARRAY_ENTRIES);

            BaseType_t yield;
            vTaskNotifyGiveIndexedFromISR(handle_, slot, &yield);
            portYIELD_FROM_ISR(yield);
        }

        // usefull for asserts
        bool itsme() {
            assert(!xPortInIsrContext());
            return xTaskGetCurrentTaskHandle() == handle_;
        }

        uint32_t priority() const { return priority_; }

        /**
         *  task startup sequence is start -> launch -> run
         */
        TaskHandle_t start(Watchdog watchdog)
        {
            watchdog_ = watchdog;
            assert(strlen(name()) < (configMAX_TASK_NAME_LEN - 1));
            bool success = xTaskCreate(launch, name(), stackSize(), this, priority_, &handle_);
            assert(success);
            (void)success;
            return handle_;
        }

        /**
         *  freertos xTaskCreate callback function that connect start to run
         */
        static void launch(void * p) { static_cast<SupervisedTask *>(requireNonNull(p))->run(); }
    };

}
