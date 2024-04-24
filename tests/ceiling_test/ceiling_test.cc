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

const unsigned int period_b = 80; // ms
const unsigned int period_c = 60; // ms
const unsigned int period_d = 120; // ms
const unsigned int period_e = 55; // ms

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
 // prioridade
 // espera
 //  priodade dnv eh pra ta as duas com a prioridade da delay
    cout << "\n" << "Im a LOW thread " << c << " and im get the semaphore" << endl;
    if(c == 'B')
        cout << "\n" << "Thread B priority " << thread_b->priority() << endl;
    if(c == 'C')
        cout << "\n" << "Thread C priority " << thread_c->priority() << endl;

    position.p();

    Delay waiting_ceiling(10000);

    cout << "\n" << "PRIORITY ELEVATED" << endl;
    if(c == 'B')
        cout << "\n" << "Thread B priority " << thread_b->priority() << endl;
    if(c == 'C')
        cout << "\n" << "Thread C priority " << thread_c->priority() << endl;

    position.v();
    cout << "\n" << "PRIORITY RESTORE" << endl;
    if(c == 'B')
        cout << "\n" << "Thread B priority " << thread_b->priority() << endl;
    if(c == 'C')
        cout << "\n" << "Thread C priority " << thread_c->priority() << endl;

    // speaker.lock();
    // cout << "Im " << c << "ceiling is done" << endl;
    // speaker.unlock();
}

inline void exec_delay(char c) {
    //tenta pegar o semaphorra
    cout << "\n" << "Im a HIGH thread " << c << " and im trying to get the semaphore" << endl;

    Delay waiting_semaphore(500);
    cout << "\n" << "Thread A priority " << thread_c->priority() << endl;
    position.p();
    Delay waiting(200);
    position.v();

    // speaker
    // cout << "Im " << c << ", my result is: " << result << " ! and Ive took: " << (chrono.read() / 1000 - elapsed) << "\n" << endl;
    // speaker.v();
}

int main()
{
    cout << "Periodic Thread Component Test" << endl;

    cout << "\nThis test consists in creating three periodic threads as follows:" << endl;
    cout << "Threads will now be created and I'll wait for them to finish..." << endl;

    // p,d,c,act,t
    // Parametros: period (us), deadline (us), capacity (us), activation (us), times
    thread_a = new Periodic_Thread(RTConf(period_a * 1000, period_a * 1000, 80 * 1000, 0, iterations), &func_a);
    thread_b = new Periodic_Thread(RTConf(period_b * 1000, period_b * 1000, 60 * 1000, 0, iterations), &func_b);
    thread_c = new Periodic_Thread(RTConf(period_c * 1000, period_c * 1000, 40 * 1000, 0, iterations), &func_c);

    chrono.start();

    int status_a = thread_a->join();
    int status_b = thread_b->join();
    int status_c = thread_c->join();

    chrono.stop();

    cout << "\n... done!" << endl;
    cout << "\n\nThread A exited with status \"" << char(status_a)
         << "\", thread B exited with status \"" << char(status_b)
         << "\" and thread C exited with status \"" << char(status_c) << "." << endl;

    cout << "\nThe estimated time to run the test was "
         << max(period_a, period_b, period_c) * iterations
         << " ms. The measured time was " << chrono.read() / 1000 <<" ms!" << endl;

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
