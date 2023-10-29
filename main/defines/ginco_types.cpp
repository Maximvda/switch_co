#include "ginco_types.hpp"

#include "supervisor.hpp"

using data::GincoMessage;

bool GincoMessage::acknowledge()
{
    ack_ = true;
    return app::taskFinder().can().transmit(*this);
}