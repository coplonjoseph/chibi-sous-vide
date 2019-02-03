#include "ch.h"
#include "hal.h"
#include "boarddef.h"
#include "ds18b20.h"

/*
 * GLOBAL VARIABLES
 */

static uint8_t g_testbuf[2];
static float g_temperature;

/*
 * Config for underlying PWM driver.
 * Note! It is NOT constant because 1-wire driver needs to change them
 * during functioning.
 */
static PWMConfig pwm_cfg = {
    0,
    0,
    NULL,
    {
     {PWM_OUTPUT_DISABLED, NULL},
     {PWM_OUTPUT_DISABLED, NULL},
     {PWM_OUTPUT_DISABLED, NULL},
     {PWM_OUTPUT_DISABLED, NULL}
    },
    0,
#if STM32_PWM_USE_ADVANCED
    0,
#endif
    0
};

/*
 * Config for one wire driver
 */
static const onewireConfig ow_cfg = {
    &PWMD3,
    &pwm_cfg,
    PWM_OUTPUT_ACTIVE_LOW,
    ONEWIRE_MASTER_CHANNEL,
    ONEWIRE_SAMPLE_CHANNEL,
    ONEWIRE_PORT,
    ONEWIRE_PIN,
#if defined(STM32F1XX)
    ONEWIRE_PAD_MODE_IDLE,
#endif
    ONEWIRE_PAD_MODE_ACTIVE,
#if ONEWIRE_USE_STRONG_PULLUP
    strong_pullup_assert,
    strong_pullup_release
#endif
};

/*
 * Get temperature value from DS18B20 temperature sensor
 */
static void ds18b20UpdateTemp(void)
{
  if (true == onewireReset(&OWD1)) {
    /* Start temperature measurement */
    g_testbuf[0] = DS18B20_CMD_SKIP_ROM;
    g_testbuf[1] = DS18B20_CMD_CONVERT_T;
    onewireWrite(&OWD1, g_testbuf, 2, 0);

    /* Poll bus waiting for ready signal */
    g_testbuf[0] = 0;
    while (g_testbuf[0] == 0) {
      chThdSleepMilliseconds(500);
      onewireRead(&OWD1, g_testbuf, 1);
    }

    onewireReset(&OWD1);
    g_testbuf[0] = DS18B20_CMD_SKIP_ROM;
    g_testbuf[1] = DS18B20_CMD_READ_SCRATCHPAD;
    onewireWrite(&OWD1, g_testbuf, 2, 0);

    onewireRead(&OWD1, g_testbuf, 2);

    g_temperature = ((g_testbuf[1] * 256.0) + g_testbuf[0]) / 16.0;
  }
}

/*
 * Initialize and configure DS18B20 temperature sensor
 */
void ds18b20Init(void)
{
  onewireObjectInit(&OWD1);
  onewireStart(&OWD1, &ow_cfg);
}

/*
 * Get the temperature
 */
float ds18b20GetTemperature(void)
{
  return g_temperature;
}

/*
 * Main thread for reading temperature
 */
void ds18b20Thread (void *arg) {
  uint8_t *interval_ms = arg;
  systime_t interval_st = TIME_MS2I(interval_ms[0]);
  systime_t now = chVTGetSystemTimeX();

  chRegSetThreadName("DS18B20");


  while (true) {
    now = chVTGetSystemTimeX();
    ds18b20UpdateTemp();
    chThdSleepUntil(now + interval_st);
  }
}
