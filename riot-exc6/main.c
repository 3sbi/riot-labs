#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"

#define N (uint8_t)0
#define P (uint8_t)1
#define D (uint8_t)2

#define DOT 500 // ms
#define DASH 3*DOT
#define LETTERS_SPACE DOT*3

#define MORSE(A1, A2, A3, A4) ((A1<<6)|(A2<<4)|(A3<<2)|(A4))

uint8_t codes[26] = {
    MORSE(P, D, N, N), // A
    MORSE(D, P, P, P), // B
    MORSE(D, P, D, P), // C
    MORSE(D, P, P, N), // D
    MORSE(P, N, N, N), // E
    MORSE(P, P, D, P), // F
    MORSE(D, D, P, N), // G
    MORSE(P, P, P, P), // H
    MORSE(P, P, N, N), // I
    MORSE(P, D, D, D), // J
    MORSE(D, P, D, N), // K
    MORSE(P, D, P, P), // L
    MORSE(D, D, N, N), // M
    MORSE(D, P, N, N), // N
    MORSE(D, D, D, N), // O
    MORSE(P, D, D, P), // P
    MORSE(D, D, P, D), // Q
    MORSE(P, D, P, N), // R
    MORSE(P, P, P, N), // S
    MORSE(D, N, N, N), // T
    MORSE(P, P, D, N), // U
    MORSE(P, P, P, D), // V
    MORSE(P, D, D, N), // W
    MORSE(D, P, P, D), // X
    MORSE(D, P, D, D), // Y
    MORSE(D, D, P, P), // Z
};

void printDot(void);

void printDash(void);

void printLetter(char l);


int main(void) {
    // -1 = nothing,
    // 0 = dot
    // 1 = dash

    printf("Hello %d\n", MORSE(P, P, P, N));
    
    char str[] = "Helloworld";
    gpio_init(LED0_PIN, GPIO_OUT);
    while(1) {
        printf("Main loop\n");
        int str_len = (int)(sizeof(str)/sizeof(char));
        printf("String len: %d\n", str_len);
        for (int i = 0; i < str_len; i++) {
            printf("%c\n", str[i]);
            printLetter(str[i]);
            printf("\n");
        }
        xtimer_sleep(5000);
    }

    return 0;
}

void printDot(void) {
    gpio_set(LED0_PIN);
    xtimer_msleep(DOT);
    gpio_clear(LED0_PIN);
}

void printDash(void) {
    gpio_set(LED0_PIN);
    xtimer_msleep(DASH);
    gpio_clear(LED0_PIN);
}

void printLetter(char l) {
    char upper = toupper(l);
    uint8_t code = codes[(int)(upper - 'A')];
    for (int i = 0; i < 4; i++) {
        uint8_t symbol = (code >> ((3-i)*2)) & 0b00000011; // 00000011 
        if (symbol == P) {
            printDot();
            printf(".\n");
        }
        if (symbol == D) {
            printDash();
            printf("-\n");
        }
        if (i < 3 && symbol != N) {
            xtimer_msleep(DOT);
        }
        else break;
    }

    xtimer_msleep(LETTERS_SPACE);
}
