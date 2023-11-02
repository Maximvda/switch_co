/**
 *   Contains several concurrent utilities in a Freeertos environment.
 *   Note that the standard cpp tools like std::mutex cannot be used by lack of a pthreads implementation.
 *   There is a Freertos pthreads project, but nor sure about its quality. The ESP32 IDF library also has a pthreads implementation.
 *
 *   This file provides:
 *
 *   Lock guard support for Freertos for Mutex and RecursiveMutex
 *   A bounded blocking concurrent queue backed by a ringbuffer.
 */

#pragma once

#include <cstdint>
#include <cassert>
#include <optional>
#include <type_traits>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"

#include "stats.hpp"

namespace utils {

    /**
     *  "fluent" assert, only use with simple objects (pointers, ints or bool) because of value semantics.
     */
    template <typename T>
    T requireNonNull(T p)
    {
        static_assert(std::is_pointer_v<T> || std::is_integral_v<T>, "Only use on pointers or integral types");
        assert(p);
        return p;
    }

    /*
     *  Millisecond to FreeRtos tick conversion
     *
     *  Current tick rates are:
     *    100 Hz on ESP32 (10 ms)
     *
     *  Remember that actual wait time will vary between (toTicks() - 1) and toTicks().
     *
     */
    struct Milliseconds {
        static constexpr uint32_t maxDelay_ = -1U;
        uint32_t ms;
        constexpr TickType_t toTicks() { return ms == maxDelay_ ? portMAX_DELAY : pdMS_TO_TICKS(ms); }
        constexpr Milliseconds(uint32_t ms) : ms(ms) {};
        static constexpr Milliseconds maxDelay() { return Milliseconds {maxDelay_} ; }
    };

    /*
     *  Queue related stats
     */
    enum class QueueCounters { pushes, pops, isrpushes, max, full, empty, count };

    using QueueStats = Stats<QueueCounters>;

    /*
     *   FreeRtos Queue wrapper
     */

    template <class T>
    class Queue {
        static_assert(std::is_trivially_copyable_v<T>, "T is not trivially copyable");
        static_assert(std::is_constructible_v<T>, "Has T a default constructor ?");

    private:
        const QueueHandle_t handle;
        QueueStats stats_;

    public:

        Queue(uint32_t size) : handle(requireNonNull(xQueueCreate(size, sizeof(T)))) {}

        Queue(const Queue&) = delete;

        ~Queue() { vQueueDelete(handle); }

        bool tryPush (const T& item, Milliseconds timeout = 0)
        {
            assert(!xPortInIsrContext());
            bool success = xQueueSend(handle, &item, timeout.toTicks());
            stats_.increment(success ? QueueCounters::pushes : QueueCounters::full);
            return success;
        }

        bool tryPushFromISR(const T& item)
        {
            assert(xPortInIsrContext());
            BaseType_t yield;
            bool success = xQueueSendFromISR(handle, &item, &yield);
            stats_.increment(success ? QueueCounters::isrpushes : QueueCounters::full);
            portYIELD_FROM_ISR(yield);
            return success;
        }

        std::optional<T> tryPop(Milliseconds timeout)
        {
            assert(!xPortInIsrContext());
            T item;
            bool success = xQueueReceive(handle, &item, timeout.toTicks());
            stats_.increment(success ? QueueCounters::pops : QueueCounters::empty);
            if (success) {
                return item;
            } else {
                return std::nullopt;
            }
        }

        const auto& stats() const { return stats_; };
    };

    /*
     * remainder of file is untested code for future use
     */
    /**
     * RAII style Guard object, analog to std::lock_guard, but the constructor has been made private,
     * and the Guard is created by (Recursive)Mutex.guard().
     *
     * @tparam M Mutex or RecursiveMutix
     */
    template <class M>
    class Guard {
    public:
        ~Guard() { m.unlock(); }
        friend M;
    private:
        Guard(M & m) : m(m) { m.lock(); }
        Guard(const Guard &other) = delete;
        M &m;
    };
    /**
     * class Mutex is a wrapper for a FreeRtos semaphore.
     * The lock and unlock are made private to promote RAII style locking using a guard object
     *
     */
    class Mutex {
    public:
        Mutex() = default;
        Mutex(const Mutex &) = delete;
        ~Mutex() { vSemaphoreDelete(handle); }
        /**
         * Create a Guard object for this mutex
         * Typical usage:
         *     {
         *         auto guard = mutex.guard(); // constructor locks mutex
         *         critical section
         *         // guard destructor unlocks mutex
         *    }
         * @return
         */
        Guard<Mutex> guard() { return Guard<Mutex>(*this); }
        friend class Guard<Mutex>;
    private:
        const SemaphoreHandle_t handle = requireNonNull(xSemaphoreCreateMutex());
        void lock() { xSemaphoreTake(handle, portMAX_DELAY); }
        void unlock() { xSemaphoreGive(handle); }
    };
    /**
     * simular to Mutex
     */
    class RecursiveMutex {
    public:
        RecursiveMutex() = default;
        RecursiveMutex(const RecursiveMutex &) = delete;
        ~RecursiveMutex() { vSemaphoreDelete(handle); }
        Guard<RecursiveMutex> guard() { return Guard(*this); }
        friend class Guard<RecursiveMutex>;
    private:
        const SemaphoreHandle_t handle = requireNonNull(xSemaphoreCreateRecursiveMutex());
        void lock() { xSemaphoreTakeRecursive(handle, portMAX_DELAY); }
        void unlock() { xSemaphoreGiveRecursive(handle); }
    };

