// EPOS Periodic Thread Component Test Program

#include <time.h>
#include <real-time.h>

using namespace EPOS;

const unsigned int iterations = 100;
const unsigned int period_a = 100; // ms
const unsigned int period_b = 80; // ms
const unsigned int period_c = 60; // ms
const unsigned int a_factorial = 13;
const unsigned int b_factorial = 10;
const unsigned int c_factorial = 8;

int func_a();
int func_b();
int func_c();
long max(unsigned int a, unsigned int b, unsigned int c) { return ((a >= b) && (a >= c)) ? a : ((b >= a) && (b >= c) ? b : c); }

OStream cout;
Chronometer chrono;
Periodic_Thread * thread_a;
Periodic_Thread * thread_b;
Periodic_Thread * thread_c;

// Função para calcular o fatorial de um número (de forma não-recursiva)
unsigned long factorial(unsigned int n) {
    unsigned long result = 1;
    for (unsigned int i = 2; i <= n; ++i)
        result *= i;
    return result;
}

inline void exec(char c, unsigned int time = 0) {
    Microsecond elapsed = chrono.read() / 1000;

    cout << "\n" << elapsed << "\t" << c
         << "\t[p(A)=" << thread_a->priority()
         << ", p(B)=" << thread_b->priority()
         << ", p(C)=" << thread_c->priority() << "]";

    if (time) {
        unsigned long result = factorial(time);  // time será usado como um argumento para o fatorial

        // Logando o resultado para garantir que o cálculo não seja otimizado fora
        cout << " - Factorial(" << time << ") = " << result;
    }
}



int main()
{
    cout << "Periodic Thread Component Test" << endl;

    cout << "\nThis test consists in creating three periodic threads as follows:" << endl;
    cout << "- Every " << period_a << "ms, thread A execs \"a\", calculate factorial for " << a_factorial << " and then execs another \"a\";" << endl;
    cout << "- Every " << period_b << "ms, thread B execs \"b\", calculate factorial for " << b_factorial << " and then execs another \"b\";" << endl;
    cout << "- Every " << period_c << "ms, thread C execs \"c\", calculate factorial for " << c_factorial << " and then execs another \"c\";" << endl;

    cout << "Threads will now be created and I'll wait for them to finish..." << endl;

    // p,d,c,act,t
    // func_a (fatorial de 12): ~50ms
    // func_b (fatorial de 10): ~20ms
    // func_c (fatorial de 8): ~10ms
    // Parametros: period (us), deadline (us), capacity (us), activation (us), times
    thread_a = new Periodic_Thread(RTConf(period_a * 1000, period_a * 1000, 80 * 1000, 0, iterations), &func_a);
    thread_b = new Periodic_Thread(RTConf(period_b * 1000, period_b * 1000, 60 * 1000, 0, iterations), &func_b);
    thread_c = new Periodic_Thread(RTConf(period_c * 1000, period_c * 1000, 40 * 1000, 0, iterations), &func_c);

    exec('M');

    chrono.start();

    int status_a = thread_a->join();
    int status_b = thread_b->join();
    int status_c = thread_c->join();

    chrono.stop();

    exec('M');

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
    exec('A');

    do {
        exec('a', a_factorial);
    } while (Periodic_Thread::wait_next());

    exec('A');

    return 'A';
}

int func_b()
{
    exec('B');

    do {
        exec('b', b_factorial);
    } while (Periodic_Thread::wait_next());

    exec('B');

    return 'B';
}

int func_c()
{
    exec('C');

    do {
        exec('c', c_factorial);
    } while (Periodic_Thread::wait_next());

    exec('C');

    return 'C';
}
