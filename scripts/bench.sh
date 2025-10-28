#!/usr/bin/env bash
set -euo pipefail
[ -x ./gate_ctrl ] || { echo "Build first: make"; exit 1; }
rm -f results.csv
# run for N seconds, send INT, then KILL after 1s if needed
FAULTS="${FAULTS:-0}" BAD_LOCK="${BAD_LOCK:-0}" \
timeout --signal=INT --kill-after=1s "${1:-5}s" ./gate_ctrl || true
python3 - <<'PY'
import csv, math
xs=[]
with open('results.csv') as f:
    r=csv.DictReader(f)
    for row in r: xs.append(float(row['latency_ms']))
xs.sort()
def pct(p): return xs[max(0, math.ceil(p*len(xs))-1)] if xs else float('nan')
print(f"n={len(xs)} p50={pct(0.5):.3f}ms p95={pct(0.95):.3f}ms p99={pct(0.99):.3f}ms")
PY

