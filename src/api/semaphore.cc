// EPOS Semaphore Implementation

#include <synchronizer.h>

__BEGIN_SYS

Semaphore::Semaphore(long v) : _value(v)
{
    db<Synchronizer>(TRC) << "Semaphore(value=" << _value << ") => " << this << endl;
}


Semaphore::~Semaphore()
{
    db<Synchronizer>(TRC) << "~Semaphore(this=" << this << ")" << endl;
}


void Semaphore::p()
{
    db<Synchronizer>(TRC) << "Semaphore::p(this=" << this << ",value=" << _value << ")" << endl;

    begin_atomic();
    if(fdec(_value) < 1) {
        // Não obtem o semaforo
        // Percorro toda a lista de owners vendo se a thread que tentou um p() tem prioridade maior que os owners
        for (auto it = _owners.begin(); it != _owners.end(); ++it) {
            Thread* owner = it->object();
            Thread* current = Thread::self();
            if (current->priority() > owner->priority()) {
                // Se tem elevo a prioridade do owner para ser igual a da thread que tentou o p()
                // TODO: Preciso percorrer a waiting e passar aqui a maior prioridade encontrada
                owner->priority_elevate(current->priority());
            }
        }
        sleep();
    } else {
        _owners.insert(Thread::self()->link_element());
    }
        
    end_atomic();
}


void Semaphore::v()
{
    db<Synchronizer>(TRC) << "Semaphore::v(this=" << this << ",value=" << _value << ")" << endl;

    begin_atomic();
    if(finc(_value) < 0) {
        wakeup();
    }
    Thread* current = Thread::self();
    current->priority_restore();
    _owners.remove(current->link_element());
        
    end_atomic();
}

__END_SYS