    /**
     * RAII wrapper for vTaskSuspendAll() / xTaskResumeAll
     * protects block against other tasks, while allowing all interrupts
     * can be used as a cheap mutex if the critical section is short
     * Do not use any blocking calls inside the critical section !!
     * Best to avoid all FreeRtos calls.
     *
     * Preferred use is
     *  {
     *        auto guard = SuspendGuard();
     *      // critical section
     *  }
     *
     */
    class SuspendGuard {
    public:
        SuspendGuard() { vTaskSuspendAll(); }
        ~SuspendGuard() { xTaskResumeAll(); }
    };

    /**
     * RAII wrapper for taskENTER_CRITICAL() / taskEXIT_CRITICAL()
     * protects block against other tasks and interrrupts
     * disable all interrupts with a lower or equal priority than the FreeRtos max priority (means higher or equal prioirty number)
     * Interrupt with a higher priority, that is a lower priority number than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY (2) are not disabled.
     * Do not use any blocking calls inside the critical section. !!
     * Best to avoid all FreeRtos calls.
     * Think twice if your use case really needs this.
     *
     * * Preferred use is
     *  {
     *        auto guard = CriticalGuard();
     *      // critical section
     *  }
     *
     */
    class CriticalGuard {
    private:
        static portMUX_TYPE my_mutex;
    public:
        CriticalGuard() { taskENTER_CRITICAL(&my_mutex); }
        ~CriticalGuard() { taskEXIT_CRITICAL(&my_mutex); }
    };

    /*
     * wrapper for FreeRtos counting semaphore API
     */
    class CountingSemaphore {
    public:
        CountingSemaphore(uint32_t maxCount , uint32_t initialCount) : handle_(requireNonNull(xSemaphoreCreateCounting(maxCount, initialCount))) {}
        CountingSemaphore(const CountingSemaphore &) = delete;
        ~CountingSemaphore() { vSemaphoreDelete(handle_); }
        bool take (Milliseconds delay) { return xSemaphoreTake(handle_, delay.toTicks()); }
        bool give() { return xSemaphoreGive(handle_); }
        bool giveFromISR()
        {
            configASSERT(xPortInIsrContext());
            BaseType_t yield;
            xSemaphoreGiveFromISR(handle_, &yield);
            portYIELD_FROM_ISR(yield);
        }
    private:
        const SemaphoreHandle_t handle_;
    };

    /*
     * wrapper for FreeRtos binary semaphore API
     */
    class BinarySemaphore {
    public:
        BinarySemaphore() = default;
        BinarySemaphore(const BinarySemaphore &) = delete;
        ~BinarySemaphore() { vSemaphoreDelete(handle); }
        bool take (Milliseconds delay) { return xSemaphoreTake(handle, delay.toTicks()); }
        bool give() { return xSemaphoreGive(handle); }
        bool giveFromISR()
        {
            configASSERT(xPortInIsrContext());
            BaseType_t yield;
            xSemaphoreGiveFromISR(handle, &yield);
            portYIELD_FROM_ISR(yield);
        }
    private:
        const SemaphoreHandle_t handle = requireNonNull(xSemaphoreCreateBinary());
    };

    /**
     *  BoundedBlockingQueue is a concurrent bounded blocking queue for inter thread communication.
     * It supports multiple readers and writers.
     * Inspired by https://morestina.net/blog/1400/minimalistic-blocking-bounded-queue-for-c
     * As freertos does not have condition variables, they are replaced with counting semaphores.
     * The queue is backed by a ringbuffer (Did not use std:deque as it allocates and deallocates a lot).
     * Drawback is that at queue construction time N instances of T are default constructed.
     * Items are moved in and out the queue, passing responsibility for the object lifecycle from pusher to popper.
     * Note that the used container is not aware if its full or empty(both head == tail), but the semaphores do.
     *
     * @tparam T item class
     * @tparam N number of entries in the queue
     */
    template <class T, size_t N>
    class BoundedBlockingQueue {
    private:
        Mutex m;
        CountingSemaphore freeSlots{N,N};
        CountingSemaphore usedSlots{N,0};
        T container[N];
        T * head = container;
        T * tail = container;
        T * fix(T * p) { return p == container + N ? container : p; }
    public:
        BoundedBlockingQueue() = default;
        BoundedBlockingQueue(const BoundedBlockingQueue &) = delete;
        /**
         * try to push an item on the queue.
         * @param item
         * @param delay
         * @return
         */
        bool try_push(T &&item, uint32_t delay) {
            auto pass  = freeSlots.take(delay);
            if (pass) {
                {
                    auto guard = m.guard();
                    *head++ = std::move(item);
                    head = fix(head);
                }
                usedSlots.give();
            }
            return pass;
         }
        /**
         * pops an item from the queue.
         * typical usage pattern:
         *     if (auto item = try_pop(100)) {
         *         process(*item);
         *     } else {
         *         //timeout, optional is empty
     *         }
           * @param delay  ms to wait for an item
           * @return an optional
           */
         std::optional<T> try_pop(uint32_t delay) {
             std::optional<T> result;
             auto pass = usedSlots.take(delay);
             if (pass) {
                 {
                     auto guard = m.guard();
                     result = std::move(*tail++);
                     tail = fix(tail);
                 }
                 freeSlots.give();
             }
             return result;
         }
    };

    template<typename T>
    bool wait(T predicate, Milliseconds maxWait, Milliseconds initialDelay, Milliseconds retryDelay) {
        assert(initialDelay.ms < maxWait.ms);
        assert(retryDelay.ms < maxWait.ms);
        auto start = xTaskGetTickCount();
        vTaskDelay(initialDelay.toTicks());
        bool success = predicate();
        while (!success && ((xTaskGetTickCount() - start) <= maxWait.toTicks())) {
            vTaskDelay(retryDelay.toTicks());
            success = predicate();
            if (success) {
                return true;
            }
        }
        // one last try
        return predicate();
    }
}
