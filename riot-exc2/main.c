#include <stdio.h>
#include "board.h"
#include "periph/gpio.h"
#include "irq.h"
#include "xtimer.h"
#include "thread.h"

// #define BTN0_PIN GPIO_PIN(PORT_B, 1)

#define VERSION 2      // 1 | 2 | 3

volatile bool led_state = false;
volatile int debounced_btn_state = 0;
volatile uint32_t last_pressed_time;

volatile uint32_t timerSwitchPeriod = 100;
volatile bool flagSwitch = false;
volatile bool blinkingEnable = false;

uint32_t DEBOUNCE_USEC = 3000000; // 3 sec
uint32_t HOLD = 5000; // 5 sec

void polling(void) {
    gpio_init(BTN0_PIN, GPIO_IN);
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_clear(LED0_PIN);
    while (1) {
        int btn_state = gpio_read(BTN0_PIN);
        if (btn_state == 1 && debounced_btn_state == 0 && xtimer_now_usec() - last_pressed_time > DEBOUNCE_USEC) {
            debounced_btn_state = 1;
            led_state = !led_state;
            if (led_state) {
                gpio_set(LED0_PIN);
            }
            else {
                gpio_clear(LED0_PIN);
            }
            last_pressed_time = xtimer_now_usec();
        }
        if (btn_state == 0 && debounced_btn_state == 1) {
            debounced_btn_state = 0;
        }
    }
}

uint32_t last_debounce_time;

volatile uint8_t TIME_LEFT = 30;

volatile int new_state;
volatile bool correct_state = false;

void timer_callback(void* arg) {
    int new_debounced_state = gpio_read(BTN0_PIN);
    if (new_state != new_debounced_state) {
        correct_state = true;
        blinkingEnable = !blinkingEnable;
        gpio_irq_enable(BTN0_PIN);
    }
    (void)(arg);
}

void timerSwitch_callback(void* arg) {
    int new_debounced_state = gpio_read(BTN0_PIN);
    if (new_state == new_debounced_state) {
        correct_state = true;
        flagSwitch = !flagSwitch;
        if (flagSwitch) timerSwitchPeriod = 100;
        else timerSwitchPeriod = 200;
        gpio_irq_enable(BTN0_PIN);
    }.
    (void)(arg);
}

xtimer_t timerSwitchMode = { .callback = timerSwitch_callback };
xtimer_t timer = { .callback = timer_callback };
xtimer_t timerBounced = { .callback = timer_callback };

void debounceCallback(void* arg) {
    new_state = gpio_read(BTN0_PIN);
    gpio_irq_disable(BTN0_PIN);
    xtimer_set(&timer, 10000);
    xtimer_set(&timerSwitchMode, 50000);
    (void)arg;
}

void noBounce(void) {
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_clear(LED0_PIN);
    gpio_init_int(BTN0_PIN,
        GPIO_IN,
        GPIO_FALLING,
        debounceCallback, NULL);
    while (1) {
        if (correct_state) {
            printf("current:%d\n\r", new_state);
            correct_state = false;
        }
        if (blinkingEnable) {
            gpio_toggle(LED0_PIN);
            xtimer_msleep(timerSwitchPeriod);
        }
        else gpio_clear(LED0_PIN);
    }
    gpio_irq_enable(BTN0_PIN);
}


#define _3sec  3000
#define _1sec  1000
volatile bool fast_mode = true;

void blinkLED(void) {
    if (led_state) {
        gpio_set(LED0_PIN);
        xtimer_msleep(fast_mode ? _1sec : _3sec);
        gpio_clear(LED0_PIN);
        xtimer_msleep(fast_mode ? _1sec : _3sec);
    }
    else {
        gpio_clear(LED0_PIN);
    }
}

volatile uint32_t last_falling_time;
volatile uint32_t last_rising_time;

void risingCallback(void* arg) {
    int pressed = gpio_read(BTN0_PIN);
    if (pressed) {
        last_falling_time = xtimer_now_usec();
    }
    else {
        last_rising_time = xtimer_now_usec();

    }
    (void)(arg);
}



void switchDuration(void) {
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(BTN0_PIN, GPIO_IN);
    gpio_clear(LED0_PIN);
    gpio_init_int(BTN0_PIN,
        GPIO_IN,
        GPIO_BOTH,
        risingCallback, NULL);

    gpio_irq_enable(BTN0_PIN);
    bool wasPressedLong = false;
    last_falling_time = xtimer_now_usec();
    last_rising_time = xtimer_now_usec();
    uint32_t current_time;
    while (1) {
        current_time = xtimer_now_usec();
        //if btn is being pressed for more then 3 sec
        if (last_falling_time > last_rising_time && current_time - last_falling_time > 3000000 && !wasPressedLong) {
            fast_mode = !fast_mode;
            wasPressedLong = true;
        }
        //btn has been released 
        if (last_rising_time >= last_falling_time) {
            wasPressedLong = false;
            //btn has been released *quickly* 
            if (last_rising_time - last_falling_time < 3000000) {
                led_state = !led_state;
            }
        }
        blinkLED();
    }
}

int main(void) {
    printf("my_version: %d\r\n", VERSION);
    switch (VERSION) {
    case 1:
        polling();
        break;
    case 2:
        noBounce();
        break;
    case 3:
        switchDuration();
        break;
    default:
        break;
    }
    return 0;
}
