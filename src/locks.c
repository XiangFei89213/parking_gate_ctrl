#include "locks.h"

// Define the mutexes
pthread_mutex_t m_plate_db;
pthread_mutex_t m_payment;
pthread_mutex_t m_gate_io;
pthread_mutex_t m_logbuf;

// Initialize all locks
void locks_init(void) {
    pthread_mutex_init(&m_plate_db, NULL);
    pthread_mutex_init(&m_payment,  NULL);
    pthread_mutex_init(&m_gate_io,  NULL);
    pthread_mutex_init(&m_logbuf,   NULL);
}

// Destroy all locks
void locks_destroy(void) {
    pthread_mutex_destroy(&m_plate_db);
    pthread_mutex_destroy(&m_payment);
    pthread_mutex_destroy(&m_gate_io);
    pthread_mutex_destroy(&m_logbuf);
}

// Helper to get mutex by order index, (choose bewteen 1..4)
static pthread_mutex_t* order_idx(int idx) {
    switch (idx) {
        case 1: return &m_plate_db;
        case 2: return &m_payment;
        case 3: return &m_gate_io;
        case 4: return &m_logbuf;
        default: return NULL;
    }
}

// Lock n mutexes in global order (1..n)
void lock_in_global_order(int n) {
    for (int i = 1; i <= n; ++i) {
        pthread_mutex_lock(order_idx(i));
    }
}

// Unlock n mutexes in reverse global order (n..1)
void unlock_in_global_order(int n) {
    for (int i = n; i >= 1; --i) {
        pthread_mutex_unlock(order_idx(i));
    }
}
