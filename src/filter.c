#include "filter.h"

void ema_init(ema_t *f, float alpha)
{
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    f->alpha  = alpha;
    f->value  = 0.0f;
    f->seeded = false;
}

float ema_update(ema_t *f, float sample)
{
    if (!f->seeded) {
        /* Seed with the first sample. Starting from zero would otherwise make
         * the output crawl up from nothing for the first few seconds. */
        f->value  = sample;
        f->seeded = true;
    } else {
        f->value += f->alpha * (sample - f->value);
    }
    return f->value;
}

float ema_value(const ema_t *f)
{
    return f->value;
}

void median3_init(median3_t *f)
{
    f->history[0] = 0.0f;
    f->history[1] = 0.0f;
    f->history[2] = 0.0f;
    f->count = 0u;
}

float median3_update(median3_t *f, float sample)
{
    f->history[0] = f->history[1];
    f->history[1] = f->history[2];
    f->history[2] = sample;

    if (f->count < 3u)
        f->count++;
    if (f->count < 3u)
        return sample;            /* not enough history yet */

    float a = f->history[0], b = f->history[1], c = f->history[2];
    if (a > b) { float t = a; a = b; b = t; }
    if (b > c) { float t = b; b = c; c = t; }
    if (a > b) { float t = a; a = b; b = t; }
    return b;
}
