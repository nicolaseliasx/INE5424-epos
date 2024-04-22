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
    // TODO: Todos os parametros são convertidos para ticks ????
    Real_Time_Scheduler_Common((deadline - capacity), Alarm::ticks(deadline),  Alarm::ticks(period),  Alarm::ticks(capacity)) {}

void LLF::update() {
    // TODO: A DIF ENTRE O TEMPO _start - Alarm::elapsed() CASO NÃO SEJA MAIOR QUE _capacity DEVE SER ATUALIZADO
    // TODO: Segundo tulio perca de deadline nao deve ser levada em conta pq teoricamente um llf nunca perde deadline
    // TODO: SO DEVO ENTRAR AQUI CASO A THREAD EM QUESTAO ACONTECEU UMA TROCA DE CONTEXTO E ELA PAROU DE EXECUTAR
    if (_finished_execution) {
        _capacity = _capacity - (_start - Alarm::elapsed());
        // TODO: ONDE DEVO ATUALIZAR ISSO CORRETAMENTE TODAS AS CLASSES DEVEM IMPLEMENTAR ISSO? PRECISO TER ISSO EM COMUM PRA SE TAR NAS TROCAS DE CONTEXTO
        _finished_execution = false;
    }
    // TODO: Criar verificacao se acabou a execucao de seu deadline e atualizar a capacity caso não tenha acabado
    if((_priority >= PERIODIC) && (_priority < APERIODIC))
        _priority = _deadline - (Alarm::elapsed() + _capacity);
}

void LLF::start_execution() {
    _start = Alarm::elapsed();
}


// Since the definition of FCFS above is only known to this unit, forcing its instantiation here so it gets emitted in scheduler.o for subsequent linking with other units is necessary.
template FCFS::FCFS<>(int p);

__END_SYS
