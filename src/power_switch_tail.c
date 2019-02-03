#include "ch.h"
#include "hal.h"
#include "boarddef.h"
#include "power_switch_tail.h"

static volatile systime_t g_on_time = 0;
static const systime_t g_period = TIME_S2I(POWER_PERIOD_S);
static bool g_enabled = FALSE;

/*
 * Enable power switch tail II relay
 */
static void powerSwitchTailEnable(void)
{
  palSetPad(POWER_SWITCH_TAIL_PORT, POWER_SWITCH_TAIL_PIN);
  g_enabled = TRUE;
}

/*
 * Disable power switch tail II relay
 */
static void powerSwitchTailDisable(void)
{
  palClearPad(POWER_SWITCH_TAIL_PORT, POWER_SWITCH_TAIL_PIN);
  g_enabled = FALSE;
}

/*
 * Initialize power switch tail II GPIO and disable by default
 */
void powerSwitchTailInit(void)
{
  palSetPadMode(POWER_SWITCH_TAIL_PORT, POWER_SWITCH_TAIL_PIN, PAL_MODE_OUTPUT_PUSHPULL);
  palClearPad(POWER_SWITCH_TAIL_PORT, POWER_SWITCH_TAIL_PIN);
  g_enabled = FALSE;
}

/*
 * Set the duty cycle of the power relay (from 0-100%)
 */
void powerSwitchTailSetOnTime(float duty_cycle)
{
  g_on_time = duty_cycle * g_period / 100;
}

/*
 * Get the status of the power relay
 */
bool powerSwitchTailGetEnabled(void)
{
  return g_enabled;
}

/*
 * Update output of relay
 */
void powerSwitchTailThread(void *arg)
{
  systime_t now = chVTGetSystemTimeX();
  systime_t window_start_time = 0;
  uint8_t* interval_ms = arg;
  systime_t interval_st = TIME_MS2I(interval_ms[0]);

  chRegSetThreadName("POWERSWITCHTAIL");

  while (true) {
    now = chVTGetSystemTimeX();

    // Set the output based on duty cycle (g_on_time)
    while (now - window_start_time > g_period) {
      window_start_time += g_period;
    }
    if (g_on_time > now - window_start_time) {
      powerSwitchTailEnable();
    }
    else {
      powerSwitchTailDisable();
    }

    chThdSleepUntil(now + interval_st);
  }
}
