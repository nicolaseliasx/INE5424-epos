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

Semaphore calculator(3);
Semaphore speaker(3);

OStream cout;
Chronometer chrono;
Periodic_Thread * thread_a;
Periodic_Thread * thread_b;
Periodic_Thread * thread_c;
Periodic_Thread * thread_d;
Periodic_Thread * thread_e;

inline void exec(char c, int x, int y, unsigned int time = 0) // in miliseconds
{
    //Delay waiting(1000);
    Microsecond elapsed = chrono.read() / 1000;

    if (c == 'M') {
        cout << "\n" << elapsed << "\t" << c
            << "\t[p(A)=" << thread_a->priority()
            << ", p(B)=" << thread_b->priority()
            << ", p(C)=" << thread_c->priority()
            << ", p(D)=" << thread_d->priority()
            << ", p(E)=" << thread_e->priority() << "]" << "\n" << endl;
    } else {
        cout << "\n" << elapsed << "\t"
            << "\t[p(" << c << ")=" << thread_a->priority() << "]" << "\n" << endl;
    }
    
    //calculator.lock();
    
    if (c != 'M') {
        cout << "Im " << c << " and im trying to get the semaphore\n" << endl;
        calculator.p();

        int result = x;

        for (int i = 1; i < 10000; i++) {
            // Some random calculations to pass the time
            // int res_x = result + y;
            // int res_y = (x - y)*0.26 + (x - y)*i*0.15 - res_x;
            // result = res_y - res_x;

            Delay thinking_about_calculation(1000);
            result = x + y * i - i*(i-x*y);
        }

        //calculator.unlock();
        calculator.v();

        speaker.p();
        cout << "Im " << c << ", my result is: " << result << " ! and Ive took: " << (chrono.read() / 1000 - elapsed) << "\n" << endl;
        speaker.v();
    }
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
    // TODO: ARTHUR TESTAR COM PRIORIDADES DIFERENTES
    // TODO: TEM QUE PARAR COM BASTANTE CALMA UM DOCINHO DO LADO E RABISCAR O QUE DEVERIA ACONTECER COLOCAR MENOS THREADS QUE REALMENTE TESTEM A IONVERSAO ACONTECENDO
    // TODO: COLOCAR POR EXEMPLO ESSA MESMA CONTA AI, POREM DOIS METODOS DIFERENTES QUE CONSOMEM O MESMO SEMAPHORE
    // Parametros: period (us), deadline (us), capacity (us), activation (us), times
    // TODO: COLOQUEI VALORES ALETATORIOS SO PRA VER SE IA QUEBRAR

    // Add activation jitter so it doesnt get scheduled as soon as threads are created
    thread_a = new Periodic_Thread(RTConf(period_a * 100, period_a * 100, 100, 10000, iterations), &func_a);
    thread_b = new Periodic_Thread(RTConf(period_b * 100, period_b * 100, 120, 0, iterations), &func_b);
    thread_c = new Periodic_Thread(RTConf(period_c * 100, period_b * 100, 130, 0, iterations), &func_c);
    thread_d = new Periodic_Thread(RTConf(period_c * 100, period_b * 100, 140, 0, iterations), &func_d);
    thread_e = new Periodic_Thread(RTConf(period_c * 100, period_b * 100, 150, 0, iterations), &func_e);

    // thread_a = new Periodic_Thread(RTConf(period_a * 1000, period_a * 1000, 100, 0, iterations), &func_a);
    // thread_b = new Periodic_Thread(RTConf(period_b * 1000, period_b * 1000, 120, 0, iterations), &func_b);
    // thread_c = new Periodic_Thread(RTConf(period_c * 1000, period_c * 1000, 130, 0, iterations), &func_c);
    // thread_d = new Periodic_Thread(RTConf(period_d * 1000, period_d * 1000, 140, 0, iterations), &func_d);
    // thread_e = new Periodic_Thread(RTConf(period_e * 1000, period_e * 1000, 150, 0, iterations), &func_e);

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
