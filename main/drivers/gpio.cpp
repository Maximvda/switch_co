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


void gpio::set_output(uint8_t id, uint8_t value){
    gpio_set_level(output_arr[id], static_cast<uint32_t>(value));
}

static void task(void* pxptr){
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
    xTaskCreate(task, "gpio_task", 2048, NULL, 1, NULL);
    if (!res){
        ESP_LOGE(TAG, "Failed to initialise");
    }
    return res;
}

bool input_init(){
    uint32_t input_mask {0};
    for (int i=0; i < TOTAL_GPIO; i++){
        input_mask |= input_arr[i];
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

    gpio_install_isr_service(0);
    // Read the current gpio states
    for (int i=0; i < TOTAL_GPIO; i++){
        gpio_isr_handler_add(input_arr[i], gpio_interrupt_handler, (void *)i);
    }
    return true;
}

bool output_init(){
    uint32_t output_mask {0};
    for (int i=0; i < TOTAL_GPIO; i++){
        output_mask |= output_arr[i];
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
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_13_BIT,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = 5000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = GPIO_NUM_23,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER_0,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0,
        .flags = {0}
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    return true;
}