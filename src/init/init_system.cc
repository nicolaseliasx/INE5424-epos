// EPOS System Initializer

#include <utility/random.h>
#include <machine.h>
#include <memory.h>
#include <system.h>
#include <process.h>

__BEGIN_SYS

class Init_System
{
private:
    static const unsigned int HEAP_SIZE = Traits<System>::HEAP_SIZE;

public:
    Init_System() {
        // provavelmente aqui problemas

        // Synchronizing systems heap creation
        CPU::smp_barrier();

        // Only the boot CPU runs INIT_SYSTEM fully
        if(CPU::id() == CPU::BSP) {
            db<Init>(INF) << "Init:si=" << *System::info() << endl;

            db<Init>(INF) << "Initializing the architecture: " << endl;
            CPU::init();

            db<Init>(INF) << "Initializing system's heap: " << endl;
            if(Traits<System>::multiheap) {
                // Dont need to keep multiheap implementation here
            } else
                System::_heap = new (&System::_preheap[0]) Heap(MMU::alloc(MMU::pages(HEAP_SIZE)), HEAP_SIZE);
            
            db<Init>(INF) << "Initializing the machine: " << endl;
            Machine::init();
            db<Init>(INF) << "machine init done!" << endl;

            CPU::smp_barrier(); // signalizes "machine ready" to other CPUs

        } else {
            CPU::smp_barrier(); // waits until the bootstrap CPU signalizes "machine ready"

            db<Init>(INF) << "Initializing the CPU: " << endl;
            CPU::init();
            Timer::init(); // We need to initialize timer interrupts for all CPUs - This is also executed inside Machine::init

            db<Init>(INF) << "Initializing the machine: " << endl;
        }
        
        CPU::smp_barrier(); // Go to system init with all the timers created

        db<Init>(INF) << "Initializing system abstractions: " << endl;
        System::init();

        // Randomize the Random Numbers Generator's seed
        // Create a random seed for the random numbers generator 
        if(Traits<Random>::enabled && Traits<TSC>::enabled) {
            db<Init>(INF) << "Randomizing the Random Numbers Generator's seed." << endl;
            Random::seed(TSC::time_stamp());
        }

        // Initialization continues at init_end
    }
};

// Global object "init_system" must be constructed first.
Init_System init_system;

__END_SYS
