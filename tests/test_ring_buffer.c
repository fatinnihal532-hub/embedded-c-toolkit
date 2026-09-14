#include "test_harness.h"
#include "ring_buffer.h"

static uint8_t storage[8];
static ring_buffer_t rb;

static void setup(void)
{
    rb_init(&rb, storage, sizeof(storage));
}

TEST(starts_empty)
{
    setup();
    CHECK(rb_is_empty(&rb));
    CHECK(!rb_is_full(&rb));
    CHECK_EQ_INT(rb_count(&rb), 0);
    CHECK_EQ_INT(rb_free(&rb), 7);      /* one slot is spent on the empty test */
}

TEST(push_then_pop_returns_same_byte)
{
    setup();
    uint8_t out = 0;
    CHECK(rb_push(&rb, 0xA5));
    CHECK_EQ_INT(rb_count(&rb), 1);
    CHECK(rb_pop(&rb, &out));
    CHECK_EQ_INT(out, 0xA5);
    CHECK(rb_is_empty(&rb));
}

TEST(pop_from_empty_fails_and_leaves_output_alone)
{
    setup();
    uint8_t out = 0x11;
    CHECK(!rb_pop(&rb, &out));
    CHECK_EQ_INT(out, 0x11);
}

TEST(fills_to_capacity_then_refuses)
{
    setup();
    for (int i = 0; i < 7; i++)
        CHECK(rb_push(&rb, (uint8_t)i));

    CHECK(rb_is_full(&rb));
    CHECK_EQ_INT(rb_count(&rb), 7);
    CHECK(!rb_push(&rb, 0xFF));        /* must not overwrite unread data */
    CHECK_EQ_INT(rb_count(&rb), 7);
}

TEST(order_is_first_in_first_out)
{
    setup();
    for (int i = 0; i < 5; i++)
        rb_push(&rb, (uint8_t)(i + 10));

    for (int i = 0; i < 5; i++) {
        uint8_t out = 0;
        CHECK(rb_pop(&rb, &out));
        CHECK_EQ_INT(out, i + 10);
    }
}

TEST(indices_wrap_around_many_times)
{
    setup();
    /* 200 pushes and pops through an 8-byte buffer forces the wrap repeatedly.
     * An off-by-one in next_index() shows up here and nowhere else. */
    for (int i = 0; i < 200; i++) {
        uint8_t out = 0;
        CHECK(rb_push(&rb, (uint8_t)i));
        CHECK(rb_pop(&rb, &out));
        CHECK_EQ_INT(out, (uint8_t)i);
    }
    CHECK(rb_is_empty(&rb));
}

TEST(peek_does_not_remove)
{
    setup();
    uint8_t out = 0;
    rb_push(&rb, 0x7E);
    CHECK(rb_peek(&rb, &out));
    CHECK_EQ_INT(out, 0x7E);
    CHECK_EQ_INT(rb_count(&rb), 1);
}

TEST(bulk_write_stops_at_capacity)
{
    setup();
    const uint8_t data[10] = {1,2,3,4,5,6,7,8,9,10};
    CHECK_EQ_INT(rb_write(&rb, data, 10), 7);
    CHECK(rb_is_full(&rb));

    uint8_t out[10] = {0};
    CHECK_EQ_INT(rb_read(&rb, out, 10), 7);
    CHECK_EQ_INT(out[0], 1);
    CHECK_EQ_INT(out[6], 7);
}

TEST(reset_empties_the_buffer)
{
    setup();
    rb_push(&rb, 1);
    rb_push(&rb, 2);
    rb_reset(&rb);
    CHECK(rb_is_empty(&rb));
    CHECK_EQ_INT(rb_count(&rb), 0);
}

int main(void)
{
    RUN(starts_empty);
    RUN(push_then_pop_returns_same_byte);
    RUN(pop_from_empty_fails_and_leaves_output_alone);
    RUN(fills_to_capacity_then_refuses);
    RUN(order_is_first_in_first_out);
    RUN(indices_wrap_around_many_times);
    RUN(peek_does_not_remove);
    RUN(bulk_write_stops_at_capacity);
    RUN(reset_empties_the_buffer);
    return test_report("ring_buffer");
}
