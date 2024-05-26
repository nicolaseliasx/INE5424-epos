// EPOS Periodic Thread Component Test Program

#include <time.h>
#include <real-time.h>
#include <utility/geometry.h>

using namespace EPOS;

const unsigned int iterations = 100;

// high priority thread
const Milisecond period_a = 50;
const Milisecond wcet_a = 50;

// low priority thread
const Milisecond period_b = 80;
const Milisecond wcet_b = 20;

const Milisecond period_c = 30;
const Milisecond wcet_c = 20;

int func_a();
int func_b();
int func_c();

OStream cout;
Chronometer chrono;

Semaphore readWrite;

volatile int very_important_value = 5;

Periodic_Thread * thread_a;
Periodic_Thread * thread_b;
Periodic_Thread * thread_c;
Periodic_Thread * thread_d;
Periodic_Thread * thread_e;

void medium_priority_keeps_lock(Semaphore sem) {
    while(true);
}

void low_priority_first_lock(Semaphore sem) {
    sem.p();

    assert(very_important_value == 5);

    int a = 0;
    for (int i = 0; i < 1000; i++) {
        very_important_value += i;
        a = a*i - (i + 1);
    }
    very_important_value += a;

    if (very_important_value == 5)
        very_important_value = 10;

    Delay wait_for_high_priority_thread(100000);

    sem.v();

    // ...
}

void high_priority_later_lock(Semaphore sem) {
    Delay wait_so_low_priority_thread_gets_lock(100000);

    sem.p();
    assert(very_important_value != 5);
    sem.v();
}

// Lets test if priority inversion works on PLLF
void set_up_priority_inversion(Semaphore sem) {
    Milisecond elapsed = chrono.read() / 1000;
    // Milisecond end = elapsed + time;

    thread_c = new Periodic_Thread(RTConf(period_a * 1000, 0, wcet_a * 1000, 0, iterations), &medium_priority_keeps_lock, readWrite);
    
    cout << "This thread should have medium priority: " << thread_c->criterion();

    thread_a = new Periodic_Thread(RTConf(period_a * 1000, 0, wcet_a * 1000, 0, iterations), &high_priority_later_lock, readWrite);

    cout << "This thread should have high priority: " << thread_a->criterion();

    thread_b = new Periodic_Thread(RTConf(period_b * 1000, 0, wcet_b * 1000, 0, iterations), &low_priority_first_lock, readWrite);

    cout << "This thread should have low priority: " << thread_b->criterion();

    // Lets confirm that thread a has higher priority than thread b (the lower the value the higher priority it has)
    assert((thread_a->criterion() < thread_c->criterion())); 
    // Lets confirm that the threads are in different cpus'
    assert((thread_a->criterion().queue() != thread_b->criterion().queue()));

    int status_a = thread_a->join();
    int status_b = thread_b->join();
    int status_c = thread_c->join();

    cout << "A: " << status_a << " B: " << status_b << " C: " << status_c;

    return;
}

int main()
{
    cout << "Periodic Thread Component Test" << endl;

    cout << "\nThis test consists in creating three periodic threads as follows:" << endl;
    cout << "- Every " << period_a << "ms, thread A executes \"a\" for " << wcet_a << "ms;" << endl;
    cout << "- Every " << period_b << "ms, thread B executes \"b\" for " << wcet_b << "ms;" << endl;
    cout << "- Every " << period_c << "ms, thread C executes \"c\" for " << wcet_c << "ms;" << endl;

    // cout << base_loop_count << " iterations per ms!" << endl;

    cout << "\nThreads will now be created and I'll wait for them to finish..." << endl;

    set_up_priority_inversion(readWrite);

    cout << "I'm also done, bye!" << endl;

    return 0;
}
