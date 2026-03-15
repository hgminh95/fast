#!/bin/bash
set -e

cd "$(dirname "$0")"

FIX=0
if [ "$1" = "--fix" ]; then
  FIX=1
fi

FAILED=0

echo "=== Python (ruff) ==="
if [ "$FIX" = "1" ]; then
  uv run ruff check --fix . && uv run ruff format . || FAILED=1
else
  uv run ruff check . && uv run ruff format --check . || FAILED=1
fi

echo ""
echo "=== C++ (clang-format) ==="
CPP_FILES=$(find bench -name '*.cpp' -o -name '*.h' | sort)
if [ "$FIX" = "1" ]; then
  echo "$CPP_FILES" | xargs clang-format -i && echo "Formatted." || FAILED=1
else
  echo "$CPP_FILES" | xargs clang-format --dry-run --Werror 2>&1 && echo "OK" || FAILED=1
fi

echo ""
echo "=== Bazel (buildifier) ==="
BZL_FILES=$(find bench -name 'BUILD.bazel' -o -name '*.bzl' | sort)
if [ "$FIX" = "1" ]; then
  echo "$BZL_FILES" | xargs buildifier && echo "Formatted." || FAILED=1
else
  echo "$BZL_FILES" | xargs buildifier --mode=check 2>&1 && echo "OK" || FAILED=1
fi

echo ""
echo "=== Zig (zig fmt) ==="
ZIG_BIN=$(find ~/.cache/bazel -name "zig" -type f -executable -path "*zig_0.14*" 2>/dev/null | head -1)
if [ -z "$ZIG_BIN" ]; then
  ZIG_BIN="zig"
fi
ZIG_FILES=$(find bench -name '*.zig' | sort)
if [ -n "$ZIG_FILES" ]; then
  if [ "$FIX" = "1" ]; then
    echo "$ZIG_FILES" | xargs "$ZIG_BIN" fmt && echo "Formatted." || FAILED=1
  else
    echo "$ZIG_FILES" | xargs "$ZIG_BIN" fmt --check 2>&1 && echo "OK" || FAILED=1
  fi
else
  echo "No Zig files found."
fi

echo ""
if [ "$FAILED" = "1" ]; then
  echo "LINT FAILED. Run './lint.sh --fix' to auto-fix."
  exit 1
else
  echo "All checks passed."
fi
