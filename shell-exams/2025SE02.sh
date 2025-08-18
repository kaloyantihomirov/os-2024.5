#!/bin/bash

env_name="SVC_DIR"
env_val="${!env_name}"

#echo "$env_val"
if [[ -z "$env_val" ]]; then
    echo "Required: the env $env_name variable must be set." >&2
    exit 1
fi

if [[ ! -d "$env_val" ]]; then
    echo "Error; Expected a directory for $env_name value." >&2
    exit 2
fi

allowed_modes="start stop running cleanup"
if [[ "$#" -lt 1 || "$#" -gt 2 ]]; then
    echo "Incorrect usage. Expected: mode [service_name]. Mode could be one from: $allowed_modes." >&2
    exit 3
fi

mode="$1"
is_allowed=false
for i_mode in $allowed_modes; do
    if [[ "$i_mode" = "$mode" ]]; then
        is_allowed=true
        break
    fi
done

if $is_allowed; then
    :
else
    echo "Unexpected mode. Supported modes are: $allowed_modes" >&2
    exit 4
fi

if [[ ( "$mode" = 'start' || "$mode" = 'stop' ) && ! "$#" -eq 2 ]]; then
    echo "Invalid usage! Expected: start/stop service_name" >&2
    exit 5
fi

service_name="$2"
if [[ "$mode" = 'running' ]]; then
    temp_file=$(mktemp)
fi

while read file; do
    #TODO: refactor -> obviously can extract a function that would take an attribute_name as a parameter
    #TODO: it would be cool to have an attribute_list="name pidfile outfile comm" and loop through it calling a func to get the attribute value from lines (overkill for exam)
    name=$(grep -E "^name:" "$file" | cut -d ':' -f2- | sed -E 's/^[[:space:]]*//')
    pidfile=$(grep -E "^pidfile:" "$file" | cut -d ':' -f2- | sed -E 's/^[[:space:]]*//') #assuming it exists
    outfile=$(grep -E "^outfile:" "$file" | cut -d ':' -f2- | sed -E 's/^[[:space:]]*//') #assuming it exists
    comm=$(grep -E "^comm:" "$file" | cut -d ':' -f2- | sed -E 's/^[[:space:]]*//')
    pid=$(cat "$pidfile")

    if [[ "$mode" = 'start' && "$name" = "$service_name" ]]; then
        if [[ -n "$pid" ]]; then
            ps_res=$(ps -p "$pid" -o pid=,s= | awk '$2="R" { print "yes" }')
            if [[ -n "$ps_res" ]]; then
                continue;
            fi
        fi
        bash -c "$comm &>$outfile &; echo \$0 > $outfile" '$!'
    elif [[ "$mode" = 'stop' && "$name" = "$service_name" ]]; then
        if [[ -z "$pid" ]]; then
            echo "Service was not started yet!" >&2
            exit 6
        fi
        kill -TERM "$pid"
    elif [[ "$mode" = 'running' ]]; then
        if [[ -n "$pid" ]]; then
            ps_res=$(ps -p "$pid" -o pid=,s= | awk '$2="R" { print "yes" }')
            if [[ -n "$ps_res" ]]; then
                echo "$name\n" >> "$temp_file"    
            fi
        fi 
    elif [[ "$mode" = 'cleanup' ]]; then
        if [[ -z "$pid" || -z "$(ps -p "$pid" -o pid=,s=)" ]]; then
            rm "$outfile"
        fi
    fi
done < <(find "$env_val" -type f) 


if [[ "$mode" = 'running' ]]; then
    echo "$(cat "$temp_file" | sort)"
    rm "$temp_file"
fi

exit 0
