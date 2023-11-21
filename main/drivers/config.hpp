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

        using data_variant = std::variant<bool, std::string, uint8_t>;

        nvs_handle_t nvs_handle_;
        std::map<ConfigKey, data_variant> config_data_;

        std::string key_names_[static_cast<uint8_t>(ConfigKey::MAXIMUM)];

        Mutex m_;

        template<typename T>
        void set(const ConfigKey key, T value)
        {
            const char* key_name = key_names_[static_cast<uint16_t>(key)].data();
            /* Use correct call depending on type */
            if constexpr(std::is_same_v<T, uint8_t>)
                nvs_set_u8(nvs_handle_, key_name, value);
            else if constexpr(std::is_same_v<T, std::string>)
                nvs_set_str(nvs_handle_, key_name, value.data());
            else
                assert(0); /* Type not yet supported */
            /* Save the new config */
            nvs_commit(nvs_handle_);
        }

        template<typename T>
        void get(const ConfigKey key)
        {
            const char* key_name = key_names_[static_cast<uint16_t>(key)].data();
            if constexpr(std::is_same_v<T, uint8_t> or std::is_same_v<T, bool>)
            {
                uint8_t value {0};
                nvs_get_u8(nvs_handle_, key_name, &value);
                if constexpr(std::is_same_v<T, uint8_t>)
                    config_data_[key] = value;
                else if constexpr(std::is_same_v<T, bool>)
                    config_data_[key] = value == 1;
            }
            else if constexpr(std::is_same_v<T, std::string>)
            {
                size_t required_size;
                nvs_get_str(nvs_handle_, key_name, nullptr, &required_size);
                char data[required_size];
                nvs_get_str(nvs_handle_, key_name, data, &required_size);
                config_data_[key] = std::string(data);
            }
            else
                assert(0); /* Type not yet supported */
        }

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

        /* TODO: can be optimised with compiler stuff as well */
        void setKey(const ConfigKey key, data_variant);

        static ConfigDriver& instance();
    };

} // namespace driver