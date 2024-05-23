// EPOS Spin Lock Utility Declarations

#ifndef __priority_stack_h
#define __priority_stack_h

#include <architecture.h>

__BEGIN_UTIL

// Definimos essa classe pois não consegui a tempo usar um list do EPOS de inteiros ou de criterios e usamos essa estrutura para fins
// de demonstrar o funcionamento dessa arquitetura de um historico de prioridades em forma de pilha para suporte de semaforos alinhados
class Priority_Stack {
private:
    int stack[20];
    int top;
public:
    Priority_Stack() : top(-1) {}
    void push(int value) {
        if (top < 19) {
            top++;
            stack[top] = value;
        }
    }
    int pop() {
        if (top >= 0) {
            int value = stack[top];
            top--;
            return value;
        }
        return -1;
    }
    int peek() {
        if (top >= 0) {
            return stack[top];
        }
        return -1;
    }

    bool empty() {
        return top == -1;
    }

    bool size() {
        return top;
    }
};

__END_UTIL

#endif
