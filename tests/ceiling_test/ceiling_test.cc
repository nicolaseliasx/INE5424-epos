#include <time.h>
#include <real-time.h>
#include <machine/display.h>
#include <synchronizer.h>
#include <process.h>

using namespace EPOS;

const unsigned int iterations = 10;
const unsigned int period_a = 100;
const unsigned int period_b = 150;
const unsigned int period_c = 200;

const unsigned int wcet_a = 50;
const unsigned int wcet_b = 30;
const unsigned int wcet_c = 20;

int func_a();
int func_b();
int func_c();
long max(unsigned int a, unsigned int b, unsigned int c) { return ((a >= b) && (a >= c)) ? a : ((b >= a) && (b >= c) ? b : c); }

Semaphore position(1);

OStream cout;
Chronometer chrono;
Periodic_Thread * thread_a;
Periodic_Thread * thread_b;
Periodic_Thread * thread_c;

int main() {
    cout << "Iniciando teste de protocolo de teto de prioridade" << endl;

    thread_a = new Periodic_Thread(RTConf(period_a * 1000, period_a * 1000, wcet_a * 1000, 0, iterations), &func_a);
    thread_b = new Periodic_Thread(RTConf(period_b * 1000, period_b * 1000, wcet_b * 1000, 0, iterations), &func_b);
    thread_c = new Periodic_Thread(RTConf(period_c * 1000, period_c * 1000, wcet_c * 1000, 0, iterations), &func_c);

    thread_a->join();
    thread_b->join();
    thread_c->join();

    cout << "Teste completo." << endl;

    return 0;
}

int func_a() {
    cout << "Thread A (alta prioridade) tentando adquirir o semáforo." << endl;
    Alarm::delay(200000);
    thread_a->priority_elevate(0);
    position.p();
    cout << "Thread A adquiriu o semáforo." << endl;
    position.v();
    cout << "Thread A liberou o semáforo." << endl;
    return 'A';
}

int func_b() {
    cout << "Thread B (média prioridade) tentando adquirir o semáforo." << endl;
    position.p();
    cout << "Thread B adquiriu o semáforo." << endl;
    Alarm::delay(50000); // Delays to ensure it starts after A has started but before A gets the semaphore
    position.v();
    cout << "Thread B liberou o semáforo." << endl;
    return 'B';
}

int func_c() {
    cout << "Thread C (baixa prioridade) tentando adquirir o semáforo." << endl;
    position.p();
    cout << "Thread C adquiriu o semáforo." << endl;
    Alarm::delay(100000); // Delays to ensure it starts after B
    position.v();
    cout << "Thread C liberou o semáforo." << endl;
    return 'C';
}
