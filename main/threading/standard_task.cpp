#include <cassert>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "standard_task.hpp"

using namespace utils;

constexpr TickType_t aliveTimeout = Milliseconds(1000).toTicks();

void StandardTask::process(Message& message)
{
    if (message.event() & 0x80000000) { // highest bit set, reserved for handling in the base class
        switch (message.event()) {
        case static_cast<uint32_t>(MyEvents::inspect):
            if (auto p = message.pointerValue<std::function<void(void)>>()) {
                (*p)();
            }
            else {
                assert(p);
            }
            break;

        default:
            assert(0); //unknown event
            break;
        }
    }
    else {
        handle(message);
    }
}

void StandardTask::run()
{

    onStart();
    uint32_t last_alive_sent = 0;

    for (;;) {
    	if (auto maybe_message = queue_.tryPop(queueTimeout())) {
            process(*maybe_message);
            // test if heap pointer has been taken
            assert(!maybe_message->hasUniquePointer());
        } else {
            onTimeout();
        }
        // Notify that thread is alive
        TickType_t now = xTaskGetTickCount();
        if((now - last_alive_sent) >= aliveTimeout) {
            if (sendAlive()) {
            	last_alive_sent = xTaskGetTickCount();
            }
        }
        tick();
    }
}



