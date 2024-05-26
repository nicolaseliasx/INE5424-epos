// EPOS Periodic Thread Component Test Program

#include <time.h>
#include <real-time.h>
#include <utility/geometry.h>

using namespace EPOS;

const unsigned int iterations = 100;

// high priority thread
const Milisecond period_a = 10;
const Milisecond wcet_a = 9;

// low priority thread
const Milisecond period_b = 80;
const Milisecond wcet_b = 20;

const Milisecond period_c = 30;
const Milisecond wcet_c = 20;

const Milisecond period_d = 60;
const Milisecond wcet_d = 40;

const Milisecond period_e = 40;
const Milisecond wcet_e = 20;

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

int medium_priority_keeps_lock(Semaphore sem) {
    cout << "Im a medium priority thread and my core is: " << CPU::id() << endl;

    while(true);
    return 0;
}

int low_priority_first_lock(Semaphore sem) {
    cout << "Im a low priority thread and my core is: " << CPU::id() << endl;

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

    return 0;
}

int high_priority_later_lock(Semaphore sem) {
    cout << "Im a high priority thread and my core is: " << CPU::id() << endl;
    Delay wait_so_low_priority_thread_gets_lock(100000);

    sem.p();
    assert(very_important_value != 5);
    sem.v();

    return 0;
}

// Lets test if priority inversion works on PLLF
void set_up_priority_inversion(Semaphore sem) {
    // Milisecond elapsed = chrono.read() / 1000;
    // Milisecond end = elapsed + time;

    cout << "CPU cores: " << CPU::cores() << endl;

    thread_c = new Periodic_Thread(RTConf(period_c * 1000, 0, wcet_c * 1000, 0, iterations), &medium_priority_keeps_lock, readWrite);
    
    // cout << "C thread should have medium priority: " << thread_c->criterion() << endl;

    cout << "Next queue: " << Priority::next_queue() << " C queue: " << thread_c->criterion().queue() << endl;

    thread_a = new Periodic_Thread(RTConf(period_a * 1000, 0, wcet_a * 1000, 0, iterations), &high_priority_later_lock, readWrite);

    // cout << "A thread should have high priority: " << thread_a->criterion() << endl;
    // cout << "A thread should have high priority: " << thread_a->criterion() << endl;

    cout << "Next queue: " << Priority::next_queue() << " A queue: " << thread_a->criterion().queue() << endl;
    
    thread_b = new Periodic_Thread(RTConf(period_b * 1000, 0, wcet_b * 1000, 0, iterations), &low_priority_first_lock, readWrite);
    cout << "Next queue: " << Priority::next_queue() << " B queue: " << thread_b->criterion().queue() << endl;

    thread_d = new Periodic_Thread(RTConf(period_d * 1000, 0, wcet_d * 1000, 0, iterations), &medium_priority_keeps_lock, readWrite);
    cout << "Next queue: " << Priority::next_queue() << " D queue: " << thread_d->criterion().queue() << endl;

    thread_e = new Periodic_Thread(RTConf(period_e * 1000, 0, wcet_e * 1000, 0, iterations), &medium_priority_keeps_lock, readWrite);
    cout << "Next queue: " << Priority::next_queue() << " E queue: " << thread_e->criterion().queue() << endl;

    // cout << "B thread should have low priority: " << thread_b->criterion() << endl;

    // Lets confirm that thread a has higher priority than thread b (the lower the value the higher priority it has)
    assert((thread_a->criterion() < thread_c->criterion()));
    assert((thread_a->criterion() < thread_b->criterion()));

    // Lets confirm that the threads are in different cpus'
    // cout << "A queue: " << thread_a->criterion().queue() << " B queue: " << thread_b->criterion().queue() << " C queue: " << thread_c->criterion().queue() << endl;
    assert((thread_a->criterion().queue() != thread_b->criterion().queue()));
    assert((thread_a->criterion().queue() != thread_c->criterion().queue()));  

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
