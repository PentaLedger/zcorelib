#!/usr/bin/env bash
#
# Format every tracked C/C++ source file in place with clang-format.
#
# Pass --check to verify formatting without writing, which is exactly what the
# CI format job runs.
#
# Usage: tools/format.sh [--check]
#
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${repo_root}"

if ! command -v clang-format >/dev/null 2>&1; then
  echo "error: clang-format not found." >&2
  echo "  macOS:  brew install clang-format" >&2
  echo "  Debian: apt-get install clang-format" >&2
  exit 1
fi

mapfile -t files < <(git ls-files '*.c' '*.h' '*.cpp' '*.hpp')

if [[ ${#files[@]} -eq 0 ]]; then
  echo "no source files tracked"
  exit 0
fi

if [[ "${1:-}" == "--check" ]]; then
  echo "==> Checking ${#files[@]} files with $(clang-format --version)"
  clang-format --dry-run --Werror "${files[@]}"
  echo "==> Formatting OK"
else
  echo "==> Formatting ${#files[@]} files with $(clang-format --version)"
  clang-format -i "${files[@]}"
  echo "==> Done"
fi
