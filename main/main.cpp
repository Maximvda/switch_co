#include "supervisor.hpp"

extern "C"
{
    void app_main();
}

void app_main(void)
{
    app::Supervisor::createTask();
}
