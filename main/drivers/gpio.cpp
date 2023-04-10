#include "gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

using namespace driver;
const char* TAG = "GPIO";

static Gpio gpio_driver;
static QueueHandle_t gpioQueue {xQueueCreate(10, sizeof(uint8_t))};

static void IRAM_ATTR gpio_interrupt_handler(void *args)
{
    uint8_t pin_number = (int)args;
    xQueueSendFromISR(gpioQueue, &pin_number, NULL);
}

static void gpio::task(void* pxptr){
    uint8_t pin_number {0};
    while (1){
        if (xQueueReceive(gpioQueue, &pin_number, portMAX_DELAY))
        {
            gpio_driver.toggle_input(pin_number);
        }
    }
}

void Gpio(){};

bool Gpio::init(){
    bool res {false};
    res = init_input();
    res = init_output() and res;
    xTaskCreate(gpio::task, "gpio_task", 2048, NULL, 1, NULL);
    return res;
}


bool Gpio::init_input(){
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
        input_state[i] = gpio_get_level(input_arr[i]) == 1;
        gpio_isr_handler_add(input_arr[i], gpio_interrupt_handler, (void *)i);
    }
    return true;
}

bool Gpio::init_output(){
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

void Gpio::toggle_input(uint8_t number){
    input_state[number] = !input_state[number];
}