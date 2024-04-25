// EPOS Periodic Thread Component Test Program

#include <time.h>
#include <real-time.h>
#include <machine/display.h>
#include <synchronizer.h>
#include <process.h>

using namespace EPOS;

const unsigned int iterations = 100;

// A should be highest priority - Menor diferença entre periodo e WCET
const unsigned int period_a = 85; // ms

const unsigned int period_b = 55; // ms
const unsigned int period_c = 60; // ms
const unsigned int period_d = 120; // ms

const unsigned int wcet_a = 70; // ms

const unsigned int wcet_b = 20; // ms
const unsigned int wcet_c = 10; // ms
const unsigned int wcet_d = 60; // ms
const unsigned int wcet_e = 8; // ms


int func_a();
int func_b();
int func_c();
int func_d();
int func_e();
long max(unsigned int a, unsigned int b, unsigned int c) { return ((a >= b) && (a >= c)) ? a : ((b >= a) && (b >= c) ? b : c); }

Semaphore position(2);
Mutex speaker();

OStream cout;
Chronometer chrono;
Periodic_Thread * thread_a;
Periodic_Thread * thread_b;
Periodic_Thread * thread_c;

inline void exec(char c) {
    // Duas threads tentam pegar o semaphore
    // Antes imprimem seuas prioridades
    // cout << "\n" << "Im a LOW thread " << c << " and im get the semaphore" << endl;
    if(c == 'B')
        cout << "\n" << "Thread B priority " << thread_b << " " << thread_b->priority() << endl;
    if(c == 'C')
        cout << "\n" << "Thread C priority " << thread_c << " " << thread_c->priority() << endl;
    
    position.p();
    // Ficam dentro esperando um tempo, ate a high chegar e elevar a prioridade das que estão dentro
    
    // Delay waiting_ceiling(100000);
  
    // Aqui eh pra prioridade da thread B e C ser elevada e imprimida igual a prioridade de A
    cout << "\n" << "PRIORITY ELEVATED" << endl;
    if(c == 'B')
        cout << "\n" << "Thread B priority " << thread_b->priority() << endl;
    if(c == 'C')
        cout << "\n" << "Thread C priority " << thread_c->priority() << endl;

    position.v();
    // Apois derem v eh pra prioridade ser restaurada
    cout << "\n" << "PRIORITY RESTORE" << endl;
    if(c == 'B')
        cout << "\n" << "Thread B priority " << thread_b->priority() << endl;
    if(c == 'C')
        cout << "\n" << "Thread C priority " << thread_c->priority() << endl;
}

inline void exec_delay(char c) {
    // Uma thread tenta pegar o semaphore
    cout << "\n" << "Im a HIGH thread " << c << " and im trying to get the semaphore" << endl;
    // Aguarda um tempo para que as outras threads entrem
    // Elevo para simular a chegada de uma thread de maior prioridade
    cout << "\n" << "Thread A priority " << thread_a << " " << thread_a->priority() << endl;
    thread_a->priority_elevate(-50000);

    // Delay waiting_semaphore(50000);
    
    // Imprime sua prioridade
    cout << "\n" << "Thread A priority " << thread_a->priority() << endl;
    position.p();
    

    // DELAY 2000

    position.v();
}

inline void exec_main_delay(char c) {
    cout << "\n" << "Im a MAIN thread " << c << " and im waiting to others threads end" << endl;
}

int main()
{
    cout << "Periodic Thread Component Test" << endl;

    cout << "\nThis test consists in creating three periodic threads as follows:" << endl;
    cout << "Threads will now be created and I'll wait for them to finish..." << endl;

    chrono.start();
    // p,d,c,act,t
    // Parametros: period (us), deadline (us), capacity (us), activation (us), times
    thread_b = new Periodic_Thread(RTConf(period_b * 1000, period_b * 1000, 60 * 1000, 0, iterations), &func_b);
    thread_c = new Periodic_Thread(RTConf(period_c * 1000, period_c * 1000, 40 * 1000, 0, iterations), &func_c);
    thread_a = new Periodic_Thread(RTConf(period_a * 1000, period_a * 1000, 35 * 1000, 0, iterations), &func_a);

    // Pra main esperar as threads terminarem
    exec_main_delay('M');

    

    int status_a = thread_a->join();
    int status_b = thread_b->join();
    int status_c = thread_c->join();

    chrono.stop();

    cout << "\n... done!" << endl;
    cout << "\n\nThread A exited with status \"" << char(status_a)
         << "\", thread B exited with status \"" << char(status_b)
         << "\" and thread C exited with status \"" << char(status_c) << "." << endl;

    cout << "I'm also done, bye!" << endl;

    return 0;
}

int func_a() {
    exec_delay('A');

    return 'A';
}

int func_b() {
    exec('B');

    return 'B';
}

int func_c() {
    exec('C');

    return 'C';
}
