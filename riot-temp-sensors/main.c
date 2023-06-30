#include <stdio.h>
#include "xtimer.h"
#include "periph/i2c.h"
#include "bmx280.h"
#include "dht.h"
#include "dht_params.h"

static dht_t dht11;
static const dht_params_t dht_params = {
    .in_mode = GPIO_OUT,
    .pin = GPIO_PIN(), //I DONT KNOW WHAT PIN IT IS
};

static bmx280_t bmx280;
static const bmx280_params_t bmx280_params = {
    .i2c_dev = I2C_DEV(1),
    .i2c_addr = 0x76,
    .t_sb = BMX280_SB_0_5,
    .filter = BMX280_FILTER_OFF,
    .run_mode = BMX280_MODE_FORCED,
    .temp_oversample = BMX280_OSRS_X1,
    .press_oversample = BMX280_OSRS_X1,
    .humid_oversample = BMX280_OSRS_X1,
};

int main(void)
{
    int16_t temp, humidity;
    if (dht_init(&dht11, dht_params) == 0) {
        puts("DHT11 succesfully initialised!\r");
    }

    if (bmx280_init(&bmx280, &bmx280_params) == BMX280_OK) {
        puts("BME280 succesfully initialised!\r");

        while (1) {
            if (dht_read(&dht11, &temp, &humidity) == DHT_OK) {
                printf("TEMP= %u, HUMIDITY= %u\r\n", temp, humidity);
            }
            int t = bmx280_read_temperature(&bmx280);    /* degrees C * 100 */
            int h = bme280_read_humidity(&bmx280);       /* percents * 100 */
            int p = bmx280_read_pressure(&bmx280) / 100; /* Pa -> mbar */

            printf("T = %i.%i C, H = %i.%i %%, P = %i mbar\r\n", t / 100, t % 100, h / 100, h % 100, p);

            xtimer_msleep(10000);
        }
    }

    return 0;
}
