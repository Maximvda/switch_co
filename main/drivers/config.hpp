#pragma once

#include "stdint.h"
#include "nvs_flash.h"

namespace driver
{
    class ConfigDriver
    {
    private:
        uint8_t getKey(const char* key);
        void setKey(const char* key, uint8_t value);
        char* getString(const char* key, size_t length);
        void setString(const char* key, const char* value);
    public:
        void init();
        uint8_t deviceId();
        /* Returns wether output is configured as pwm or normal gpio (true for pwm) */
        bool outputPwmMode(uint8_t id);
        uint8_t outputLevel(uint8_t id);
    };

} // namespace driver