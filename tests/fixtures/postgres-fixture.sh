#!/usr/bin/env bash
#
# postgres-fixture.sh - Start/Stop a temporary PostgreSQL instance for testing
#
# Usage: ./postgres-fixture.sh <setup|cleanup> <state_dir>
#
# Environment variables:
#   POSTGRES_TEST_PORT: The port to bind to (default: 55432)
#

set -euo pipefail

action="${1:?expected setup or cleanup}"
state_dir="${2:?expected state directory}"
port="${POSTGRES_TEST_PORT:-55432}"
env_file="${state_dir}/postgres.env"

cleanup() {
  if [[ -f "${env_file}" ]]; then
    # shellcheck disable=SC1090
    source "${env_file}"
    pg_ctl -D "${POSTGRES_DATA_DIR}" stop -m fast >/dev/null 2>&1 || true
    rm -rf "${POSTGRES_DATA_DIR}"
  fi
  rm -rf "${state_dir}"
}

case "${action}" in
  setup)
    cleanup
    mkdir -p "${state_dir}"
    data_dir="$(mktemp -d "${TMPDIR:-/tmp}/libcoda-postgres.XXXXXX")"
    {
      printf 'POSTGRES_DATA_DIR=%q\n' "${data_dir}"
      printf 'POSTGRES_URI=%q\n' "postgres://127.0.0.1:${port}/test"
    } > "${env_file}"
    trap cleanup ERR

    initdb -D "${data_dir}" --auth=trust >/dev/null
    pg_ctl -D "${data_dir}" -l "${state_dir}/postgres.log" -o "-h 127.0.0.1 -p ${port} -k ${data_dir}" start >/dev/null
    createdb -h 127.0.0.1 -p "${port}" test
    trap - ERR
    ;;
  cleanup)
    cleanup
    ;;
  *)
    echo "unsupported action: ${action}" >&2
    exit 2
    ;;
esac
