#include "test_harness.h"
#include "pid.h"

/* First-order plant, the same model used in the PID controller project.
 *   tau * dT/dt = -(T - ambient) + K * u   */
static float plant_step(float temperature, float u)
{
    const float dt = 0.1f, tau = 25.0f, k = 60.0f, ambient = 25.0f;
    float drive = k * (u / 100.0f);
    return temperature + (-(temperature - ambient) + drive) / tau * dt;
}

TEST(proportional_only_output_is_gain_times_error)
{
    pid_t c;
    pid_init(&c, 2.0f, 0.0f, 0.0f, 0.1f, -1000.0f, 1000.0f);
    CHECK_NEAR(pid_update(&c, 10.0f, 4.0f), 12.0f, 1e-4);
}

TEST(output_is_clamped_to_the_limits)
{
    pid_t c;
    pid_init(&c, 100.0f, 0.0f, 0.0f, 0.1f, 0.0f, 100.0f);
    CHECK_NEAR(pid_update(&c, 50.0f, 0.0f), 100.0f, 1e-4);
    CHECK_NEAR(pid_update(&c, 0.0f, 50.0f), 0.0f, 1e-4);
}

TEST(integral_removes_steady_state_error)
{
    pid_t c;
    pid_init(&c, 1.0f, 0.4f, 0.0f, 0.1f, 0.0f, 100.0f);
    float t = 25.0f;
    for (int i = 0; i < 4000; i++)
        t = plant_step(t, pid_update(&c, 60.0f, t));
    CHECK_NEAR(t, 60.0f, 0.2f);
}

TEST(a_setpoint_change_does_not_kick_the_derivative)
{
    /* Derivative on measurement: with the measurement held still, moving the
     * set point must not produce any derivative contribution at all. */
    pid_t c;
    pid_init(&c, 0.0f, 0.0f, 10.0f, 0.1f, -1000.0f, 1000.0f);
    CHECK_NEAR(pid_update(&c, 0.0f, 30.0f), 0.0f, 1e-4);
    CHECK_NEAR(pid_update(&c, 100.0f, 30.0f), 0.0f, 1e-4);   /* no kick */
}

TEST(derivative_opposes_a_rising_measurement)
{
    pid_t c;
    pid_init(&c, 0.0f, 0.0f, 1.0f, 0.1f, -1000.0f, 1000.0f);
    pid_update(&c, 0.0f, 0.0f);
    /* measurement rises by 1 in 0.1 s, so d = -kd * 1 / 0.1 = -10 */
    CHECK_NEAR(pid_update(&c, 0.0f, 1.0f), -10.0f, 1e-4);
}

TEST(anti_windup_keeps_the_integral_bounded_while_saturated)
{
    pid_t c;
    pid_init(&c, 1.0f, 5.0f, 0.0f, 0.1f, 0.0f, 100.0f);
    /* hold a large error for a long time with the output pinned at the limit */
    for (int i = 0; i < 1000; i++)
        pid_update(&c, 1000.0f, 0.0f);
    CHECK(c.integral <= 100.0f + 1e-3f);
}

TEST(reset_clears_the_accumulated_integral)
{
    pid_t c;
    pid_init(&c, 0.0f, 1.0f, 0.0f, 0.1f, -100.0f, 100.0f);
    for (int i = 0; i < 50; i++)
        pid_update(&c, 10.0f, 0.0f);
    CHECK(c.integral > 1.0f);

    pid_reset(&c);
    CHECK_NEAR(c.integral, 0.0f, 1e-6);
}

TEST(set_gains_keeps_the_integral)
{
    pid_t c;
    pid_init(&c, 1.0f, 1.0f, 0.0f, 0.1f, -100.0f, 100.0f);
    for (int i = 0; i < 20; i++)
        pid_update(&c, 5.0f, 0.0f);
    float before = c.integral;
    pid_set_gains(&c, 3.0f, 1.0f, 0.5f);
    CHECK_NEAR(c.integral, before, 1e-6);
}

int main(void)
{
    RUN(proportional_only_output_is_gain_times_error);
    RUN(output_is_clamped_to_the_limits);
    RUN(integral_removes_steady_state_error);
    RUN(a_setpoint_change_does_not_kick_the_derivative);
    RUN(derivative_opposes_a_rising_measurement);
    RUN(anti_windup_keeps_the_integral_bounded_while_saturated);
    RUN(reset_clears_the_accumulated_integral);
    RUN(set_gains_keeps_the_integral);
    return test_report("pid");
}
