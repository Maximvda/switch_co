#pragma once

/* std includes */
#include <map>
#include <variant>
#include <string>
/* esp includes */
#include "nvs_flash.h"

#include "concurrent.hpp"

using utils::Mutex;

namespace driver
{
    enum class ConfigKey
    {
        DEVICE_ID,
        MAXIMUM
    };

    class ConfigDriver
    {
    private:
        ConfigDriver();

        using data_variant = std::variant<bool, std::string>;

        nvs_handle_t nvs_handle_;
        std::map<ConfigKey, data_variant> config_data_;

        std::string key_names_[static_cast<uint8_t>(ConfigKey::MAXIMUM)];

        Mutex m_;

        void setUint8(const ConfigKey key, uint8_t value);
        void setString(const ConfigKey key, std::string value);
        void getString(const ConfigKey key);
        void getBool(const ConfigKey key);

    public:
        ConfigDriver(ConfigDriver &other) = delete;
        void operator=(const ConfigDriver& ) = delete;

        template<typename T>
        T& getKey(const ConfigKey key)
        {
            auto guard = m_.guard();
            /* Key should always be defined in our map*/
            assert(config_data_.find(key) != config_data_.end());
            auto& value = config_data_[key];
            /* Check that value has desired alternate */
            assert(std::holds_alternative<T>(value));
            return std::get<T>(value);
        }

        void setKey(const ConfigKey key, data_variant);

        static ConfigDriver& instance();
    };

} // namespace driver