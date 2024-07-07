#include "ginco_message.hpp"

#include "supervisor.hpp"

bool ginco::GincoMessage::send(bool acknowledge) { return app::taskFinder().ginco().transmit(*this); }