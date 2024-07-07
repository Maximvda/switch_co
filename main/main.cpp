#include "supervisor.hpp"

extern "C" {
void app_main();
}

void app_main(void) {
    /* Load all config from storage before starting threads */
    driver::config::configInit();
    /* Startup the tasks */
    app::Supervisor::createTask();
}
