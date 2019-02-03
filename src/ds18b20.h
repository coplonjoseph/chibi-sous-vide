#ifndef DS1820B_H
#define DS1820B_H

#define DS18B20_CMD_SKIP_ROM 0xCC
#define DS18B20_CMD_CONVERT_T 0x44
#define DS18B20_CMD_READ_SCRATCHPAD 0xBE

/*
 * Initialize and configure DS18B20 temperature sensor
 */
void ds18b20Init(void);

/*
 * Get the temperature
 */
float ds18b20GetTemperature(void);

/*
 * Main thread for reading temperature
 */
void ds18b20Thread(void *arg);

#endif
