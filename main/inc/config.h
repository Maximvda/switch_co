#ifndef __CONFIG_DRIVER_H_
#define __CONFIG_DRIVER_H_

#include "stdint.h"

namespace config {
    void init();
    uint8_t get_key(const char* key);
    void set_key(const char* key, uint8_t value);
}

#endif