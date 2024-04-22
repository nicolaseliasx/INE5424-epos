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
    Real_Time_Scheduler_Common((deadline - capacity), Alarm::ticks(deadline),  Alarm::ticks(period),  Alarm::ticks(capacity)) {}

void LLF::update() {
    // _finished_execution é uma flag que indica se a execução da thread foi finalizada, ela eh setada no dispatch
    if (_finished_execution) {
        _finished_execution = false;
        _capacity = _capacity - (Alarm::elapsed() - _start);
    }
    if((_priority >= PERIODIC) && (_priority < APERIODIC))
        _priority = _deadline - (Alarm::elapsed() + _capacity);
}

void LLF::start_execution() {
    _start = Alarm::elapsed();
}


// Since the definition of FCFS above is only known to this unit, forcing its instantiation here so it gets emitted in scheduler.o for subsequent linking with other units is necessary.
template FCFS::FCFS<>(int p);

__END_SYS
