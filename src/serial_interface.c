#include "ch.h"
#include "hal.h"
#include "boarddef.h"
#include "chprintf.h"
#include "serial_interface.h"

static int8_t temperature = 30;


/*
 * Initialize the serial driver
 */
void serialInterfaceInit(void)
{
  /* Activates the serial driver 2 using the driver default configuration. */
  sdStart(&SD2, NULL);
}

/*
 * Get the temperature value set over serial
 */
int8_t serialInterfaceGetTemperature(void)
{
  return temperature;
}

/*
 * Thread for reading serial commands
 */
void serialInterfaceThread(void* arg)
{
  (void)arg;
  size_t length = 0;

  chRegSetThreadName("SERIAL");

  while (true) {
    uint8_t buffer[BUF_SIZE];
    length = sdRead(&SD2, buffer, BUF_SIZE);

    switch ((char) buffer[0]) {
      // Set target temperature
      case 'T':
      case 't':
        temperature = 0;
        for (size_t i=1; i<length; i++) {
          temperature = (temperature * 10) + (buffer[i] - '0');
        }
        chprintf((BaseSequentialStream *)&SD2, "New temperature entered: %d\n", temperature);
        break;

      default:
        break;
    }

    chThdSleepMilliseconds(10);
  }
}
