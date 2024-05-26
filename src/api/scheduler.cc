// EPOS CPU Scheduler Component Implementation

#include <process.h>
#include <time.h>

__BEGIN_SYS

volatile unsigned int Variable_Queue_Scheduler::_next_queue;

inline Real_Time_Scheduler_Common::Tick Real_Time_Scheduler_Common::elapsed() { return Alarm::elapsed(); }

Real_Time_Scheduler_Common::Tick Real_Time_Scheduler_Common::ticks(Microsecond time) { return Alarm::ticks(time);}

Microsecond Real_Time_Scheduler_Common::time(Tick ticks) { return Alarm::time(ticks); }

void Real_Time_Scheduler_Common::collect(Event event) {
    db<Thread>(TRC) << "RT::handle(this=" << this << ",e=";
    if(event & CREATE) {
        db<Thread>(TRC) << "CREATE";

        _statistics.thread_creation = elapsed();
        _statistics.job_released = false;
    }
    if(event & FINISH) {
        db<Thread>(TRC) << "FINISH";

        _statistics.thread_destruction = elapsed();
    }
    if(event & ENTER) {
        db<Thread>(TRC) << "ENTER";

        _statistics.thread_last_dispatch = elapsed();
    }
    if(event & LEAVE) {
        Tick cpu_time = elapsed() - _statistics.thread_last_dispatch;

        db<Thread>(TRC) << "LEAVE";

        _statistics.thread_last_preemption = elapsed();
        _statistics.thread_execution_time += cpu_time;
        _statistics.execution_per_cpu[CPU::id()] += cpu_time;
        _statistics.job_utilization += cpu_time;
    }
    if(periodic() && (event & JOB_RELEASE)) {
        db<Thread>(TRC) << "RELEASE";

        _statistics.job_released = true;
        _statistics.job_release = elapsed();
        _statistics.job_start = 0;
        _statistics.job_utilization = 0;
        _statistics.jobs_released++;
    }
    if(periodic() && (event & JOB_FINISH)) {
        db<Thread>(TRC) << "WAIT";

        _statistics.job_released = false;
        _statistics.job_finish = elapsed();
        _statistics.jobs_finished++;
    }
    if(event & COLLECT) {
        db<Thread>(TRC) << "|COLLECT";
    }
    if(periodic() && (event & CHARGE)) {
        db<Thread>(TRC) << "|CHARGE";
    }
    if(periodic() && (event & AWARD)) {
        db<Thread>(TRC) << "|AWARD";
    }
    if(periodic() && (event & UPDATE)) {
        db<Thread>(TRC) << "|UPDATE";
    }
    db<Thread>(TRC) << ") => {i=" << _priority << ",p=" << _period << ",d=" << _deadline << ",c=" << _capacity << "}" << endl;
}

// The following Scheduling Criteria depend on Alarm, which is not available at scheduler.h
template <typename ... Tn>
FCFS::FCFS(int p, Tn & ... an): Priority((p == IDLE) ? IDLE : Alarm::elapsed()) {}

EDF::EDF(Microsecond p, Microsecond d, Microsecond c): Real_Time_Scheduler_Common(int(elapsed() + ticks(d)), p, d, c) {}

void EDF::update() {
    if(periodic())
        _priority = elapsed() + _deadline;
}

LLF::LLF(Microsecond p, Microsecond d, Microsecond c): Real_Time_Scheduler_Common(int(elapsed() + ticks((d ? d : p) - c)), p, d, c) {}

void LLF::update() {
    if(periodic())
        _priority = elapsed() + _deadline - _capacity + _statistics.thread_execution_time;
}

// Since the definition of FCFS above is only known to this unit, forcing its instantiation here so it gets emitted in scheduler.o for subsequent linking with other units is necessary.
template FCFS::FCFS<>(int p);

__END_SYS
