#pragma once

#include "standard_task.hpp"

#include  "concurrent.hpp"

using utils::StandardTask;
using utils::Milliseconds;

namespace app {

	class GincoTask : public StandardTask {

	private:
		void handle(utils::Message&) override;
		void onStart() override;
		void onTimeout() override;
       	Milliseconds queueTimeout() override { return 10;};

	public:

		GincoTask(uint32_t priority) : StandardTask(priority) {}

		const char * name() const override { return "ginco"; }

	};

}
