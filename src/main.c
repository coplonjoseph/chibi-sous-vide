/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include "pid.h"
#include "ds18b20.h"
#include "chprintf.h"
#include "power_switch_tail.h"
#include "serial_interface.h"

/* PID Parameters */
#define PID_PARAM_KP 10.0   // Proportional
#define PID_PARAM_KI 0.025 // Integral
#define PID_PARAM_KD 20.0    // Derivative

/* Temperature sensor thread */
static THD_WORKING_AREA(waTemperature, 256);
thread_t *temperatureThread;
static const uint8_t temperatureIntervalMS = 50;

/* Power switch tail relay thread */
static THD_WORKING_AREA(waRelay, 256);
thread_t *relayThread;
static const uint8_t relayIntervalMS = 50;

/* Serial thread */
static THD_WORKING_AREA(waSerial, 256);
thread_t *serialThread;

/*
 * Application entry point.
 */
int main(void) {

  pidc_t pid;
  float temperature;
  float temperature_goal = 0;
  float duty;

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();
  ds18b20Init();
  powerSwitchTailInit();
  serialInterfaceInit();

  /* Initialize PID system */
  pid_create(&pid, &temperature, &duty, &temperature_goal,
             PID_PARAM_KP, PID_PARAM_KI, PID_PARAM_KD, PID_ON_M, PID_DIRECT);
  pid_setOutputLimits(&pid, 0.0, 100.0);
  pid_setSampleTime(&pid, 10);
  pid_setMode(&pid, PID_AUTOMATIC);

  /* Create the temperature sensor thread. */
  temperatureThread = chThdCreateStatic(waTemperature, sizeof(waTemperature),
                                        NORMALPRIO + 2, ds18b20Thread,
                                        &temperatureIntervalMS);
  /* Create the power relay thread. */
  relayThread = chThdCreateStatic(waRelay, sizeof(waRelay),
                                  NORMALPRIO + 3, powerSwitchTailThread,
                                  &relayIntervalMS);
  /* Create the serial thread */
  serialThread = chThdCreateStatic(waSerial, sizeof(waSerial),
                                   NORMALPRIO + 1, serialInterfaceThread, NULL);

  /*
   * Normal main() thread activity. Reads sensor values, updates settings from
   * interface commands, and updates actuator outputs.
   */

  while (true) {
    temperature = ds18b20GetTemperature();
    temperature_goal = (float) serialInterfaceGetTemperature();
    chprintf((BaseSequentialStream *)&SD2, "Temperature: %f\n", temperature);

    pid_compute(&pid);

    chprintf((BaseSequentialStream *)&SD2, "Duty: %f\n", duty);

    powerSwitchTailSetOnTime(duty);

    chThdSleepMilliseconds(10);
  }
}
