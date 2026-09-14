#include "pid.h"

static float clampf(float v, float lo, float hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void pid_init(pid_t *c, float kp, float ki, float kd, float dt,
              float out_min, float out_max)
{
    c->kp = kp;
    c->ki = ki;
    c->kd = kd;
    c->dt = (dt > 0.0f) ? dt : 1.0f;
    c->out_min = out_min;
    c->out_max = out_max;
    pid_reset(c);
}

void pid_set_gains(pid_t *c, float kp, float ki, float kd)
{
    c->kp = kp;
    c->ki = ki;
    c->kd = kd;
}

void pid_reset(pid_t *c)
{
    c->integral         = 0.0f;
    c->last_measurement = 0.0f;
    c->seeded           = false;
}

float pid_update(pid_t *c, float setpoint, float measurement)
{
    float error = setpoint - measurement;
    float p = c->kp * error;

    if (!c->seeded) {
        c->last_measurement = measurement;
        c->seeded = true;
    }
    float d = -c->kd * (measurement - c->last_measurement) / c->dt;
    c->last_measurement = measurement;

    float unsaturated = p + c->integral + d;

    bool winding_up = (unsaturated >= c->out_max && error > 0.0f) ||
                      (unsaturated <= c->out_min && error < 0.0f);
    if (!winding_up) {
        c->integral += c->ki * error * c->dt;
        c->integral = clampf(c->integral, c->out_min, c->out_max);
    }

    return clampf(p + c->integral + d, c->out_min, c->out_max);
}
