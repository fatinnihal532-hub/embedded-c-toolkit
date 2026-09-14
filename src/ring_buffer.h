/*
 * ring_buffer.h - a fixed-size byte FIFO for one producer and one consumer.
 *
 * This is the structure that sits behind every interrupt-driven UART driver.
 * The interrupt handler writes bytes in, the main program reads them out, and
 * neither one has to disable interrupts to do it, provided there is exactly one
 * writer and exactly one reader.
 *
 * The trick is that the writer only ever changes `head` and the reader only
 * ever changes `tail`. Each index is written by a single context, so a partly
 * updated index can never be observed. The capacity is one byte less than the
 * buffer length, which is what lets "head == tail" mean empty without a
 * separate count that both sides would have to update.
 *
 * No dynamic memory. The caller supplies the storage, which is how embedded
 * code avoids a heap it cannot afford to fragment.
 */
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t         *buffer;
    size_t           size;          /* length of buffer in bytes  */
    volatile size_t  head;          /* written by the producer    */
    volatile size_t  tail;          /* written by the consumer    */
} ring_buffer_t;

/* Bind a ring buffer to caller-supplied storage. size must be at least 2. */
void   rb_init(ring_buffer_t *rb, uint8_t *storage, size_t size);

/* Discard everything. Only safe when neither side is running. */
void   rb_reset(ring_buffer_t *rb);

bool   rb_is_empty(const ring_buffer_t *rb);
bool   rb_is_full(const ring_buffer_t *rb);

/* Number of bytes stored, and number that can still be written. */
size_t rb_count(const ring_buffer_t *rb);
size_t rb_free(const ring_buffer_t *rb);

/* Push one byte. Returns false if the buffer is full; the byte is not stored. */
bool   rb_push(ring_buffer_t *rb, uint8_t value);

/* Pop one byte into *out. Returns false if the buffer is empty. */
bool   rb_pop(ring_buffer_t *rb, uint8_t *out);

/* Read the next byte without removing it. */
bool   rb_peek(const ring_buffer_t *rb, uint8_t *out);

/* Bulk helpers. Both return the number of bytes actually moved. */
size_t rb_write(ring_buffer_t *rb, const uint8_t *data, size_t length);
size_t rb_read(ring_buffer_t *rb, uint8_t *data, size_t length);

#endif /* RING_BUFFER_H */
