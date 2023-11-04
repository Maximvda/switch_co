#include "ginco_types.hpp"

#include "supervisor.hpp"

using data::GincoMessage;

GincoMessage GincoMessage::acknowledge()
{
    GincoMessage copy = *this;
    copy.ack_ = true;
    copy.data_length = 0;
    return copy;
}

bool GincoMessage::send(bool acknowledge)
{
    bool res = app::taskFinder().can().transmit(*this);
    if (acknowledge)
    {
        /* FIXME: Is not actually returning real response of the acknowledge !! */
        /*TODO: !!*/
        return res;
    }
    return res;
}