#include "test_harness.h"
#include "filter.h"

TEST(ema_seeds_on_the_first_sample)
{
    ema_t f;
    ema_init(&f, 0.2f);
    CHECK_NEAR(ema_update(&f, 100.0f), 100.0f, 1e-6);
}

TEST(ema_moves_alpha_of_the_way_to_each_sample)
{
    ema_t f;
    ema_init(&f, 0.25f);
    ema_update(&f, 0.0f);                       /* seed at 0 */
    CHECK_NEAR(ema_update(&f, 100.0f), 25.0f, 1e-4);
    CHECK_NEAR(ema_update(&f, 100.0f), 43.75f, 1e-4);
}

TEST(ema_converges_on_a_constant_input)
{
    ema_t f;
    ema_init(&f, 0.3f);
    ema_update(&f, 0.0f);
    for (int i = 0; i < 200; i++)
        ema_update(&f, 42.0f);
    CHECK_NEAR(ema_value(&f), 42.0f, 1e-3);
}

TEST(ema_with_alpha_one_follows_the_input_exactly)
{
    ema_t f;
    ema_init(&f, 1.0f);
    ema_update(&f, 5.0f);
    CHECK_NEAR(ema_update(&f, 99.0f), 99.0f, 1e-6);
}

TEST(ema_clamps_an_out_of_range_alpha)
{
    ema_t f;
    ema_init(&f, 5.0f);
    ema_update(&f, 0.0f);
    CHECK_NEAR(ema_update(&f, 10.0f), 10.0f, 1e-6);   /* alpha clamped to 1 */
}

TEST(median3_rejects_a_single_spike)
{
    median3_t f;
    median3_init(&f);
    median3_update(&f, 10.0f);
    median3_update(&f, 10.0f);
    CHECK_NEAR(median3_update(&f, 900.0f), 10.0f, 1e-6);   /* spike ignored */
    CHECK_NEAR(median3_update(&f, 10.0f), 10.0f, 1e-6);
}

TEST(median3_follows_a_real_step_after_two_samples)
{
    median3_t f;
    median3_init(&f);
    median3_update(&f, 0.0f);
    median3_update(&f, 0.0f);
    median3_update(&f, 50.0f);
    CHECK_NEAR(median3_update(&f, 50.0f), 50.0f, 1e-6);
}

int main(void)
{
    RUN(ema_seeds_on_the_first_sample);
    RUN(ema_moves_alpha_of_the_way_to_each_sample);
    RUN(ema_converges_on_a_constant_input);
    RUN(ema_with_alpha_one_follows_the_input_exactly);
    RUN(ema_clamps_an_out_of_range_alpha);
    RUN(median3_rejects_a_single_spike);
    RUN(median3_follows_a_real_step_after_two_samples);
    return test_report("filter");
}
