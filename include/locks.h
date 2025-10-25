#ifndef LOCKS_H
#define LOCKS_H

#include <pthread.h>

// Shared resources protected by mutexes (simulated for now)
extern pthread_mutex_t m_plate_db;   // 車牌資料庫
extern pthread_mutex_t m_payment;    // 付款狀態
extern pthread_mutex_t m_gate_io;    // 閘門I/O
extern pthread_mutex_t m_logbuf;     // 紀錄緩衝區

// Initialize all mutexes.
void locks_init(void);
// Destroy all mutexes.
void locks_destroy(void);

// Enforce global lock order: plate_db -> payment -> gate_io -> logbuf
// Lock the first N in that order (1..4). Unlock in reverse.
void lock_in_global_order(int n);
void unlock_in_global_order(int n);

#endif
