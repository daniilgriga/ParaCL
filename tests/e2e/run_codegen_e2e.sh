#!/bin/bash

# Run end-to-end tests for ParaCL compiler pipeline:
#   source -> .ll via paracl -> native executable via clang++ + pclrt -> run

set -u

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

if [ $# -eq 0 ]; then
    echo -e "${RED}ERROR: Please provide path to compiler binary${NC}"
    echo "Usage: $0 <path_to_paracl>"
    echo "Example: $0 ./build/llvm/paracl"
    exit 1
fi

COMPILER_BIN="$1"
TEST_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$TEST_DIR/../.." && pwd)"
RUNTIME_CPP="$REPO_ROOT/runtime/pclrt.cpp"
PASSED=0
FAILED=0
SKIPPED=0

if [ ! -f "$COMPILER_BIN" ]; then
    echo -e "${RED}ERROR: $COMPILER_BIN not found!${NC}"
    exit 1
fi

if [ ! -f "$RUNTIME_CPP" ]; then
    echo -e "${RED}ERROR: runtime source $RUNTIME_CPP not found!${NC}"
    exit 1
fi

if ! command -v clang++ >/dev/null 2>&1; then
    echo -e "${RED}ERROR: clang++ not found in PATH${NC}"
    exit 1
fi

echo "Running end-to-end tests for ParaCL compiler pipeline"
echo "====================================================="

rm -f "$TEST_DIR"/*.ll "$TEST_DIR"/*.dat.ll

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

    if [ "$expected_rc" -ne 0 ] || [ -f "$err_file" ]; then
        echo -e "Test $test_id: ${YELLOW}[SKIPPED]${NC}"
        echo "  Non-zero rc / stderr expectations are not supported by compiler e2e yet"
        ((SKIPPED++))
        continue
    fi

    tmp_stdout=$(mktemp)
    tmp_stderr=$(mktemp)
    tmp_diff=$(mktemp)
    tmp_compile_log=$(mktemp)
    tmp_codegen_log=$(mktemp)
    tmp_exe=$(mktemp "${TMPDIR:-/tmp}/paracl_codegen.XXXXXX")

    ll_file="${dat_file%.dat}.ll"
    legacy_ll_file="${dat_file}.ll"

    if ! "$COMPILER_BIN" "$dat_file" >"$tmp_codegen_log" 2>"$tmp_stderr"; then
        echo -e "Test $test_id: ${RED}[FAILED]${NC}"
        echo "  LLVM IR generation failed"
        cat "$tmp_stderr"
        rm -f "$tmp_stdout" "$tmp_stderr" "$tmp_diff" "$tmp_compile_log" \
              "$tmp_codegen_log" "$tmp_exe"
        ((FAILED++))
        continue
    fi

    if [ ! -f "$ll_file" ]; then
        echo -e "Test $test_id: ${RED}[FAILED]${NC}"
        echo "  Expected IR file $ll_file was not generated"
        rm -f "$tmp_stdout" "$tmp_stderr" "$tmp_diff" "$tmp_compile_log" \
              "$tmp_codegen_log" "$tmp_exe"
        ((FAILED++))
        continue
    fi

    if ! clang++ "$ll_file" "$RUNTIME_CPP" -o "$tmp_exe" >"$tmp_compile_log" 2>&1; then
        echo -e "Test $test_id: ${RED}[FAILED]${NC}"
        echo "  clang++ failed while building native executable"
        cat "$tmp_compile_log"
        rm -f "$tmp_stdout" "$tmp_stderr" "$tmp_diff" "$tmp_compile_log" \
              "$tmp_codegen_log" "$tmp_exe"
        ((FAILED++))
        continue
    fi

    in_file="$TEST_DIR/${test_id}.in"
    if [ -f "$in_file" ]; then
        "$tmp_exe" < "$in_file" >"$tmp_stdout" 2>"$tmp_stderr"
    else
        "$tmp_exe" >"$tmp_stdout" 2>"$tmp_stderr"
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

    if [ "$output_ok" -eq 1 ] && [ "$exit_ok" -eq 1 ]; then
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
        if [ -s "$tmp_stderr" ]; then
            echo "  Stderr:"
            cat "$tmp_stderr"
        fi
        ((FAILED++))
    fi

    rm -f "$tmp_stdout" "$tmp_stderr" "$tmp_diff" "$tmp_compile_log" \
          "$tmp_codegen_log" "$tmp_exe"
done

rm -f "$TEST_DIR"/*.ll "$TEST_DIR"/*.dat.ll

echo "====================================================="
echo "Results: $PASSED passed, $FAILED failed, $SKIPPED skipped"

if [ $FAILED -gt 0 ]; then
    exit 1
else
    echo -e "${GREEN}All supported compiler e2e tests passed!${NC}"
    exit 0
fi
