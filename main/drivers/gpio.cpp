#include "gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define OUTPUT_MASK GPIO_NUM_23 | GPIO_NUM_25 | GPIO_NUM_14 | GPIO_NUM_12 | GPIO_NUM_19 | GPIO_NUM_18 | GPIO_NUM_17
#define INPUT_MASK GPIO_NUM_15 | GPIO_NUM_33 | GPIO_NUM_26 | GPIO_NUM_27 | GPIO_NUM_13 | GPIO_NUM_4 | GPIO_NUM_16

using namespace driver::gpio;
const char* TAG = "GPIO";

xQueueHandle interputQueue;




static void IRAM_ATTR gpio_interrupt_handler(void *args)
{
    int pinNumber = (int)args;
    xQueueSendFromISR(interputQueue, &pinNumber, NULL);
}

void init(){
    // Readout current state of the gpio pins
    gpio_config_t gpio_config;
    gpio_config->pin_bit_mask = INPUT_MASK;
    gpio_config->mode = GPIO_MODE_INPUT;
    gpio_config->pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config->pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config->intr_type = GPIO_INTR_ANYEDGE;

    // Enable interrupts on gpio inputs
    for(int i=0; i < 7; i++){
        gpio_set_intr_type(i, GPIO_INTR_ANYEDGE);
        gpio_intr_enable(i);
    }
}
