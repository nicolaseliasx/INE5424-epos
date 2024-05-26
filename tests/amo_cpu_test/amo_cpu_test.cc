// EPOS Generic CPU Test Program

#include <architecture/cpu.h>
#include <time.h>
#include <real-time.h>

using namespace EPOS;

const unsigned int iterations = 200;
const Milisecond period = 100;  // Unified period for simplicity
const Milisecond wcet = 100;     // Worst-case execution time for each thread

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

int execute(volatile int* n, volatile int* lock) {
    cout << "CPU test" << endl;

    CPU cpu;

    {
        cout << "CPU::tsl(l=false)\t=> ";

        volatile int lock = false;
        if(cpu.tsl(lock))
            cout << "failed [1]!" << endl;
        else
            if(cpu.tsl(lock))
                cout << "passed!" << endl;
            else
                cout << "failed [2]!" << endl;
    }
    {
        cout << "CPU::finc(n=100)\t=> ";

        volatile int number = 1000;
        volatile int tmp;
        if((tmp = cpu.finc(number)) != 1000)
            cout << "failed (n=" << tmp << ", should be 100)!" << endl;
        else
            if((tmp = cpu.finc(number)) != 1001)
                cout << "failed (n=" << tmp << ", should be 101)!" << endl;
            else
                cout << "passed!" << endl;
    }
    {
        cout << "CPU::fdec(n=100)\t=> ";

        volatile int number = 1000;
        volatile int tmp;
        if((tmp = cpu.fdec(number)) != 1000)
            cout << "failed (n=" << tmp << ", should be 100)!" << endl;
        else
            if((tmp = cpu.fdec(number)) != 999)
                cout << "failed (n=" << tmp << ", should be 99)!" << endl;
            else
                cout << "passed!" << endl;
    }
    {
        cout << "CPU::cas(n=2000)\t\t=> ";

        volatile int compare = *n;
        volatile int replacement = *n - 1;
        volatile int tmp;
        if((tmp = cpu.cas(*n, *lock,  compare, replacement)) != compare)
            cout << "failed [1] (n=" << tmp << ", should be " << compare << ")!" << endl;
        else
            if((tmp = cpu.cas(*n, *lock, compare, replacement)) != replacement)
                cout << "failed [2] (n=" << tmp << ", should be " << replacement << ")!" << endl;
            else
                cout << "passed!" << endl;
    }

    cout << "I'm done, bye!" << endl;

    return 0;
}

int main()
{
    volatile int number = 2000;
    volatile int lock = 0;

    chrono.start();

    for (int i = 0; i < 10; i++) {
        thread_a = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &execute, &number, &lock);
        thread_b = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &execute, &number, &lock);
        thread_c = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &execute, &number, &lock);
        thread_d = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &execute, &number, &lock);
        thread_e = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &execute, &number, &lock);
        thread_f = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &execute, &number, &lock);
        thread_g = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &execute, &number, &lock);
        thread_h = new Periodic_Thread(RTConf(period * 1000, 0, wcet * 1000, 0, iterations), &execute, &number, &lock);
    }

    execute(&number, &lock);

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

    cout << "\nFor this test with " << CPU::cores() << " cores, the measured execution time is " << chrono.read() / 1000 << " ms!" << endl;

    return 0;
}
