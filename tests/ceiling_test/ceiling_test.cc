// EPOS Periodic Thread Component Test Program

#include <time.h>
#include <real-time.h>
#include <machine/display.h>
#include <synchronizer.h>
#include <process.h>

using namespace EPOS;

const unsigned int iterations = 100;
const unsigned int period_a = 100; // ms
const unsigned int period_b = 80; // ms
const unsigned int period_c = 60; // ms
const unsigned int period_d = 120; // ms
const unsigned int period_e = 55; // ms

const unsigned int wcet_a = 50; // ms
const unsigned int wcet_b = 20; // ms
const unsigned int wcet_c = 10; // ms
const unsigned int wcet_d = 60; // ms
const unsigned int wcet_e = 8; // ms


int func_a();
int func_b();
int func_c();
long max(unsigned int a, unsigned int b, unsigned int c) { return ((a >= b) && (a >= c)) ? a : ((b >= a) && (b >= c) ? b : c); }

Mutex calculator;
Semaphore speaker;

OStream cout;
Chronometer chrono;
Periodic_Thread * thread_a;
Periodic_Thread * thread_b;
Periodic_Thread * thread_c;
Periodic_Thread * thread_d;
Periodic_Thread * thread_e;

inline void exec(char c, int x, int y, unsigned int time = 0) // in miliseconds
{
    Microsecond elapsed = chrono.read() / 1000;

    cout << "\n" << elapsed << "\t" << c
    << "\t[p(A)=" << thread_a->priority()
    << ", p(B)=" << thread_b->priority()
    << ", p(C)=" << thread_c->priority()
    << ", p(D)=" << thread_d->priority()
    << ", p(E)=" << thread_e->priority() << "]";

    calculator.lock();

    int result = x;

    for (int i = 1; i < 10; i++) {
        result += y
    }

    calculator.unlock();

    speaker.p();
    cout << "my result is: " << result << " ! and Ive took: " << (chrono.read() / 1000 - elapsed);
    speaker.v();
}


int main()
{
    cout << "Periodic Thread Component Test" << endl;

    cout << "\nThis test consists in creating three periodic threads as follows:" << endl;
    cout << "- Every " << period_a << "ms, thread A execs \"a\", waits for " << wcet_a << "ms and then execs another \"a\";" << endl;
    cout << "- Every " << period_b << "ms, thread B execs \"b\", waits for " << wcet_b << "ms and then execs another \"b\";" << endl;
    cout << "- Every " << period_c << "ms, thread C execs \"c\", waits for " << wcet_c << "ms and then execs another \"c\";" << endl;
    cout << "- Every " << period_d << "ms, thread D execs \"d\", waits for " << wcet_d << "ms and then execs another \"d\";" << endl;
    cout << "- Every " << period_e << "ms, thread E execs \"e\", waits for " << wcet_e << "ms and then execs another \"e\";" << endl;

    cout << "Threads will now be created and I'll wait for them to finish..." << endl;

    // p,d,c,act,t
    thread_a = new Periodic_Thread(RTConf(period_a * 100, 0, 0, 0, iterations), &func_a);
    thread_b = new Periodic_Thread(RTConf(period_b * 100, 0, 0, 0, iterations), &func_b);
    thread_c = new Periodic_Thread(RTConf(period_c * 100, 0, 0, 0, iterations), &func_c);
    thread_d = new Periodic_Thread(RTConf(period_c * 100, 0, 0, 0, iterations), &func_d);
    thread_e = new Periodic_Thread(RTConf(period_c * 100, 0, 0, 0, iterations), &func_e);

    thread_a = new Periodic_Thread(RTConf(period_a * 1000, period_a * 1000, wcet_a, 0, iterations), &func_a);
    thread_b = new Periodic_Thread(RTConf(period_b * 1000, period_b * 1000, wcet_b, 0, iterations), &func_b);
    thread_c = new Periodic_Thread(RTConf(period_c * 1000, period_c * 1000, wcet_c, 0, iterations), &func_c);
    thread_d = new Periodic_Thread(RTConf(period_d * 1000, period_d * 1000, wcet_d, 0, iterations), &func_d);
    thread_e = new Periodic_Thread(RTConf(period_e * 1000, period_e * 1000, wcet_e, 0, iterations), &func_e);

    exec('M', 18942, 29384);

    chrono.start();

    int status_a = thread_a->join();
    int status_b = thread_b->join();
    int status_c = thread_c->join();

    chrono.stop();

    exec('M', 18942, 29384);

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

int func_a()
{
    exec('A', 123123, 1256384);

    do {
        exec('a', 123123, 1256384, wcet_a);
    } while (Periodic_Thread::wait_next());

    exec('A', 123123, 1256384);

    return 'A';
}

int func_b()
{
    exec('B', 123123, 1256384);

    do {
        exec('b', 123123, 1256384, wcet_b);
    } while (Periodic_Thread::wait_next());

    exec('B', 123123, 1256384);

    return 'B';
}

int func_c()
{
    exec('C', 123123, 1256384);

    do {
        exec('c', 123123, 1256384, wcet_c);
    } while (Periodic_Thread::wait_next());

    exec('C', 123123, 1256384);

    return 'C';
}

int func_d()
{
    exec('D', 123123, 1256384);

    do {
        exec('d', 123123, 1256384, wcet_d);
    } while (Periodic_Thread::wait_next());

    exec('D', 123123, 1256384);

    return 'D';
}

int func_e()
{
    exec('E', 123123, 1256384);

    do {
        exec('d', 123123, 1256384, wcet_e);
    } while (Periodic_Thread::wait_next());

    exec('E', 123123, 1256384);

    return 'E';
}
