// EPOS RISC-V Timer Mediator Initialization

#include <architecture/cpu.h>
#include <machine/timer.h>
#include <machine/ic.h>

__BEGIN_SYS

void Timer::init()
{
    db<Init, Timer>(TRC) << "Timer::init()" << endl;

    assert(CPU::int_disabled());

    if (CPU::id() == CPU::BSP)
        IC::int_vector(IC::INT_SYS_TIMER, int_handler);

    reset(); // Resets each mtimecmp timer
    
    if (CPU::id() == CPU::BSP) 
        IC::enable(IC::INT_SYS_TIMER);  // Enabling int_sys_timer is enabling timer interrupts
}

__END_SYS
