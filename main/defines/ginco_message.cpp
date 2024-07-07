#include "ginco_message.hpp"

#include "supervisor.hpp"

static constexpr char* TAG = "types";

bool ginco::GincoMessage::send(bool acknowledge) { return app::taskFinder().ginco().transmit(*this); }