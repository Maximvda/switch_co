#pragma once

#include "standard_task.hpp"

#include "can.hpp"
#include "concurrent.hpp"
#include "events.h"

using utils::StandardTask;
using utils::Milliseconds;
using driver::can::CanDriver;

namespace app {

	class CanTask : public StandardTask {

	private:
		void handle(utils::Message&) override;
		void onStart() override;
		void onTimeout() override;
       	Milliseconds queueTimeout() override { return 10; };

		CanDriver can_driver_;

	public:

		CanTask(uint32_t priority) : StandardTask(priority) {}

		const char * name() const override { return "Can"; }

		bool transmit(twai_message_t message)
		{
			return post(EVENT_CAN_TRANSMIT, std::make_unique<twai_message_t>(message));
		}

	};

}
