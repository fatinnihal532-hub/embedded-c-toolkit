/*
 * pid.h - a discrete PID controller with anti-windup and derivative on
 * measurement.
 *
 * The same controller that runs in the embedded-pid-controller project, pulled
 * out as a portable module with no hardware dependencies so it can be unit
 * tested on a laptop. Fixed sample interval: call pid_update() at the rate
 * given to pid_init(), not whenever it is convenient.
 *
 * Two departures from the textbook equation, both deliberate:
 *   the derivative acts on the measurement, so a set-point change does not
 *   produce a spike in the output (derivative kick);
 *   the integral is frozen while the output is saturated, so a long approach
 *   to the target does not build up a term that has to be unwound afterwards
 *   (integral windup).
 */
#ifndef PID_H
#define PID_H

#include <stdbool.h>

typedef struct {
    float kp, ki, kd;
    float dt;               /* sample interval in seconds */
    float out_min, out_max;

    float integral;
    float last_measurement;
    bool  seeded;
} pid_t;

void  pid_init(pid_t *c, float kp, float ki, float kd, float dt,
               float out_min, float out_max);

/* Change gains without disturbing the accumulated integral. */
void  pid_set_gains(pid_t *c, float kp, float ki, float kd);

/* Clear the integral and the derivative history. */
void  pid_reset(pid_t *c);

/* One control step. Returns the output, already clamped to the limits. */
float pid_update(pid_t *c, float setpoint, float measurement);

#endif /* PID_H */
