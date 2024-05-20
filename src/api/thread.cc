// EPOS Thread Implementation

#include <machine.h>
#include <system.h>
#include <process.h>

extern "C" { volatile unsigned long _running() __attribute__ ((alias ("_ZN4EPOS1S6Thread4selfEv"))); }

__BEGIN_SYS

bool Thread::_not_booting;
volatile unsigned int Thread::_thread_count;
Scheduler_Timer * Thread::_timer;
Scheduler<Thread> Thread::_scheduler;
Spin Thread::_spin;

void Thread::constructor_prologue(unsigned int stack_size)
{
    lock();

    _thread_count++;
    _scheduler.insert(this);

    _stack = new (SYSTEM) char[stack_size];
}


void Thread::constructor_epilogue(Log_Addr entry, unsigned int stack_size)
{
    db<Thread>(TRC) << "Thread(entry=" << entry
                    << ",state=" << _state
                    << ",priority=" << _link.rank()
                    << ",stack={b=" << reinterpret_cast<void *>(_stack)
                    << ",s=" << stack_size
                    << "},context={b=" << _context
                    << "," << *_context << "}) => " << this << endl;

    assert((_state != WAITING) && (_state != FINISHING)); // invalid states

    if((_state != READY) && (_state != RUNNING))
        _scheduler.suspend(this);

    criterion().collect(Criterion::CREATE);

    if(preemptive && (_state == READY) && (_link.rank() != IDLE))
        reschedule();

    unlock();
}


Thread::~Thread()
{
    lock();

    db<Thread>(TRC) << "~Thread(this=" << this
                    << ",state=" << _state
                    << ",priority=" << _link.rank()
                    << ",stack={b=" << reinterpret_cast<void *>(_stack)
                    << ",context={b=" << _context
                    << "," << *_context << "})" << endl;

    // The running thread cannot delete itself!
    assert(_state != RUNNING);

    switch(_state) {
    case RUNNING:  // For switch completion only: the running thread would have deleted itself! Stack wouldn't have been released!
        exit(-1);
        break;
    case READY:
        _scheduler.remove(this);
        _thread_count--;
        break;
    case SUSPENDED:
        _scheduler.resume(this);
        _scheduler.remove(this);
        _thread_count--;
        break;
    case WAITING:
        _waiting->remove(this);
        _scheduler.resume(this);
        _scheduler.remove(this);
        _thread_count--;
        break;
    case FINISHING: // Already called exit()
        break;
    }

    if(_joining)
        _joining->resume();

    unlock();

    delete _stack;
}

// Precisamos decidir qual vai ser a estratégia para avisar aos outros cores quando uma tarefa tem a sua prioridade reavaliada.
// Temos que mandar IC::ipi's pros outros cores. Nessa linha, iremos mandar apenas para aquele core que teve uma thread sua alterada
// Ou para todos os cores e cada core "se vira" com essa informação?

// Priority needs to be multicore! Needs to use IPI to signal other cores that the scheduler list has been updated

// It should use IC::ipi to send signals to other processors
// We need to determine what are our strategy

    // static void ipi(unsigned int cpu, Interrupt_Id i) {
    //     db<IC>(TRC) << "IC::ipi(cpu=" << cpu << ",int=" << i << ")" << endl;
    //     assert(i < INTS);
    //     msip(cpu) = 1;
    // }

void Thread::priority(const Criterion & c)
{
    lock();

    db<Thread>(TRC) << "Thread::priority(this=" << this << ",prio=" << c << ")" << endl;

    if(_state != RUNNING) { // reorder the scheduling queue
        _scheduler.remove(this);
        _link.rank(Criterion(c));
        _scheduler.insert(this);
    } else
        _link.rank(Criterion(c));

    if(preemptive)
        reschedule();

    unlock();
}


int Thread::join()
{
    lock();

    db<Thread>(TRC) << "Thread::join(this=" << this << ",state=" << _state << ")" << endl;

    // Precondition: no Thread::self()->join()
    assert(running() != this);

    // Precondition: a single joiner
    assert(!_joining);
    if(_state != FINISHING) {
        Thread * prev = running();

        _joining = prev;
        prev->_state = SUSPENDED;
        _scheduler.suspend(prev); // implicitly choose() if suspending chosen()

        Thread * next = _scheduler.chosen();

        dispatch(prev, next);
    }

    unlock();

    return *reinterpret_cast<int *>(_stack);
}


