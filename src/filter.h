/*
 * filter.h - two small filters for noisy sensor readings.
 *
 * Exponential moving average
 *   y[n] = y[n-1] + alpha * (x[n] - y[n-1])
 *   One multiply, one subtract, one state variable. No history buffer, which
 *   is why it is the usual choice on a microcontroller. alpha between 0 and 1
 *   sets how fast it follows: small alpha is smooth and slow, large alpha is
 *   responsive and noisy.
 *
 * Median of three
 *   Returns the middle of the last three samples. An average is pulled off
 *   course by a single wild reading; a median ignores it completely. This is
 *   what to use when the noise is occasional spikes rather than constant hiss,
 *   for example an ADC sharing a board with a switching regulator.
 */
#ifndef FILTER_H
#define FILTER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float alpha;
    float value;
    bool  seeded;
} ema_t;

void  ema_init(ema_t *f, float alpha);
float ema_update(ema_t *f, float sample);
float ema_value(const ema_t *f);

typedef struct {
    float history[3];
    uint8_t count;
} median3_t;

void  median3_init(median3_t *f);
float median3_update(median3_t *f, float sample);

#endif /* FILTER_H */
