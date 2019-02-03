#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H

#define BUF_SIZE 3

/*
 * Initialize the serial driver
 */
void serialInterfaceInit(void);

/*
 * Get the temperature value set over serial
 */
int8_t serialInterfaceGetTemperature(void);

/*
 * Thread for reading serial commands
 */
void serialInterfaceThread(void* arg);


#endif
