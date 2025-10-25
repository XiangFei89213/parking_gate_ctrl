#include "metrics.h"
#include <stdio.h>

int metrics_open(metrics_t* m, const char* path){
    m->fp = fopen(path, "w");
    if(!m->fp) return -1;
    fprintf(m->fp, "plate,latency_ms\n");
    fflush(m->fp);
    return 0;
}

void metrics_log(metrics_t* m, const char* plate, long latency_ns){
    if(!m->fp) return;
    double ms = (double)latency_ns / 1e6;
    fprintf(m->fp, "%s,%.3f\n", plate, ms);
    // 視需要可不每次 fflush（更快）；為簡單起見先保險
    fflush(m->fp);
}

void metrics_close(metrics_t* m){
    if(m->fp){ fclose(m->fp); m->fp = NULL; }
}
