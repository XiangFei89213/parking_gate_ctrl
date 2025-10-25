#ifndef METRICS_H
#define METRICS_H
#include <stdio.h>

typedef struct {
    FILE* fp;
} metrics_t;

// 開檔（CSV 標頭：plate,latency_ms）
int metrics_open(metrics_t* m, const char* path);
// 記錄一筆延遲（ns -> 轉 ms 印）
void metrics_log(metrics_t* m, const char* plate, long latency_ns);
// 關檔
void metrics_close(metrics_t* m);

#endif
