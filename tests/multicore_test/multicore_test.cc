#include <time.h>
#include <real-time.h>
#include <utility/geometry.h>

using namespace EPOS;

const unsigned int iterations = 200;
const Milisecond period = 100;  // Unified period for simplicity
const Milisecond wcet = 100;     // Worst-case execution time for each thread

int func_a();
int func_b();
int func_c();
int func_d();
int func_e();
int func_f();
int func_g();
int func_h();

OStream cout;
Chronometer chrono;

// Expanding to 8 threads
Periodic_Thread * thread_a;
Periodic_Thread * thread_b;
Periodic_Thread * thread_c;
Periodic_Thread * thread_d;
Periodic_Thread * thread_e;
Periodic_Thread * thread_f;
Periodic_Thread * thread_g;
Periodic_Thread * thread_h;

Point<long, 2> p, p1(2131231, 123123), p2(2, 13123), p3(12312, 123123);

unsigned long base_loop_count;

void calibrate() {
    chrono.start();
    Microsecond end = chrono.read() + Microsecond(1000000UL);

    base_loop_count = 0;
    while(chrono.read() < end) {
        p = p + Point<long, 2>::trilaterate(p1, 123123, p2, 123123, p3, 123123);
        base_loop_count++;
    }

    chrono.stop();
    base_loop_count /= 1000;
}

inline void exec(char c, Milisecond time = 0) {
    Milisecond elapsed = chrono.read() / 1000;
    Milisecond end = elapsed + time;
    while(elapsed < end) {
        for(unsigned long i = 0; i < time; i++)
            for(unsigned long j = 0; j < base_loop_count; j++) {
                p = p + Point<long, 2>::trilaterate(p1, 123123, p2, 123123, p3, 123123);
            }
        elapsed = chrono.read() / 1000;
        cout << "\n" << elapsed << "Thread running = " << c << end;
    }
}

int main() {
    cout << "Multicore with " << CPU::cores() << " cores Component Test" << endl;
    cout << "\nThis test now involves creating eight periodic threads." << endl;
    cout << "\nCalibrating the duration of the base execution loop: ";
    calibrate();
    cout << base_loop_count << " iterations per ms!" << endl;
    cout << "\nThreads will now be created and I'll wait for them to finish..." << endl;

    // Creating threads for each function
    thread_a = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &func_a);
    thread_b = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &func_b);
    thread_c = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &func_c);
    thread_d = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &func_d);
    thread_e = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &func_e);
    thread_f = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &func_f);
    thread_g = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &func_g);
    thread_h = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &func_h);

    chrono.reset();
    chrono.start();

    // Waiting for all threads to finish
    int status_a = thread_a->join();
    int status_b = thread_b->join();
    int status_c = thread_c->join();
    int status_d = thread_d->join();
    int status_e = thread_e->join();
    int status_f = thread_f->join();
    int status_g = thread_g->join();
    int status_h = thread_h->join();

    chrono.stop();

    cout << "\n... done!" << endl;
    cout << "\nThread A exited with status \"" << char(status_a)
         << "\", thread B with \"" << char(status_b)
         << "\", thread C with \"" << char(status_c)
         << "\", thread D with \"" << char(status_d)
         << "\", thread E with \"" << char(status_e)
         << "\", thread F with \"" << char(status_f)
         << "\", thread G with \"" << char(status_g)
         << "\", and thread H with \"" << char(status_h) << "." << endl;
    
    cout << "\nThe average execution time with 1 core is approximately 643.8 ms. For this test with 4 cores, the measured execution time is " << chrono.read() / 1000 << " ms!" << endl;
    
    cout << "I'm also done, bye!" << endl;

    return 0;
}


int func_a()
{
    exec('a', wcet);

    return 'A';
}

int func_b()
{

    exec('b', wcet);

    return 'B';
}

int func_c()
{
    exec('c', wcet);

    return 'C';
}

int func_d()
{
    exec('d', wcet);
    return 'D';
}

int func_e()
{
    exec('e', wcet);
    return 'E';
}

int func_f()
{
    exec('f', wcet);
    return 'F';
}

int func_g()
{
    exec('g', wcet);
    return 'G';
}

int func_h()
{
    exec('h', wcet);
    return 'H';
}
