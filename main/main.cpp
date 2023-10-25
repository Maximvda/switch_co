#include "supervisor.hpp"

extern "C" {
	void app_main();
}

void app_main(void)
{
	app::Supervisor::createTask();
	// config::init();
	// device::init();
    // xTaskCreate(driver::gpio::task, "gpio_task", 512, NULL, GPIO_TASK_PRIO, NULL);
    // //xTaskCreate(driver::can::receive_task, "CAN_rx", 4096, NULL, RX_TASK_PRIO, NULL);
    // xTaskCreate(on_second, "on_second", 512, NULL, ON_SECOND_PRIO, NULL);
}
