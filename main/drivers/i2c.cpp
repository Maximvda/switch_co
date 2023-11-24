#include "i2c.hpp"

using driver::I2C;

void I2C::init(uint32_t clk_speed)
{
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 18,
        .scl_io_num = 19,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = clk_speed
        },
        .clk_flags = 0
    };
    i2c_param_config(I2C_NUM_0, &i2c_config);
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, i2c_config.mode, 0, 0, 0));
}

void I2C::read(uint8_t address, uint8_t* buffer, uint8_t length)
{
    i2c_master_read_from_device(I2C_NUM_0, address, buffer, length, 10);
}

void I2C::write(uint8_t address, uint8_t* buffer, uint8_t length)
{
    i2c_master_write_to_device(I2C_NUM_0, address, buffer, length, 10);
}