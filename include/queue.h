#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stddef.h>

typedef struct {
    char plate[16];
    long ts_ns; // event timestamp (nanoseconds), can be 0 for now
} plate_event_t;

typedef struct {
    plate_event_t *buf;
    size_t cap;      // capacity
    size_t head;     // next pop
    size_t tail;     // next push
    size_t count;    // items in queue
    pthread_mutex_t mtx;
    pthread_cond_t  not_empty;
    pthread_cond_t  not_full;
} plate_queue_t;

// Initialize a bounded queue (capacity >= 1). Returns 0 on success.
int  pq_init(plate_queue_t *q, size_t capacity);
// Destroy queue (frees resources).
void pq_destroy(plate_queue_t *q);
// Blocking enqueue; returns 0 on success.
int  pq_push(plate_queue_t *q, const plate_event_t *ev);
// Blocking dequeue; returns 0 on success.
int  pq_pop(plate_queue_t *q, plate_event_t *out);

#endif
