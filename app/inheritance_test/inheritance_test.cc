#include <time.h>
#include <real-time.h>
#include <machine/display.h>
#include <synchronizer.h>
#include <process.h>

using namespace EPOS;

const unsigned int iterations = 10;
// Periods (smaller period indicates higher priority under rate-monotonic scheduling)
const unsigned int period_a = 60;  // Highest priority
const unsigned int period_b = 150;
const unsigned int period_c = 200;

// Worst-case execution times (WCET)
const unsigned int wcet_a = 50;
const unsigned int wcet_b = 30;
const unsigned int wcet_c = 20;

int func_a();
int func_b();
int func_c();

Semaphore position(2);  // Semáforo com duas posições
Semaphore nested_position(2);

OStream cout;
Chronometer chrono;
Periodic_Thread *thread_a;
Periodic_Thread *thread_b;
Periodic_Thread *thread_c;

int main() {
    cout << "Start ceiling protocol test" << endl;

    thread_a = new Periodic_Thread(RTConf(period_a * 1000, period_a * 1000, wcet_a * 1000, 0, iterations), &func_a);
    thread_b = new Periodic_Thread(RTConf(period_b * 1000, period_b * 1000, wcet_b * 1000, 0, iterations), &func_b);
    thread_c = new Periodic_Thread(RTConf(period_c * 1000, period_c * 1000, wcet_c * 1000, 0, iterations), &func_c);

    thread_c->join();
    thread_b->join();
    thread_a->join();

    cout << "\nThread A time in all cores: " << endl;
    for (unsigned int i = 0; i < CPU::cores(); i ++)
        cout << "\n In Core " << i << " : " << thread_a->criterion().statistics().execution_per_cpu[i] << endl;

    cout << "\nThread B time in all cores: " << endl;
    for (unsigned int i = 0; i < CPU::cores(); i ++)
        cout << "\n In Core " << i << " : " << thread_b->criterion().statistics().execution_per_cpu[i] << endl;

    cout << "\nThread C time in all cores: " << endl;
    for (unsigned int i = 0; i < CPU::cores(); i ++)
        cout << "\n In Core " << i << " : " << thread_c->criterion().statistics().execution_per_cpu[i] << endl;

    cout << "\nYou need to analyze the priority for Thread B after the message A trying to acquire the semaphore" << endl;
    cout << "you will see thread B ceiling aplicated equals the priority of A" << endl;

    cout << "\nEND TEST" << endl;

    return 0;
}

void print_priorities(const char *label) {
    cout << label << ": Thread A: " << thread_a->priority()
         << ", Thread B: " << thread_b->priority()
         << ", Thread C: " << thread_c->priority() << endl;
}

int func_a() {
    // Aguarda as outras threads menos prioritarias
    Alarm::delay(100000);
    // A ideia eh colocar simuladamente a thread A em uma prioridade extremamente alta e alinhado com os delays
    // temos uma inversao de prioridade acontecendo onde a thread B e C de prioridades mais baixar entram no semaphore
    // de duas posicoes antes da thread A na linha de baixo com a maior prioridade possivel enquanto ambas as threads
    // imprimem suas prioridades podendo visualizar o celing acontecendo as threads B e C de dentro do semaphore ficarao com 
    // suas prioridades sendo herdadas
    print_priorities("A trying to acquire the position semaphore");

    position.p();    
    nested_position.p();
    
    print_priorities("A acquired nested and position semaphores");
    
    nested_position.v();
    position.v();

    for (int i = 0; i < 70; ++i) {
        print_priorities("A outside the semaphores");
        Alarm::delay(5000);
    }
    return 'A';
}
int func_b() {
    position.p();
    nested_position.p();
    for (int i = 0; i < 70; ++i) {
        print_priorities("B inside");
        Alarm::delay(5000);
    }
    nested_position.v();
    position.v();
    return 'B';
}

int func_c() {
    position.p();
    nested_position.p();
    for (int i = 0; i < 70; ++i) {
        print_priorities("C inside");
        Alarm::delay(5000);
    }
    nested_position.v();
    position.v();
    return 'C';
}
