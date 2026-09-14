#include "test_harness.h"
#include "debounce.h"

/* Feed the same level n times. */
static void feed(debounce_t *d, bool level, int n)
{
    for (int i = 0; i < n; i++)
        debounce_update(d, level);
}

TEST(starts_released)
{
    debounce_t d;
    debounce_init(&d, 5);
    CHECK(!debounce_state(&d));
    CHECK(!debounce_take_press(&d));
}

TEST(a_clean_press_is_accepted_after_the_threshold)
{
    debounce_t d;
    debounce_init(&d, 5);

    feed(&d, true, 4);
    CHECK(!debounce_state(&d));        /* not yet */

    feed(&d, true, 1);
    CHECK(debounce_state(&d));
    CHECK(debounce_take_press(&d));
    CHECK(!debounce_take_press(&d));   /* the edge fires only once */
}

TEST(bounce_shorter_than_the_threshold_is_rejected)
{
    debounce_t d;
    debounce_init(&d, 5);

    /* two samples high, two low, three high: the integrator never reaches 5 */
    feed(&d, true, 2);
    feed(&d, false, 2);
    feed(&d, true, 3);
    CHECK(!debounce_state(&d));
    CHECK(!debounce_take_press(&d));
}

TEST(release_needs_the_counter_to_reach_zero)
{
    debounce_t d;
    debounce_init(&d, 4);

    feed(&d, true, 4);
    CHECK(debounce_state(&d));
    debounce_take_press(&d);

    feed(&d, false, 3);
    CHECK(debounce_state(&d));         /* still held */

    feed(&d, false, 1);
    CHECK(!debounce_state(&d));
    CHECK(debounce_take_release(&d));
}

TEST(a_long_hold_produces_exactly_one_press_edge)
{
    debounce_t d;
    debounce_init(&d, 3);

    feed(&d, true, 500);
    int edges = 0;
    /* the single edge was raised during the first few updates */
    if (debounce_take_press(&d)) edges++;
    feed(&d, true, 500);
    if (debounce_take_press(&d)) edges++;

    CHECK_EQ_INT(edges, 1);
    CHECK(debounce_state(&d));
}

TEST(zero_threshold_is_corrected_to_one)
{
    debounce_t d;
    debounce_init(&d, 0);
    debounce_update(&d, true);
    CHECK(debounce_state(&d));
}

int main(void)
{
    RUN(starts_released);
    RUN(a_clean_press_is_accepted_after_the_threshold);
    RUN(bounce_shorter_than_the_threshold_is_rejected);
    RUN(release_needs_the_counter_to_reach_zero);
    RUN(a_long_hold_produces_exactly_one_press_edge);
    RUN(zero_threshold_is_corrected_to_one);
    return test_report("debounce");
}