void Thread::pass()
{
    lock();

    db<Thread>(TRC) << "Thread::pass(this=" << this << ")" << endl;

    Thread * prev = running();
    Thread * next = _scheduler.choose(this);

    if(next)
        dispatch(prev, next, false);
    else
        db<Thread>(WRN) << "Thread::pass => thread (" << this << ") not ready!" << endl;

    unlock();
}


void Thread::suspend()
{
    lock();

    db<Thread>(TRC) << "Thread::suspend(this=" << this << ")" << endl;
    Thread * prev = running();

    _state = SUSPENDED;
    _scheduler.suspend(this);

    Thread * next = _scheduler.chosen();

    dispatch(prev, next);

    unlock();
}


void Thread::resume()
{
    lock();

    db<Thread>(TRC) << "Thread::resume(this=" << this << ")" << endl;

    if(_state == SUSPENDED) {
        _state = READY;
        _scheduler.resume(this);

        if(preemptive)
            reschedule();
    } else
        db<Thread>(WRN) << "Resume called for unsuspended object!" << endl;

    unlock();
}


void Thread::yield()
{
    lock();

    db<Thread>(TRC) << "Thread::yield(running=" << running() << ")" << endl;

    Thread * prev = running();
    Thread * next = _scheduler.choose_another();

    dispatch(prev, next);

    unlock();
}


void Thread::exit(int status)
{
    lock();

    db<Thread>(TRC) << "Thread::exit(status=" << status << ") [running=" << running() << "]" << endl;

    Thread * prev = running();
    _scheduler.remove(prev);
    prev->_state = FINISHING;
    *reinterpret_cast<int *>(prev->_stack) = status;
    prev->criterion().collect(Criterion::FINISH);

    _thread_count--;

    if(prev->_joining) {
        prev->_joining->_state = READY;
        _scheduler.resume(prev->_joining);
        prev->_joining = 0;
    }

    Thread * next = _scheduler.choose(); // at least idle will always be there

    dispatch(prev, next);

    unlock();
}


void Thread::sleep(Queue * q)
{
    db<Thread>(TRC) << "Thread::sleep(running=" << running() << ",q=" << q << ")" << endl;
    // alguns sleep() bugam se não estiverem com lock()
    // pode ser mais de um da oprobklea m sei la
    assert(locked()); // locking handled by caller

    Thread * prev = running();
    _scheduler.suspend(prev);
    prev->_state = WAITING;
    prev->_waiting = q;
    q->insert(&prev->_link);

    Thread * next = _scheduler.chosen();

    dispatch(prev, next);
}


void Thread::wakeup(Queue * q)
{
    db<Thread>(TRC) << "Thread::wakeup(running=" << running() << ",q=" << q << ")" << endl;

    assert(locked()); // locking handled by caller
    if(!q->empty()) {
        Thread * t = q->remove()->object();
        t->_state = READY;
        t->_waiting = 0;
        _scheduler.resume(t);

        if(preemptive)
            reschedule();
    }
}


void Thread::wakeup_all(Queue * q)
{
    db<Thread>(TRC) << "Thread::wakeup_all(running=" << running() << ",q=" << q << ")" << endl;

    assert(locked()); // locking handled by caller

    if(!q->empty()) {
        while(!q->empty()) {
            Thread * t = q->remove()->object();
            t->_state = READY;
            t->_waiting = 0;
            _scheduler.resume(t);
        }

        if(preemptive)
            reschedule();
    }
}

// Same thing as priority. This needs to be multicore aware using signaling
void Thread::prioritize(Queue * q)
{
    assert(locked()); // locking handled by caller
    if(priority_inversion_protocol == Traits<Build>::NA || q->empty()) {
        return;
    }

    db<Thread>(TRC) << "Thread::prioritize(q=" << q << ") [running=" << running() << "]" << endl;
    Thread * run = running();
    for(Queue::Iterator i = q->begin(); i != q->end(); ++i) {
        auto owner = i->object();
        if(owner->priority() > run->priority()) {
            owner->_natural_priority = owner->criterion();
            Criterion c = (priority_inversion_protocol == Traits<Build>::CEILING) ? CEILING : run->criterion();
            if(owner->_state == READY) {
                _scheduler.suspend(owner);
                owner->_link.rank(c);
                _scheduler.resume(owner);
            } else if(owner->state() == WAITING) {
                owner->_waiting->remove(&owner->_link);
                owner->_link.rank(c);
                owner->_waiting->insert(&owner->_link);
            } else
                owner->_link.rank(c);
        }
    }
}

