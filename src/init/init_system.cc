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
        if(CPU::id() != 0) {
            // Wait until the boot CPU has initialized the machine
            CPU::smp_barrier();
            Timer::init();
            db<Init>(INF) << "Init:si=" << "Im not the BSP return" << endl;
            return;
        }

        // Only boostrap CPU runs init system

    db<Init>(INF) << "Init:si=" << *System::info() << endl;

        db<Init>(INF) << "Initializing the architecture: " << endl;
        CPU::init();

        db<Init>(INF) << "Initializing system's heap: " << endl;
        if(Traits<System>::multiheap) {
            System::_heap_segment = new (&System::_preheap[0]) Segment(HEAP_SIZE, Segment::Flags::SYSD);
            char * heap;
            if(Memory_Map::SYS_HEAP == Traits<Machine>::NOT_USED)
                heap = Address_Space(MMU::current()).attach(System::_heap_segment);
            else
                heap = Address_Space(MMU::current()).attach(System::_heap_segment, Memory_Map::SYS_HEAP);
            if(!heap)
                db<Init>(ERR) << "Failed to initialize the system's heap!" << endl;
            System::_heap = new (&System::_preheap[sizeof(Segment)]) Heap(heap, System::_heap_segment->size());
        } else
            System::_heap = new (&System::_preheap[0]) Heap(MMU::alloc(MMU::pages(HEAP_SIZE)), HEAP_SIZE);
        
        db<Init>(INF) << "Initializing the machine: " << endl;
        Machine::init();
        db<Init>(INF) << "machine init done!" << endl;

        // Boostraping CPU signaling other CPUs that the heap has been created
        CPU::smp_barrier();
        
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
