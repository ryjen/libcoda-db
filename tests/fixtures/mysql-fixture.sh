#!/usr/bin/env bash
#
# mysql-fixture.sh - Start/Stop a temporary MySQL/MariaDB instance for testing
#
# Usage: ./mysql-fixture.sh <setup|cleanup> <state_dir>
#
# Environment variables:
#   MYSQL_TEST_PORT: The port to bind to (default: 53306)
#

set -euo pipefail

action="${1:?expected setup or cleanup}"
state_dir="${2:?expected state directory}"
port="${MYSQL_TEST_PORT:-53306}"
env_file="${state_dir}/mysql.env"

cleanup() {
  if [[ -f "${env_file}" ]]; then
    # shellcheck disable=SC1090
    source "${env_file}"
    mariadb-admin --protocol=tcp -h127.0.0.1 -P"${MYSQL_PORT}" -uroot shutdown >/dev/null 2>&1 || true
    if [[ -f "${MYSQL_PID_FILE}" ]]; then
      kill "$(cat "${MYSQL_PID_FILE}")" >/dev/null 2>&1 || true
    fi
    rm -rf "${MYSQL_DATA_DIR}"
  fi
  rm -rf "${state_dir}"
}

case "${action}" in
  setup)
    cleanup
    mkdir -p "${state_dir}"
    data_dir="$(mktemp -d "${TMPDIR:-/tmp}/libcoda-mysql.XXXXXX")"
    socket_path="${data_dir}/mysql.sock"
    pid_file="${state_dir}/mysql.pid"
    {
      printf 'MYSQL_DATA_DIR=%q\n' "${data_dir}"
      printf 'MYSQL_PID_FILE=%q\n' "${pid_file}"
      printf 'MYSQL_PORT=%q\n' "${port}"
      printf 'MYSQL_URI=%q\n' "mysql://root@127.0.0.1:${port}/test"
    } > "${env_file}"
    trap cleanup ERR

    mariadb-install-db \
      --datadir="${data_dir}" \
      --auth-root-authentication-method=normal \
      --skip-test-db >/dev/null

    mariadbd \
      --datadir="${data_dir}" \
      --socket="${socket_path}" \
      --bind-address=127.0.0.1 \
      --port="${port}" \
      --pid-file="${pid_file}" \
      --log-error="${state_dir}/mysql.log" &

    for _ in $(seq 1 100); do
      if mariadb-admin --protocol=tcp -h127.0.0.1 -P"${port}" -uroot ping >/dev/null 2>&1; then
        break
      fi
      sleep 0.1
    done

    mariadb-admin --protocol=tcp -h127.0.0.1 -P"${port}" -uroot ping >/dev/null
    mariadb --protocol=tcp -h127.0.0.1 -P"${port}" -uroot -e 'CREATE DATABASE test;'
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
