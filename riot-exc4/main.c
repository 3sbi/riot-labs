#include <stdio.h>
#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"

#define VALID_KEY 0x36 // Валидный ключ 0312 1423
#define KEY_LENGTH 0x04 // Длина валидного ключа

#define BUTTON1_CODE 0 // Коды кнопок
#define BUTTON2_CODE 1
#define BUTTON3_CODE 2
#define BUTTON4_CODE 3

#define buttonS1 GPIO_PIN(PORT_A, 0)
#define buttonS2 GPIO_PIN(PORT_A, 1)
#define buttonS3 GPIO_PIN(PORT_A, 7)
#define buttonS4 GPIO_PIN(PORT_A, 6)

volatile uint8_t currentKeylength = 0; // Счетчик нажатий
volatile uint8_t currentInput = 0; // Буфер последовтельности нажатий

int main(void) {
    printf("Hello world\n\r");
    gpio_init(buttonS1, GPIO_IN);
    gpio_init(buttonS2, GPIO_IN);
    gpio_init(buttonS3, GPIO_IN);
    gpio_init(buttonS4, GPIO_IN);
    gpio_init(LED0_PIN, GPIO_OUT);
    /* Задача кейса */

    int b1 = 0;
    int b2 = 0;
    int b3 = 0;
    int b4 = 0;
    int inactivityTime = 0;
    int previousKey = 0;
    while (1) {
        // Опрашиваем состояние кнопок
        //==============================
        b1 = gpio_read(buttonS1);
        b2 = gpio_read(buttonS2);
        b3 = gpio_read(buttonS3);
        b4 = gpio_read(buttonS4);
        // printf("Buttons values: %d %d %d %d\r\n", b1, b2, b3, b4);

        if (!b1) {
            printf("b1\r\n");
            if (previousKey == 1) goto ignore;
            previousKey = 1;
            currentKeylength++; // Инкрементируем счетчик нажатий
            currentInput <<= 2;  // Сдвигаем последовательность кодов нажатий
            currentInput |= BUTTON1_CODE; // Записываем код очередного нажатия
            inactivityTime = 0;
        }
        else if (!b2) {
            printf("b2\r\n");
            if (previousKey == 2) goto ignore;
            previousKey = 2;
            currentKeylength++;
            currentInput <<= 2;
            currentInput |= BUTTON2_CODE;
            inactivityTime = 0;
        }
        else if (!b3) {
            printf("b3\r\n");
            if (previousKey == 3) goto ignore;
            previousKey = 3;
            currentKeylength++;
            currentInput <<= 2;
            currentInput |= BUTTON3_CODE;
            inactivityTime = 0;
        }
        else if (!b4) {
            printf("b4\r\n");
            if (previousKey == 4) goto ignore;
            previousKey = 4;
            currentKeylength++;
            currentInput <<= 2;
            currentInput |= BUTTON4_CODE;
            inactivityTime = 0;
        }
        else {
            inactivityTime ++;
            if (inactivityTime > 40 && currentKeylength > 0) {
                printf("Clearing\r\n");
                gpio_set(LED0_PIN);
                xtimer_usleep(50000);
                gpio_clear(LED0_PIN);
                inactivityTime = 0;
                previousKey = 0;
                currentKeylength = 0; // Обнуляем счетчик нажатий
                currentInput = 0; // Очищаем буфер
            }

        }
        //==============================
        if (currentKeylength >= 4) {

            if (currentInput == VALID_KEY) { // Сравниваем набранную в буфер последовательность нажатий с ключом
                gpio_toggle(LED0_PIN);
                puts("CORRECT KEY!");
            }
            else {
                gpio_clear(LED0_PIN);
                puts("Wrong key :^(\r\n");
            }
            previousKey = 0;
            currentKeylength = 0; // Обнуляем счетчик нажатий
            currentInput = 0; // Очищаем буфер
        }
        ignore:
        xtimer_usleep(100000);

    }
}
