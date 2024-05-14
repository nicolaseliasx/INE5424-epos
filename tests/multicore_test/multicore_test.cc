// EPOS Scheduler Test Program

#include <machine/display.h>
#include <time.h>
#include <synchronizer.h>
#include <process.h>

using namespace EPOS;

const int iterations = 10;

Mutex table;

Thread * phil[5];
Semaphore * chopstick[5];

OStream cout;

int philosopher(int n, int l, int c);

int main()
{
    table.lock();
    Display::clear();
    Display::position(0, 0);
    cout << "The Philosopher's Dinner Multicore!!:" << endl;
    cout << "CPUS= "<< CPU::cores() << endl;

    for(int i = 0; i < 5; i++)
        chopstick[i] = new Semaphore;

    phil[0] = new Thread(&philosopher, 0,  5, 28);
    phil[1] = new Thread(&philosopher, 1, 10, 44);
    phil[2] = new Thread(&philosopher, 2, 16, 42);
    phil[3] = new Thread(&philosopher, 3, 16, 16);
    phil[4] = new Thread(&philosopher, 4, 10, 18);

    cout << "Philosophers are alive and hungry!" << endl;

    Display::position(7, 44);
    cout << '/';
    Display::position(13, 44);
    cout << '\\';
    Display::position(16, 35);
    cout << '|';
    Display::position(13, 27);
    cout << '/';
    Display::position(7, 27);
    cout << '\\';
    Display::position(19, 0);

    cout << "The dinner is served ..." << endl;
    table.unlock();

    for(int i = 0; i < 5; i++) {
        int ret = phil[i]->join();
        table.lock();
        Display::position(20 + i, 0);
        cout << "Philosopher " << i << " ate " << ret << " times " << endl;
        table.unlock();
    }

    for(int i = 0; i < 5; i++)
        delete chopstick[i];
    for(int i = 0; i < 5; i++)
        delete phil[i];

    cout << "The end!" << endl;

    return 0;
}

int philosopher(int n, int l, int c)
{
    int first = (n < 4) ? n : 0;
    int second = (n < 4) ? n + 1 : 4;

    for(int i = iterations; i > 0; i--) {

        table.lock();
        Display::position(l, c);
        cout << "thinking CPU " << CPU::id();  // Mostra o ID da CPU ao lado de "thinking"
        table.unlock();

        Delay thinking(1000000);

        table.lock();
        Display::position(l, c);
        cout << " hungry CPU " << CPU::id();  // Mostra o ID da CPU ao lado de "hungry"
        table.unlock();

        chopstick[first]->p();   // pega o primeiro hashi
        chopstick[second]->p();  // pega o segundo hashi

        table.lock();
        Display::position(l, c);
        cout << " eating CPU " << CPU::id();  // Mostra o ID da CPU ao lado de "eating"
        table.unlock();

        Delay eating(500000);

        table.lock();
        Display::position(l, c);
        cout << " sate CPU " << CPU::id();  // Mostra o ID da CPU ao lado de "sate"
        table.unlock();

        chopstick[first]->v();   // libera o primeiro hashi
        chopstick[second]->v();  // libera o segundo hashi
    }

    table.lock();
    Display::position(l, c);
    cout << " done CPU " << CPU::id();  // Mostra o ID da CPU ao lado de "done"
    table.unlock();

    return iterations;
}
