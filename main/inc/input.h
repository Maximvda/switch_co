#ifndef __INPUT_H_
#define __INPUT_H_

#include "can_driver.h"

class Input {
    private:
        uint8_t id {0};
        bool button {false};
        bool state {false};

    public:
        Input();
        explicit Input(uint8_t id);
        void heartbeat();
};

#endif