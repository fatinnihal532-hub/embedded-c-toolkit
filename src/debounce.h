/*
 * debounce.h - turn a bouncing mechanical contact into clean press events.
 *
 * A switch does not close once. It closes, bounces open, closes again, and
 * settles after a few milliseconds. Sampled naively, one press looks like five.
 *
 * The integrator method used here counts up while the input reads active and
 * down while it reads inactive, and only changes its output at the two ends of
 * the count. Short bursts of noise never travel far enough to flip it, and the
 * memory cost is one counter per button. It is more robust than "wait 50 ms and
 * look again", and unlike a delay it never blocks the caller.
 *
 * Call debounce_update() at a steady rate, for example from a timer interrupt.
 */
#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t counter;        /* integrator, 0 .. threshold        */
    uint8_t threshold;      /* samples needed to accept a change */
    bool    state;          /* debounced level: true = pressed   */
    bool    pressed_edge;   /* set for one update on press       */
    bool    released_edge;  /* set for one update on release     */
} debounce_t;

/* threshold is how many consecutive samples confirm a change. At a 1 ms
 * sample rate, a threshold of 20 rejects any bounce shorter than 20 ms. */
void debounce_init(debounce_t *d, uint8_t threshold);

/* Feed one raw sample. true means the input reads active right now. */
void debounce_update(debounce_t *d, bool raw);

/* Current debounced level. */
bool debounce_state(const debounce_t *d);

/* Edge flags. Each returns true once per event and clears itself. */
bool debounce_take_press(debounce_t *d);
bool debounce_take_release(debounce_t *d);

#endif /* DEBOUNCE_H */
