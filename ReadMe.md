## 🚗 Parking Gate Controller (Concurrent Systems Simulation)

### Overview

This project simulates a **multi-threaded parking gate control system** on Linux using **C and POSIX threads (pthreads)**.
It demonstrates **concurrency control, deadlock avoidance, performance measurement**, and **fault injection** — all core skills in embedded and operating system development.

Each thread represents a real-world parking subsystem:

* **OCR Thread** – Detects cars entering the parking lot and reads license plates.
* **Billing Thread** – Validates payment and triggers gate opening.
* **Gate Thread** – Simulates the physical gate actuator.
* **Alarm Thread** – Monitors and logs system status.

---

### 🧱 System Architecture

```
        +------------------+
        |  OCR Thread      |   → Detects plates, timestamps event
        +------------------+
                  |
                  v
        +------------------+
        |  Queue (Mutex +  |   → Thread-safe bounded buffer
        |  Cond Vars)      |
        +------------------+
                  |
                  v
        +------------------+
        |  Billing Thread  |   → Validates payment, actuates gate
        +------------------+
                  |
                  v
        +------------------+
        |  Gate Thread     |   → Opens/Closes gate
        +------------------+
                  |
                  v
        +------------------+
        |  Alarm Thread    |   → Periodic monitoring/logging
        +------------------+
```

---

### 🔒 Concurrency & Synchronization

**Shared resources** are protected by four global mutexes:

| Mutex        | Protects               | Example of Use          |
| ------------ | ---------------------- | ----------------------- |
| `m_plate_db` | License plate database | Reading/writing records |
| `m_payment`  | Payment status         | Updating user balance   |
| `m_gate_io`  | Gate actuator          | Sending I/O signals     |
| `m_logbuf`   | Log buffer             | Writing concurrent logs |

All locks follow a **global lock order**:

```
plate_db → payment → gate_io → logbuf
```

This order guarantees **deadlock avoidance**.

---

### ⚙️ Build & Run

#### Requirements

* GCC or Clang
* Linux / macOS with POSIX thread support

#### Build

```bash
make
```

#### Run

```bash
./gate_ctrl
```

To simulate random delays and lock-order faults:

```bash
FAULTS=1 BAD_LOCK=1 ./gate_ctrl
```

---

### 🧪 Benchmark Automation

Use the included script to run the system for N seconds and automatically analyze latency:

```bash
chmod +x scripts/bench.sh
./scripts/bench.sh 8
```

Example output:

```
n=25  p50=10.45ms  p95=10.58ms  p99=10.63ms
```

---

### 📊 Metrics Explained

Each car event logs **end-to-end latency** from detection (OCR) → gate actuation (Billing/Gate).

| Metric  | Meaning                                                         |
| ------- | --------------------------------------------------------------- |
| **p50** | Median latency — half the events are faster.                    |
| **p95** | 95th percentile — only 5% are slower.                           |
| **p99** | 99th percentile — tail latency, measures jitter and contention. |

The results are saved in `results.csv`:

```
plate,latency_ms
ABC0001,10.233
ABC0002,10.445
...
```

---

### 💥 Fault Injection

| Flag         | Description                                                                                                    |
| ------------ | -------------------------------------------------------------------------------------------------------------- |
| `FAULTS=1`   | Adds random jitter (0–9 ms) to simulate real CPU or I/O delays.                                                |
| `BAD_LOCK=1` | Occasionally acquires locks in the wrong order (payment → plate_db) to create potential deadlocks for testing. |

These allow testing **performance stability** and preparing for **deadlock detection** in future upgrades.

---

### 🛠️ Future Work

* **WFG Thread** – Implement a *Wait-For Graph* to detect deadlocks dynamically.
* **Watchdog Thread** – Monitor thread heartbeats and queue depth, automatically reset stalled modules.
* **Visualization** – Plot latency histogram and queue utilization.

---

### 📚 Key Technical Concepts

* Multi-threaded concurrency (`pthread_create`, `pthread_join`)
* Mutex and condition variable synchronization
* Producer–Consumer pattern (bounded queue)
* Deadlock avoidance via global lock order
* Latency measurement and tail analysis
* Fault injection (chaos testing)
* Shell scripting for automated benchmarking

---

### 📄 Example Output

```
Starting Parking Gate Controller (Linux, pthreads)...
[Config] FAULTS=1 BAD_LOCK=1
[OCR] Detected ABC0001 -> queued
[Billing] OK for ABC0001
[Gate] Actuating for plate=ABC0001
[Gate] Heartbeat
[Alarm] Monitoring
...
```

---

### 🧠 Summary

This project models a real-time concurrent system that:

* Handles multiple processes safely through locking and synchronization.
* Avoids deadlocks with global lock ordering.
* Measures performance at the 50th, 95th, and 99th percentile.
* Stresses reliability through fault injection and chaos testing.

It’s a complete educational example of **concurrency, synchronization, and systems performance engineering in C**.

---
