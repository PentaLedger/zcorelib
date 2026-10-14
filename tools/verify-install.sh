#!/usr/bin/env bash
#
# Prove that find_package(zcore) works against a real install tree.
#
# This is the round trip that an in-tree build never exercises and that most
# libraries therefore get wrong: configure -> build -> install to a throwaway
# prefix -> configure a *separate* consumer project against that prefix ->
# build and run it.
#
# Usage: tools/verify-install.sh [build-type]
#
set -euo pipefail

build_type="${1:-RelWithDebInfo}"

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
work_dir="$(mktemp -d)"
trap 'rm -rf "${work_dir}"' EXIT

build_dir="${work_dir}/build"
prefix_dir="${work_dir}/prefix"
consumer_dir="${work_dir}/consumer"

echo "==> Configuring zcorelib (${build_type})"
cmake -S "${repo_root}" -B "${build_dir}" \
  -DCMAKE_BUILD_TYPE="${build_type}" \
  -DCMAKE_INSTALL_PREFIX="${prefix_dir}" \
  -DZCORE_BUILD_TESTS=OFF \
  -DZCORE_BUILD_EXAMPLES=OFF \
  -DZCORE_BUILD_DOCS=OFF

echo "==> Building and installing to ${prefix_dir}"
cmake --build "${build_dir}" --config "${build_type}"
cmake --install "${build_dir}" --config "${build_type}"

echo "==> Installed tree:"
find "${prefix_dir}" -type f | sed "s|${prefix_dir}/||" | sort | sed 's/^/    /'

echo "==> Configuring the standalone consumer project"
cmake -S "${repo_root}/tests/package" -B "${consumer_dir}" \
  -DCMAKE_BUILD_TYPE="${build_type}" \
  -DCMAKE_PREFIX_PATH="${prefix_dir}"

echo "==> Building and running the consumer"
cmake --build "${consumer_dir}" --config "${build_type}"
ctest --test-dir "${consumer_dir}" --build-config "${build_type}" --output-on-failure

echo "==> find_package(zcore) verified"
