#!/usr/bin/env bash
set -u

EBMC_BIN="${EBMC_BIN:-ebmc}"
BOUND="${1:-8}"
ROOT="$(cd "$(dirname "$0")" && pwd)"

run_case() {
    local name="$1"
    "$EBMC_BIN" "$ROOT/$name/sequence_detector.sv" \
        --top TopModule \
        --bound "$BOUND" \
        --trace
}

echo "Expected: $BOUND"
if run_case good; then
    echo "PASS"
else
    echo " FAILED" >&2
    exit 1
fi

echo
echo "Expected: counterexample"
if run_case bad; then
    echo "PASSED unexpectedly" >&2
    exit 1
else
    echo "Failed as expected"
fi
