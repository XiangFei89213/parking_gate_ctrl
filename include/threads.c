#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "api.h"
#include "locks.h"
#include "queue.h"

// define in main.c
extern metrics_t g_metrics; 

// Global queue: OCR(producer) -> Billing(consumer)
plate_queue_t g_ocr_to_bill;

// --- Simulated hardware(gate) action, ---
static void fake_gate_actuate(const char* plate) {
    // Lock only gate_io (3) and logbuf (4) 
    // to demonstrate partial order lock
    lock_in_global_order(3); 
    // locks 1..3, but we only need gate_io; keep it simple now
    // ...operate gate...
    // print the action: the plate being actuated for
    printf("[Gate] Actuating for plate=%s\n", plate);
    unlock_in_global_order(3);
}

// --- Threads ---

// input peremeter: none, output: none
// producer thread: simulates OCR reading plates and pushing to queue
void* ocr_thread(void* arg) {
    (void)arg;
    int counter = 1;
    while (1) {
        plate_event_t ev = {0};
        snprintf(ev.plate, sizeof(ev.plate), "ABC%04d", counter++);
        
        // time stamp for metrics 
        ev.ts_ns = now_ns(); 

        // (Optionally set ev.ts_ns = now_ns();)
        pq_push(&g_ocr_to_bill, &ev);
        printf("[OCR] Detected %s -> queued\n", ev.plate);

        // simulate 1 car would arrive every 200 ms
        usleep(200 * 1000); // 200 ms
    }
    return NULL;
}

// 在 fake_gate_actuate 後記錄延遲：Billing 決定開門 → gate 動作後立即記錄
static void record_latency(const plate_event_t* ev){
    long end_ns = now_ns();
    long dt = end_ns - ev->ts_ns;
    metrics_log(&g_metrics, ev->plate, dt);
}

// consumer thread: billing verification, then tell gate to actuate
void* billing_thread(void* arg) {
    (void)arg;
    while (1) {
        plate_event_t ev;
        pq_pop(&g_ocr_to_bill, &ev);

        // Lock in global order: plate_db -> payment -> gate_io -> logbuf (1..4)
        lock_in_global_order(2); // Here we actually need 1..2 for DB and payment
        // Simulate lookups / payment verification (short critical section)
        // ... pretend: check plate in DB, check/charge account ...
        usleep(10 * 1000); // 10 ms inside the lock to mimic work
        unlock_in_global_order(2);

        printf("[Billing] OK for %s\n", ev.plate);

        // Tell gate to actuate (locks gate_io etc.)
        fake_gate_actuate(ev.plate);

        record_latency(&ev);
    }
    return NULL;
}

// gate control thread: monitors gate status (simulated here)
void* gate_thread(void* arg) {
    (void)arg;
    while (1) {
        printf("[Gate] Heartbeat\n");
        usleep(1000 * 1000);
    }
    return NULL;
}

// alarm monitoring thread: monitors alarms (simulated here)
void* alarm_thread(void* arg) {
    (void)arg;
    while (1) {
        printf("[Alarm] Monitoring\n");
        usleep(1200 * 1000);
    }
    return NULL;
}

// --- helpers ---

long now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}