// This guy needs to be multicore aware as well!
void Thread::deprioritize(Queue * q)
{
    assert(locked()); // locking handled by caller

    if(priority_inversion_protocol == Traits<Build>::NA || q->empty())
        return;

    db<Thread>(TRC) << "Thread::deprioritize(q=" << q << ") [running=" << running() << "]" << endl;
    for(Queue::Iterator i = q->begin(); i != q->end(); ++i) {
        auto owner = i->object();
        Criterion c = owner->_natural_priority;
        if(owner->_natural_priority != -9000) {
            if(owner->_state == READY) {
                _scheduler.suspend(owner);
                owner->_link.rank(c);
                _scheduler.resume(owner);
            } else if(owner->state() == WAITING) {
                owner->_waiting->remove(&owner->_link);
                owner->_link.rank(c);
                owner->_waiting->insert(&owner->_link);
            } else
                owner->_link.rank(c);
        }
    }
}


void Thread::reschedule()
{
    if(!Criterion::timed || Traits<Thread>::hysterically_debugged)
        db<Thread>(TRC) << "Thread::reschedule()" << endl;

    assert(locked()); // locking handled by caller
    
    Thread * prev = running();
    Thread * next = _scheduler.choose();

    dispatch(prev, next);
}


void Thread::time_slicer(IC::Interrupt_Id i)
{
    lock();
    reschedule();
    unlock();
}


void Thread::dispatch(Thread * prev, Thread * next, bool charge)
{
    // "next" is not in the scheduler's queue anymore. It's already "chosen"
    if(charge && Criterion::timed) {
        _timer->restart();
    }

    if(prev != next) {
        if(Criterion::dynamic) {
            prev->criterion().collect(Criterion::CHARGE | Criterion::LEAVE);
            update_all_priorities();
            next->criterion().collect(Criterion::AWARD  | Criterion::ENTER);
        }
        if(prev->_state == RUNNING)
            prev->_state = READY;
        next->_state = RUNNING;

        db<Thread>(TRC) << "Thread::dispatch(prev=" << prev << ",next=" << next << ")" << endl;
        if(Traits<Thread>::debugged && Traits<Debug>::info) {
            CPU::Context tmp;
            tmp.save();
            db<Thread>(INF) << "Thread::dispatch:prev={" << prev << ",ctx=" << tmp << "}" << endl;
        }
        db<Thread>(INF) << "Thread::dispatch:next={" << next << ",ctx=" << *next->_context << "}" << endl; 

        // The non-volatile pointer to volatile pointer to a non-volatile context is correct
        // and necessary because of context switches, but here, we are locked() and
        // passing the volatile to switch_constext forces it to push prev onto the stack,
        // disrupting the context (it doesn't make a difference for Intel, which already saves
        // parameters on the stack anyway).
        if(mp)
            _spin.release();
        CPU::switch_context(const_cast<Context **>(&prev->_context), next->_context);
        if(mp)
            _spin.acquire();
    }
}
 // Alguns syncronizers precisam de lock() para usar o CPU::int_disable(); e funcinar corretamente

int Thread::idle()
{
    db<Thread>(TRC) << "Thread::idle(this=" << running() << ")" << endl;

    while(_thread_count > Traits<System>::CPUS) { // someone else besides idle
        if(Traits<Thread>::trace_idle)
            db<Thread>(TRC) << "Thread::idle(this=" << running() << ")" << endl;

        CPU::int_enable();
        CPU::halt();

        if(!preemptive)
            yield();
    }

    CPU::int_disable();
    if(CPU::id() == CPU::BSP) {
        db<Thread>(WRN) << "The last thread has exited!" << endl;
        if(reboot) {
            db<Thread>(WRN) << "Rebooting the machine ..." << endl;
            Machine::reboot();
        } else {
            db<Thread>(WRN) << "Halting the machine ..." << endl;
            CPU::halt();
        }
    }
    
    CPU::halt();

    return 0;
}

// For spin access
Thread * volatile Thread::self() {
    return _not_booting ? running() : reinterpret_cast<Thread * volatile>(CPU::id() + 1);
}

__END_SYS
