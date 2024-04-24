// EPOS CPU Scheduler Component Implementation

#include <process.h>
#include <time.h>

__BEGIN_SYS

// The following Scheduling Criteria depend on Alarm, which is not available at scheduler.h
template <typename ... Tn>
FCFS::FCFS(int p, Tn & ... an): Priority((p == IDLE) ? IDLE : Alarm::elapsed()) {}

EDF::EDF(const Microsecond & d, const Microsecond & p, const Microsecond & c, unsigned int): Real_Time_Scheduler_Common(Alarm::ticks(d), Alarm::ticks(d), p, c) {}

void EDF::update() {
    if((_priority >= PERIODIC) && (_priority < APERIODIC))
        _priority = Alarm::elapsed() + _deadline;
}

LLF::LLF(const Microsecond & deadline, const Microsecond & period, const Microsecond & capacity, unsigned int): 
    Real_Time_Scheduler_Common((deadline - capacity), Alarm::ticks(deadline),  Alarm::ticks(period),  Alarm::ticks(capacity)) { start_execution = Alarm::elapsed(); }

void LLF::update() {
    if((_priority >= PERIODIC) && (_priority < APERIODIC))
        _priority = _deadline - (Alarm::elapsed() + _capacity);
}

// TODO: Avaliar se precisa ficar aqui mesmo, e todo o resto que ta no shceduler.h
void LLF::collect_start_time() {
    start_execution = Alarm::elapsed();
}

void LLF::update_capacity() {
    auto elapsed_time = Alarm::elapsed() - start_execution;
    if (elapsed_time < _capacity)
        _capacity = _capacity - elapsed_time;
    else
        _capacity = 0;
}

// Since the definition of FCFS above is only known to this unit, forcing its instantiation here so it gets emitted in scheduler.o for subsequent linking with other units is necessary.
template FCFS::FCFS<>(int p);

__END_SYS
