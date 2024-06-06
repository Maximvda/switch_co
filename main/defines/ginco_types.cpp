#include "ginco_types.hpp"

#include "esp_log.h"

#include "supervisor.hpp"

static constexpr char* TAG = "types";

using data::GincoMessage;

bool GincoMessage::send(bool acknowledge) { return app::taskFinder().ginco().transmit(*this); }