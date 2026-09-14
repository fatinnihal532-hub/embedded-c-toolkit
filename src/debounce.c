#include "debounce.h"

void debounce_init(debounce_t *d, uint8_t threshold)
{
    d->counter       = 0u;
    d->threshold     = (threshold == 0u) ? 1u : threshold;
    d->state         = false;
    d->pressed_edge  = false;
    d->released_edge = false;
}

void debounce_update(debounce_t *d, bool raw)
{
    if (raw) {
        if (d->counter < d->threshold)
            d->counter++;
    } else {
        if (d->counter > 0u)
            d->counter--;
    }

    if (!d->state && d->counter >= d->threshold) {
        d->state        = true;
        d->pressed_edge = true;
    } else if (d->state && d->counter == 0u) {
        d->state         = false;
        d->released_edge = true;
    }
}

bool debounce_state(const debounce_t *d)
{
    return d->state;
}

bool debounce_take_press(debounce_t *d)
{
    bool e = d->pressed_edge;
    d->pressed_edge = false;
    return e;
}

bool debounce_take_release(debounce_t *d)
{
    bool e = d->released_edge;
    d->released_edge = false;
    return e;
}
