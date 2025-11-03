1. why is the project arrange like this? how to define what is in include and what is in src, 
2. how did chatGPT come up with these 4 resourse that have to be locked(protect)
3. ??? still not sure what api.h is doing ??? 
4. ??? wait-for Graph ??? 


now i have threads.c locks.c and queue.c, 
```threads.c``` defines all 4 threads:
  1. `ocr_thread`：模擬車牌事件(10–20 FPS)→ 事件佇列
  2. `billing_thread`：查帳/白名單（可加隨機延遲模擬雲端）
  3. `gate_thread`：馬達開關、限位偵測
  4. `alarm_thread`：異常聲光/記錄
  and a fake_gate_actuate() to simulate the gates actions, to call when other states is finished and to change the status of the gate.

```locks.c``` 管的是「多把鎖 + 固定順序」→ 控制整個工廠的工具
declare 4 mutex, each represent different mutual resourse, define locks_init/ lock_destroy/ - create and destory a mutex, 
 lock_in_global_order / unlock_in_global_order/ 

```queue.c```  用的是「單一鎖 + 條件變數」→ 控制傳送帶。 
declare queue, pq_init / pq_destroy - create and destory queue, pq_pop/ pq_push - producer push, consumer pop, 
+ mutex + :::conditional variable:::

# 專案一句話

用 C 做一個「多執行緒的自動閘門控制器」：車牌輸入、扣款/白名單檢查、柵欄馬達、告警各自跑不同 thread，用嚴格鎖序與（可切換）死鎖偵測確保**永不卡死**，並量測 p95/p99 延遲。

# 系統架構（最小可行）

* Threads（或 FreeRTOS tasks）

  1. `ocr_thread`：模擬車牌事件(10–20 FPS)→ 事件佇列
  2. `billing_thread`：查帳/白名單（可加隨機延遲模擬雲端）
  3. `gate_thread`：馬達開關、限位偵測
  4. `alarm_thread`：異常聲光/記錄
* 資源（要加鎖）：`plate_db`、`payment_state`、`gate_io`、`log_buffer`
* 同步：`pthread_mutex_t`（Linux）或 FreeRTOS mutex（含 priority inheritance）
* 排程重點：給 `gate_thread` 高優先權，`billing_thread` 次之，`ocr_thread`/`alarm_thread` 較低。
* 日誌：預先配置（preallocated）環形緩衝區，ISR/高頻寫不動態配記憶體。
* 看門狗：每 100ms 心跳，超時自復原（重啟子模組、記錄 dump）。

# 死鎖處理（兩段式）

1. **避免**：統一鎖序（Lock Ordering），例如 `plate_db → payment_state → gate_io → log_buffer`，任何地方都嚴守這順序。
2. **偵測**（可切換）：維護 wait-for graph（誰在等誰），每 50ms 跑一次循環偵測；找到環就取消其中一個鎖請求並記錄事件（victim abort），系統不會卡死。

# 效能與驗證

* 測試情境：1–20 車/秒突發、扣款 API 注入隨機延遲/超時、馬達 I/O 抖動、故意打亂鎖順序（驗證偵測器）。
* 指標：

  * **p50/p95/p99** 閘門反應時間（車牌→馬達啟動）
  * 死鎖次數（理想為 0；開啟偵測時應能自動解除）
  * 漏讀率（missed plates = 0）
  * CPU 使用率與 context switches/sec
* 期望成績（可寫進 README/履歷）：`10^6` 次事件 0 死鎖；在 20 req/s 突發下 p99 ≤ 100 ms。

# 目錄與可交付物

```
/src
  main.c, threads.c, locks.c, watchdog.c, wfg.c   // wait-for graph
/include
  api.h, metrics.h
/tools
  loadgen.c, fault_injector.c
/docs
  design.md, results.md (含圖表), lock_order.md
/scripts
  run.sh, bench.sh, plot.py
```

* 60 秒 demo 錄影：突發流量 + 統計輸出 +「死鎖偵測開關」展示。

# 履歷可用敘述（直接貼）

* Built a real-time parking gate controller in C with four priority-controlled threads; enforced global lock ordering and runtime wait-for-graph detection to eliminate deadlocks. Achieved 0 deadlocks over 1e6 events; p99 actuation latency 85 ms at 20 req/s burst.
* Implemented a preallocated lock-free logging buffer and watchdog-based self-recovery; mean recovery <150 ms from injected stalls.

# 兩週實作路線

1. 立刻：拉 repo、建 `Makefile`、做四個 thread 的雛形與事件佇列。
2. 加上鎖與**固定鎖序**，寫 `results.md` 指標欄位（先空著）。
3. 實作 wait-for graph + 偵測；做 `fault_injector` 故意搗亂。
4. 看門狗 + 預先配置 logger。
5. 跑基準、存 CSV、畫 p95/p99 圖貼到 `results.md`，錄 60 秒 demo。

# 延伸（加分）

* 切換 EDF vs Rate-Monotonic 小排程器，展示截止時間違約率差異。
* 以 CPU affinity 把 `gate_thread` 固定在 core 0，量化 jitter 改善。
* 將「死鎖事件」以 UART 輸出簡易 ASCII 圖，面試秀一眼就懂。
