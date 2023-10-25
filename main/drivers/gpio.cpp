#include "gpio.hpp"

#include "supervisor.hpp"

#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "esp_err.h"

using driver::GpioDriver;
const static char* TAG = "gpio driver";
static void IRAM_ATTR gpio_interrupt_handler(void *args);

bool GpioDriver::init()
{
    ESP_LOGI(TAG, "Initialising");
    bool res = initInput();
    res &= initOutput();
    res &= initLed();
    if (!res){
        ESP_LOGE(TAG, "Failed to initialise");
    }
    return res;
}

bool GpioDriver::initInput()
{
    uint64_t input_mask {0};
    for (int i=0; i < GpioDriver::total_gpio; i++){
        input_mask |= ( 1ULL << inputs_[i]);
    }

    gpio_config_t config = {};
    config.pin_bit_mask = input_mask;
    config.mode = GPIO_MODE_INPUT;
    config.pull_up_en = GPIO_PULLUP_ENABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_ANYEDGE;

    if (gpio_config(&config) != ESP_OK){
        ESP_LOGE(TAG, "Failed to set input config");
        return false;
    }

    ESP_LOGI(TAG, "Input config set.");

    gpio_install_isr_service(0);
    for (int i=0; i < GpioDriver::total_gpio; i++){
        gpio_isr_handler_add(inputs_[i], gpio_interrupt_handler, (void *)i);
    }
    ESP_LOGI(TAG, "ISR configured.");
    return true;
}

bool GpioDriver::initOutput()
{
    uint32_t output_mask {0};
    for (int i=0; i < GpioDriver::total_gpio; i++){
        output_mask |= (1ULL << outputs_[i]);
    }
    gpio_config_t config;
    config.pin_bit_mask = output_mask;
    config.mode = GPIO_MODE_OUTPUT;

    return gpio_config(&config) == ESP_OK;
}

bool GpioDriver::initLed()
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_HIGH_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_13_BIT,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = 5000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    return ledc_timer_config(&ledc_timer) == ESP_OK;
}

bool GpioDriver::setOutput(uint8_t id, std::variant<bool, uint8_t> value)
{
    auto& state = output_states_[id];
    if (std::holds_alternative<bool>(value))
    {
        /* When output pin is in pwm we have to reconfigure it */
        if (state.pwm_mode)
        {
            changeOutput(id);
        }
        /* Just toggling the output pin */
        state.high = std::get<bool>(value);
        return gpio_set_level(outputs_[id], static_cast<uint32_t>(state.high)) == ESP_OK;
    }

    /* Should hold uint8_t alternative otherwise programming error! */
    assert(std::holds_alternative<uint8_t>(value));

    if (!state.pwm_mode)
    {
        changeOutput(id);
    }

    state.pwm_level = std::get<uint8_t>(value);

    ledc_channel_t channel = static_cast<ledc_channel_t>(id);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, static_cast<uint32_t>(state.pwm_level));
    return ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel) == ESP_OK;
}

bool GpioDriver::changeOutput(const uint8_t id)
{
    ledc_channel_t channel = static_cast<ledc_channel_t>(id);
    auto& state = output_states_[id];
    if (state.pwm_mode){
        state.pwm_mode = false;
        return ledc_stop(LEDC_HIGH_SPEED_MODE, channel, state.high) == ESP_OK;
    }
    state.pwm_mode = true;
    /* Change config to pwm mode */
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = outputs_[id],
        .speed_mode     = LEDC_HIGH_SPEED_MODE,
        .channel        = channel,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER_0,
        .duty           = state.pwm_level, // Set duty to 0%
        .hpoint         = 0,
        .flags = {0}
    };
    return ledc_channel_config(&ledc_channel) == ESP_OK;
}

static void IRAM_ATTR gpio_interrupt_handler(void *args)
{
    app::taskFinder().gpio().isrGpioToggle((uint32_t)args);
}