#ifndef POWER_SWITCH_TAIL_H
#define POWER_SWITCH_TAIL_H

#define POWER_PERIOD_S 10

/*
 * Initialize power switch tail II GPIO and disable by default
 */
void powerSwitchTailInit(void);

/*
 * Set the duty cycle of the power relay (from 0-100%)
 */
void powerSwitchTailSetOnTime(float duty_cycle);

/*
 * Get the status of the power relay
 */
bool powerSwitchTailGetEnabled(void);

/*
 * Thread for updating output of power switch tail II relay
 */
void powerSwitchTailThread(void *arg);


#endif
