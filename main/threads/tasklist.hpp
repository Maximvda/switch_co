#pragma once

#include "standard_task.hpp"
#include "can_thread.hpp"
#include "gpio_thread.hpp"
#include "ginco_thread.hpp"

using utils::SupervisedTask;

namespace app {

	class TaskList {
	private:
        app::CanTask can_ {3};
		app::GpioTask gpio_ {2};
		app::GincoTask ginco_ {1};

		const std::vector<SupervisedTask *> tasks_ {
			&can_,
			&gpio_,
			&ginco_
		};

		TaskList() = default;

	public:
		// only Supervisor can create the TaskList
		friend class Supervisor;

		TaskList(const TaskList&) = delete;
		auto& tasks() { return tasks_; }

		auto& can() { return can_;}
		auto& gpio() { return gpio_;}
		auto& ginco() { return ginco_;}
	};
}
