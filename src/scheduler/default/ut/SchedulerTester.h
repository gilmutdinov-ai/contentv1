#pragma once
#include "scheduler/default/Scheduler.h"

namespace contentv1 {

class SchedulerTester {

  inline static const std::string s_conf_rel_path{
      "/src/scheduler/default/ut/scheduler_ut.conf"};

  std::unique_ptr<Scheduler> m_scheduler;

public:
  SchedulerTester();
};

} // namespace contentv1