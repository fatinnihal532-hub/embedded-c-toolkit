#include "ring_buffer.h"

static size_t next_index(const ring_buffer_t *rb, size_t index)
{
    size_t n = index + 1u;
    return (n == rb->size) ? 0u : n;
}

void rb_init(ring_buffer_t *rb, uint8_t *storage, size_t size)
{
    rb->buffer = storage;
    rb->size   = size;
    rb->head   = 0u;
    rb->tail   = 0u;
}

void rb_reset(ring_buffer_t *rb)
{
    rb->head = 0u;
    rb->tail = 0u;
}

bool rb_is_empty(const ring_buffer_t *rb)
{
    return rb->head == rb->tail;
}

bool rb_is_full(const ring_buffer_t *rb)
{
    return next_index(rb, rb->head) == rb->tail;
}

size_t rb_count(const ring_buffer_t *rb)
{
    if (rb->head >= rb->tail)
        return rb->head - rb->tail;
    return rb->size - rb->tail + rb->head;
}

size_t rb_free(const ring_buffer_t *rb)
{
    return rb->size - 1u - rb_count(rb);
}

bool rb_push(ring_buffer_t *rb, uint8_t value)
{
    size_t next = next_index(rb, rb->head);
    if (next == rb->tail)
        return false;                 /* full: never overwrite unread data */

    rb->buffer[rb->head] = value;
    rb->head = next;                  /* publish only after the data is in */
    return true;
}

bool rb_pop(ring_buffer_t *rb, uint8_t *out)
{
    if (rb_is_empty(rb))
        return false;

    *out = rb->buffer[rb->tail];
    rb->tail = next_index(rb, rb->tail);
    return true;
}

bool rb_peek(const ring_buffer_t *rb, uint8_t *out)
{
    if (rb_is_empty(rb))
        return false;

    *out = rb->buffer[rb->tail];
    return true;
}

size_t rb_write(ring_buffer_t *rb, const uint8_t *data, size_t length)
{
    size_t written = 0u;
    while (written < length && rb_push(rb, data[written]))
        written++;
    return written;
}

size_t rb_read(ring_buffer_t *rb, uint8_t *data, size_t length)
{
    size_t read = 0u;
    while (read < length && rb_pop(rb, &data[read]))
        read++;
    return read;
}
