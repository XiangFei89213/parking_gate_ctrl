// thread-safe plate event queue implementation 
// pq_push = producer push event, wait when empty full 
// pq_pop = consumer pop event, wait when queue empty 
// pq_init / pq_destroy 
// using mutex and condition variables
// ??? what is tail, head, count, cap, env, ???

#include "queue.h"
#include <stdlib.h>
#include <string.h>

// Initialize the plate queue, 
int pq_init(plate_queue_t *q, size_t capacity) {
    // invalid input(capacity == 0), return -1
    if (!q || capacity == 0) return -1;

    // allocate buffer
    q->buf = (plate_event_t*)calloc(capacity, sizeof(plate_event_t));
    
    // check allocation success
    if (!q->buf) return -1;

    // initialize fields
    q->cap = capacity;
    q->head = q->tail = q->count = 0;

    // initialize 1 mutex and 2 condition variables
    pthread_mutex_init(&q->mtx, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);

    return 0;
}

// Destroy the plate queue,
// free resources: mutex, cond vars, buffer, (memset)
void pq_destroy(plate_queue_t *q) {
    if (!q) return;
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
    pthread_mutex_destroy(&q->mtx);
    free(q->buf);
    memset(q, 0, sizeof(*q));
}

// Push a plate event into the queue (blocking if full)
int pq_push(plate_queue_t *q, const plate_event_t *ev) {
    // lock mutex, 
    pthread_mutex_lock(&q->mtx);
    // wait until not full
    while (q->count == q->cap) {
        pthread_cond_wait(&q->not_full, &q->mtx);
    }
    // push event, update tail and count
    q->buf[q->tail] = *ev;
    q->tail = (q->tail + 1) % q->cap;
    q->count++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mtx);
    return 0;
}

// Pop a plate event from the queue (blocking if empty)
int pq_pop(plate_queue_t *q, plate_event_t *out) {
    pthread_mutex_lock(&q->mtx);
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->mtx);
    }
    *out = q->buf[q->head];
    q->head = (q->head + 1) % q->cap;
    q->count--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mtx);
    return 0;
}
