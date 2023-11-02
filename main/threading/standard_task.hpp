#pragma once

#include <cstdint>
#include <memory>
#include <functional>
#include <variant>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include  "concurrent.hpp"
#include "supervised_task.hpp"
#include "message.hpp"

namespace utils
{
    /**
     * A StandardTask uses a FreeRtos Queue that has a fixed utils::Message as Queue item.
     *
     * The task is a message processing loop.
     * Messages originate from other tasks and from interrupt service routines.
     *
     * Messages from other tasks cannot directly use the message sending API (post),
     * but derived classes should provide a logical function per event type.
     *
     * For interfacing with ISR's we provide an IsrCallback object (see below)
     *
     * Note: Derived classes should not use event numbers greater than 0x7FFFFFFF.
     * Event codes with the highest bit set are reserved for the base class.
     *
     *
     */
    class StandardTask : public SupervisedTask {

    private:
        static constexpr uint32_t default_queue_size = 10;
        static constexpr Milliseconds default_queue_timeout = Milliseconds {1000};
        enum class MyEvents : uint32_t { inspect = 0x80000000U };
           Queue<Message> queue_;

        void process(Message& message);

    protected:

           /*
            * the timeout used for waiting on a message, subclasses can override
            */
           virtual Milliseconds queueTimeout() { return default_queue_timeout; }

        /*
         * The FreeRtos task has been created
         * perform additional initialization
         */
        virtual void onStart() {}
        /*
         *  no messsage has been received for queueTimeout() milliseconds
         */
        virtual void onTimeout() {}
        /*
         *  a message has been processed or a timeout on the queue.
         */
        virtual void tick() {}

        /**
         *     the task's main function
         */
        virtual void handle(Message&) = 0;

        /*
         * API to send messages to the queue. Five variants:
         *
         *  - task to task uint32 value
         *  - task to task void * value
         *  - task to task unique_ptr value
         *  - ISR to task uint32 value
         *  - ISR to task unique_ptr value
         */

        bool post(uint32_t event, uint32_t value = 0, uint32_t timeout = 0)
        {
            return queue_.tryPush(Message{event, value}, timeout);
        }

        template<typename T>
        bool post(uint32_t event, T * p, uint32_t timeout = 0)
        {
            return queue_.tryPush(Message {event, Message::Pointer{p}}, timeout);
        }

        template<typename T>
        bool post(uint32_t event, std::unique_ptr<T>&& p, uint32_t timeout = 0)
        {
            bool result = queue_.tryPush(Message {event , Message::UniquePointer{p.get()} }, timeout);
            if (result) {
                p.release();
            }
            return result;
        }

        bool postFromISR(uint32_t event, uint32_t value = 0)
        {
            return queue_.tryPushFromISR(Message {event, value});
        }

        bool postFromISR(uint32_t event, void * p)
        {
            return queue_.tryPushFromISR(Message {event, Message::Pointer{p}} );
        }

        [[noreturn]] virtual void run() final override;

    public:
        StandardTask(uint32_t priority, uint32_t queue_size = default_queue_size) : SupervisedTask(priority) , queue_(queue_size) {}

        virtual const QueueStats * queueStats() const final { return &(queue_.stats()); }

        /*
        * syncronuously runs operator() on the argument in the context of this task.
        * The calling task is suspended untill this task has performed the requested operation
        *
        * Typically used by diagnostic tools that need to collect information in a thread safe way.
        *
        */
        template<typename T>
        bool inspect(T&& inspector)
        {
            assert(!itsme());

            TaskHandle_t caller = xTaskGetCurrentTaskHandle();

            // should not be necessary, calling task should enter with clear notification state
            BaseType_t cleared = xTaskNotifyStateClear(caller);
            assert(!cleared);

            // wrap the inspector - probably a lambda but could be any c++ Functor (type with operator()) -
            // in other lambda that notifies the caller when inspection is over.
            //
            auto runnable = [&inspector, caller]() {
                inspector();
                xTaskNotifyGive(caller);
            };
            // wrap the runnable in a std::function to have a well known type that the receiving task can handle
            std::function<void(void)> func { runnable };

            // no need for unique_ptr, func stays in scope untill runnable is executed.
            if (post(static_cast<uint32_t>(MyEvents::inspect), &func)) {
                ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
                return true;
            } else {
                return false;
            }
        }

        /*
         * give access to postFromISR from ISRCallback
         */
        friend class IsrCallback;

    };

    /*
     * Helper class to give an ISR access to the task message queue
     *
     * Note: Pass by value to avoid dangling references/pointers
     */
    class IsrCallback {
        private:
            StandardTask * target_;
            uint32_t event_;

        public:
            // default constructor to allow use as subobject in peripheral class.
            // initialize the object to an "empty" state.
            IsrCallback() : target_ {nullptr} , event_ {0} {}

            IsrCallback(StandardTask * target, uint32_t event) : target_ {requireNonNull(target)} , event_{event} {};

            /*
             *  notify the task.
             *  using an empty a programming error (hence the assert).
             *  ISR's can test for emptyness using the the cast to bool operator.
             */
            bool operator()(uint32_t value = 0)
            {
                assert(target_);
                return target_ ? target_->postFromISR(event_, value) : false;
            }

            bool operator()(void * p)
            {
                assert(target_);
                return target_ ? target_->postFromISR(event_, p) : false;
            }

            explicit operator bool() { return target_; }
    };

}
