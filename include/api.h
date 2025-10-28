#ifndef API_H
#define API_H

#include "queue.h"

// Thread entry points
void* ocr_thread(void* arg);
void* billing_thread(void* arg);
void* gate_thread(void* arg);
void* alarm_thread(void* arg);

// Shared queues (declared in threads.c)
extern plate_queue_t g_ocr_to_bill;

// Small helpers
long now_ns(void);

extern int g_faults;
extern int g_bad_lock_order;

#endif
