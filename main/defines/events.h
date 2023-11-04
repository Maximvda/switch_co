#pragma once


typedef enum {
    EVENT_UNUSED_EVENT,
    EVENT_SECOND,
    EVENT_GPIO_TOGGLE,
    EVENT_CAN_TICK,
    EVENT_CAN_TRANSMIT,
    EVENT_CAN_RECEIVED,
} eEventType;

