#!/bin/bash

buildPermOptions() {
    value=""
    if [[ "$1" = 'R' ]]; then
        value="$2"
    elif [[ "$1" = 'A' ]]; then
        value="/"$2""
    else
        value="-"$2""
    fi

    echo "-perm "$value""
}

[[ ! $# -eq 1 ]] && { echo "expected 1 argument" >&2; exit 1; }
[[ ! -f $1 ]] && { echo "file with rules does not exist" >&2; exit 2; }
#[[ ! $(whoami) = 'root' ]] && { echo "script should be ran as root" >&2; exit 3; } 

while read rule; do
  start_dir="$(echo -n "$rule" | grep -Eo "^[^[:space:]]+")"
  mode="$(echo -n "$rule" | grep -Eo '[[:space:]]+(R|A|T)[[:space:]]+' | sed -E 's/[[:space:]]+(R|A|T)[[:space:]]+/\1/')"
  perm="$(echo -n "$rule" | grep -Eo '[[:space:]]+[0-7]{3}[[:space:]]*$' | sed -E 's/[[:space:]]+([0-7]{3})[[:space:]]*$/\1/')"
  
  [[ -z "$mode" || -z "$perm" || -z "$start_dir" ]] && { echo "invalid format" >&2; exit 4; }
  find "$start_dir" -mindepth 1 -type f $(buildPermOptions "$mode" "$perm") -exec chmod 664 '{}' \; -o -mindepth 1 -type d $(buildPermOptions "$mode" "$perm") -exec chmod 755 '{}' \; 
  #echo "start_dir $start_dir mode $mode perm $perm"
done < <(cat "$1")
