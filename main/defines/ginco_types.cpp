#include "ginco_types.hpp"

#include "esp_log.h"

#include "supervisor.hpp"

static constexpr char* TAG = "types";

using data::GincoMessage;

GincoMessage::GincoMessage(const twai_message_t& message)
{
    source_id = (message.identifier >> 18) & 0xFF;
    linked_ = (message.identifier >> 17) & 0x01;
    ack_ = (message.identifier >> 16) & 0x01;
    feature_type_ = (message.identifier >> 13) & 0x07;
    index_ = (message.identifier >> 8) & 0x1F;
    function = static_cast<Function>(message.identifier & 0xFF);
    data_length = message.data_length_code;
    memcpy(&data, message.data, message.data_length_code);
};

GincoMessage GincoMessage::acknowledge()
{
    GincoMessage copy = *this;
    copy.ack_ = true;
    copy.data_length = 0;
    return copy;
}

bool GincoMessage::send(bool acknowledge)
{

    /* Block calling thread untill operation is finished */
    bool res = false;
    auto& can_task = app::taskFinder().can();
    if (acknowledge)
    {
        do {
            /* This executes a can transmit on the CAN thread while blocking the calling thread untill finished */
            can_task.inspect(
                [&res, this](){res = app::taskFinder().can().can_driver.transmit(*this, true);}
            );
            if (!res)
            {
                ESP_LOGW(TAG, "Did retransmit! %lu", static_cast<uint32_t>(function));
            }
        } while (!res); /* This will handle retransmits */
        return res;
    }
    return can_task.transmit(*this);
}

uint32_t GincoMessage::id() const
{
    return (
        (event_ ? 1 << 26 : 0) +
        (source_id << 18) +
        (linked_ ? 1 << 17 : 0) +
        (ack_ ? 1 << 16 : 0) +
        (feature_type_ << 13) +
        (index_ << 8) +
        static_cast<uint8_t>(function));
}

twai_message_t& GincoMessage::canMessage()
{
    can_message_.identifier = id();
    can_message_.extd = 1;
    can_message_.data_length_code = data_length;
    memcpy(can_message_.data, &data, data_length);
    return can_message_;
}

bool GincoMessage::isAcknowledge(GincoMessage &other)
{
    // invert the other message and compare id
    other.ack_ = !other.ack_;
    bool resp = other == *this;
    other.ack_ = !other.ack_;
    return resp;
}