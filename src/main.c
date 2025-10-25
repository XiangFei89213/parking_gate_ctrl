#include <stdio.h>
#include <pthread.h>
#include "api.h"
#include "locks.h"
#include "queue.h"

metrics_t g_metrics;

int main(void) {
    printf("Starting Parking Gate Controller (Linux, pthreads)...\n");

    // Init shared resources
    locks_init();
    if (pq_init(&g_ocr_to_bill, 64) != 0) {
        fprintf(stderr, "Queue init failed\n");
        return 1;
    }

    // Launch threads
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, ocr_thread, NULL);
    pthread_create(&t2, NULL, billing_thread, NULL);
    pthread_create(&t3, NULL, gate_thread, NULL);
    pthread_create(&t4, NULL, alarm_thread, NULL);

    // Wait forever (Ctrl+C to exit)
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

    // Cleanup (not reached in this demo)
    metrics_close(&g_metrics);
    pq_destroy(&g_ocr_to_bill);
    locks_destroy();
    return 0;
}
