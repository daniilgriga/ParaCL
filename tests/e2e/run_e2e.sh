#!/bin/bash

# Run end-to-end tests for ParaCL interpreter

set -u

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

if [ $# -eq 0 ]; then
    echo -e "${RED}ERROR: Please provide path to binary${NC}"
    echo "Usage: $0 <path_to_paracl>"
    echo "Example: $0 ./build/paracl"
    exit 1
fi

PROGRAM_BIN="$1"
TEST_DIR="$(dirname "$0")"
PASSED=0
FAILED=0

if [ ! -f "$PROGRAM_BIN" ]; then
    echo -e "${RED}ERROR: $PROGRAM_BIN not found!${NC}"
    exit 1
fi

echo "Running end-to-end tests for ParaCL"
echo "========================================"

for dat_file in "$TEST_DIR"/*.dat; do
    test_id="$(basename "${dat_file%.dat}")"
    ans_file="$TEST_DIR/${test_id}.ans"
    rc_file="$TEST_DIR/${test_id}.rc"
    err_file="$TEST_DIR/${test_id}.err"

    if [ ! -f "$ans_file" ]; then
        echo -e "${YELLOW}WARNING: $ans_file not found, skipping...${NC}"
        continue
    fi

    expected_rc=0
    if [ -f "$rc_file" ]; then
        expected_rc=$(cat "$rc_file")
    fi

    if ! [[ "$expected_rc" =~ ^-?[0-9]+$ ]]; then
        echo -e "Test $test_id: ${RED}[FAILED]${NC}"
        echo "  Invalid expected exit code in $rc_file: '$expected_rc'"
        ((FAILED++))
        continue
    fi

    tmp_stdout=$(mktemp)
    tmp_stderr=$(mktemp)
    tmp_diff=$(mktemp)
    tmp_err_diff=$(mktemp)

    in_file="$TEST_DIR/${test_id}.in"
    if [ -f "$in_file" ]; then
        "$PROGRAM_BIN" "$dat_file" < "$in_file" >"$tmp_stdout" 2>"$tmp_stderr"
    else
        "$PROGRAM_BIN" "$dat_file" >"$tmp_stdout" 2>"$tmp_stderr"
    fi
    actual_rc=$?

    output_ok=0
    if diff -u "$ans_file" "$tmp_stdout" >"$tmp_diff"; then
        output_ok=1
    fi

    exit_ok=0
    if [ "$actual_rc" -eq "$expected_rc" ]; then
        exit_ok=1
    fi

    err_ok=1
    if [ -f "$err_file" ]; then
        if ! diff -u "$err_file" "$tmp_stderr" >"$tmp_err_diff"; then
            err_ok=0
        fi
    fi

    if [ "$output_ok" -eq 1 ] && [ "$exit_ok" -eq 1 ] && [ "$err_ok" -eq 1 ]; then
        echo -e "Test $test_id: ${GREEN}[PASSED]${NC}"
        ((PASSED++))
    else
        echo -e "Test $test_id: ${RED}[FAILED]${NC}"
        if [ "$exit_ok" -ne 1 ]; then
            echo "  Exit code mismatch: expected $expected_rc, got $actual_rc"
        fi
        if [ "$output_ok" -ne 1 ]; then
            echo "  Stdout diff:"
            cat "$tmp_diff"
        fi
        if [ "$err_ok" -ne 1 ]; then
            echo "  Stderr diff:"
            cat "$tmp_err_diff"
        fi
        ((FAILED++))
    fi

    rm -f "$tmp_stdout" "$tmp_stderr" "$tmp_diff" "$tmp_err_diff"
done

echo "========================================"
echo "Results: $PASSED passed, $FAILED failed"

if [ $FAILED -gt 0 ]; then
    exit 1
else
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
fi
