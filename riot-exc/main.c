#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "thread.h"
#include "periph/gpio.h"
#include "irq.h"

char rcv_thread_stack[THREAD_STACKSIZE_MAIN];

int a1;
const int c1;
static int b1;
static const int d1;

void foo(void) {
    int a3;
    static int b3;
    static const int d3;
    printf("local to foo() is at %p\n\r", &a3);
    printf("static local to foo() is at %p\n\r", &b3);
    printf("Static const local to foo() is at %p\n\r\n", &d3);
}

void* rcv_thread(void* arg) {
    int a2;
    static int b2;
    static const int d2;
    printf("local to tread is at %p\n\r", &a2);
    printf("static local to tread is at %p\n\r", &b2);
    printf("Static const local to thread is at %p\n\r\n", &d2);
    foo();
    return arg;
}

void interruption(void* arg) {
    int a2;
    static int b2;
    static const int d2;
    printf("local to interruption is at %p\n\r", &a2);
    printf("static local to interruption is at %p\n\r", &b2);
    printf("Static const local to interruption is at %p\n\r\n", &d2);
    foo();
    (void)arg;
}

int main(void) {
    gpio_init_int(BTN0_PIN,
        GPIO_IN,
        GPIO_FALLING,
        interruption,
        NULL);
    int a2;
    static int b2;
    const int c2;
    static const int d2;
    printf("local to main() is at %p\n\r", &a2);
    printf("static local to main() is at %p\n\r", &b2);
    printf("Const local to main() is at %p\n\r", &c2);
    printf("Static const local to main() is at %p\n\r\n", &d2);

    printf("global is at %p\n\r", &a1);
    printf("static global is at %p\n\r", &b1);
    printf("global const %p\n\r", &c1);
    printf("Static global const is at %p\n\r", &d1);
    printf("func foo() is at %p\n\r\n", foo);
    /** todo: 
     * rcv_thread_stack + sizeof(rcv_thread_stack)
    */
    thread_create(rcv_thread_stack, sizeof(rcv_thread_stack),
        THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
        rcv_thread, NULL, "rcv_thread");
    foo();
    printf("&rcv_thread_stack: %p\n\r", &rcv_thread_stack);
    printf("sizeof(rcv_thread_stack): %u\n\r", sizeof(rcv_thread_stack));
    printf("sum: %p\n\r", &rcv_thread_stack[sizeof(rcv_thread_stack)]);
    gpio_irq_enable(BTN0_PIN);
    return 0;
}
