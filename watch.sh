#!/usr/bin/env bash
GAME_PID="$1"
WATCH_DIRS=(src)
COLOR_RESET=$'\e[0m'
RED=$'\e[31m'
GREEN=$'\e[32m'
BLUE=$'\e[34m'

log() {
  local kind="$1"
  shift
  local message="$*"
  local color

  case "$kind" in
    INFO) color=$BLUE ;;
    OK) color=$GREEN ;;
    ERR) color=$RED ;;
    *) color=$COLOR_RESET ;;
  esac

  if [[ $kind == ERR ]]; then
    printf '%s[%s]%s %s\n' "$color" "$kind" "$COLOR_RESET" "$message" >&2
  else
    printf '%s[%s]%s %s\n' "$color" "$kind" "$COLOR_RESET" "$message"
  fi
}

exec 3< <(inotifywait -m -r -q -e close_write,create,delete,move \
  --format '%w%f' "${WATCH_DIRS[@]}")
INOTIFY_PID=$!
trap 'kill $INOTIFY_PID 2>/dev/null' EXIT
log INFO "Observing ${WATCH_DIRS[*]} (PID $GAME_PID)"

while kill -0 "$GAME_PID" 2> /dev/null; do
  if read -r -t 1 -u 3; then
    while read -r -t 0.3 -u 3 _; do :; done
    log INFO "recompiling..."
    if cmake --build build-hot --target wheel_of_fear_game -j; then
      log OK "$(date +%T)"
    else
      log ERR "dynamic link has failed"
    fi
  fi
done
echo "Jogo encerrado, saindo do watcher."
