/*
 * stats.h
 *
 *  Created on: 27 Jun 2023
 *      Author: karel
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <atomic>
#include <vector>

namespace utils {
    /**
     * class Stats provides thread safe counters for gathering usage statistics
     *
     * template parametes
     *   E  enum or enum class naming the individual counters
     *   The enum must provide a count value and the values must be ordered from 0 to count.
     */
    template<typename E>
    class Stats {
    private:
        std::atomic_uint32_t counters_[static_cast<size_t> (E::count)] {};

        std::atomic_uint32_t& counter(E e) { return counters_[static_cast<uint32_t>(e)]; }

        const std::atomic_uint32_t& counter(E e) const { return counters_[static_cast<uint32_t>(e)]; }

    public:

        // stats producer api:

        Stats<E>& add(E e, uint32_t value) {
            counter(e).fetch_add(value, std::memory_order_relaxed);
            return *this;
        }

        Stats<E>& increment(E e) { return add(e,1); }

        /**
         *  update counter value with argument only if it is the new maximum
         */
        Stats<E>& max(E e, uint32_t value) {
            uint32_t current = counter(e).load(std::memory_order_relaxed);
            while (value > current && !counter(e).compare_exchange_weak(current, value, std::memory_order_relaxed));
            return *this;
        }

        // stats constumer api

        uint32_t get(E e) const {
            return counter(e).load(std::memory_order_relaxed);
        }

    };

}
