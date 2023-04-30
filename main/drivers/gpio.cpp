#include "gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "esp_err.h"

using namespace driver;
const static char* TAG = "GPIO";

static gpio_num_t input_arr[TOTAL_GPIO] {GPIO_NUM_15,GPIO_NUM_33,GPIO_NUM_26,GPIO_NUM_27,GPIO_NUM_13,GPIO_NUM_4,GPIO_NUM_16};
static gpio_num_t output_arr[TOTAL_GPIO] {GPIO_NUM_23,GPIO_NUM_25,GPIO_NUM_14,GPIO_NUM_12,GPIO_NUM_19,GPIO_NUM_18,GPIO_NUM_17};

static QueueHandle_t gpioQueue {xQueueCreate(10, sizeof(uint8_t))};
static void (*toggle_callback)(uint8_t id);


void gpio::set_output(uint8_t id, uint32_t value){
    gpio_set_level(output_arr[id], value);
    ESP_LOGI(TAG, "Set level for id %u, pin number, %lu with value %lu", id, static_cast<uint32_t>(output_arr[id]), static_cast<uint32_t>(value));
}

void gpio::set_level(uint8_t id, uint32_t level){
    ledc_channel_t channel = static_cast<ledc_channel_t>(id);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, level);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
}

void gpio::task(void* pxptr){
    uint8_t pin_number {0};
    while (1){
        if (xQueueReceive(gpioQueue, &pin_number, portMAX_DELAY))
        {
            toggle_callback(pin_number);
        }
    }
}

static void IRAM_ATTR gpio_interrupt_handler(void *args)
{
    uint8_t pin_number = (int)args;
    xQueueSendFromISR(gpioQueue, &pin_number, NULL);
}


bool input_init();
bool output_init();
bool init_led_timer();

bool gpio::init(void (*_callback)(uint8_t id)){
    toggle_callback = _callback;
    bool res {false};
    res = input_init();
    res = output_init() and res;
    res = init_led_timer() and res;
    if (!res){
        ESP_LOGE(TAG, "Failed to initialise");
    }
    return res;
}

bool input_init(){
    uint64_t input_mask {0};
    for (int i=0; i < TOTAL_GPIO; i++){
        input_mask |= ( 1ULL << input_arr[i]);
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
    } else {
        ESP_LOGI(TAG, "Input config set.");
    }

    gpio_install_isr_service(0);
    // Read the current gpio states
    for (int i=0; i < TOTAL_GPIO; i++){
        gpio_isr_handler_add(input_arr[i], gpio_interrupt_handler, (void *)i);
    }
    ESP_LOGI(TAG, "ISR configured.");
    return true;
}

bool output_init(){
    uint32_t output_mask {0};
    for (int i=0; i < TOTAL_GPIO; i++){
        output_mask |= (1ULL << output_arr[i]);
    }
    gpio_config_t config;
    config.pin_bit_mask = output_mask;
    config.mode = GPIO_MODE_OUTPUT;
    if (gpio_config(&config) != ESP_OK){
        ESP_LOGE(TAG, "Failed to set output config");
        return false;
    }
    return true;
}

bool init_led_timer(void){
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

bool gpio::change_output(uint8_t id, bool pwm, uint32_t level){
    ledc_channel_t channel = static_cast<ledc_channel_t>(id);
    if (pwm){
        ledc_channel_config_t ledc_channel = {
            .gpio_num       = output_arr[id],
            .speed_mode     = LEDC_HIGH_SPEED_MODE,
            .channel        = channel,
            .intr_type      = LEDC_INTR_DISABLE,
            .timer_sel      = LEDC_TIMER_0,
            .duty           = level, // Set duty to 0%
            .hpoint         = 0,
            .flags = {0}
        };
        return ledc_channel_config(&ledc_channel) == ESP_OK;
    }
    // Setting config for normal output
    return ledc_stop(LEDC_HIGH_SPEED_MODE, channel, level) == ESP_OK;
}