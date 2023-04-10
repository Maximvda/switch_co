#ifndef __SWITCH_H_
#define __SWITCH_H_

#include "can_driver.h"

namespace switch_co {
    void on_can_msg(driver::can::message_t message);
}

class SwitchCo {
    public:
        SwitchCo();
        void heartbeat();
};

#endif