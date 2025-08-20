#!/bin/bash

#idea -> sync PrevCloud users with system users with UIDS >= 1_000 (the so-called local users in the problem)

#- rms: Richard Stallman
#- dennis: Dennis Ritchie
#- brian: Brian Kernigha- rms: Richard Stallman

source_file="/etc/passwd"
[[ -n "$PASSWD" ]] && { source_file="$PASSWD"; }

user_exists_in_prevcl() {
    if grep -E -q "^- "$1":" < <(./occ user:list); then
        echo "true"
    else 
        echo "false"
    fi
}

unlock_if_locked() {
    #can call ./occ user:info username here coz we confirmed the user exists
    if grep -q '^- enabled: false' < <(./occ user:info "$1"); then
        ./occ user:enable "$1"
    fi
}i

while read user_line; do
    username="$(echo "$user_line" | sed -n -E 's/^- ([^:]+):.*/\1/p')"
    [[ -z $(cat "$source_file" | awk -F':' -v usr="$username" '$1==usr && $3 >= 1000 { print "exists" }') ]] && { ./occ user:disable "$username"; } 
done < <(./occ user:list)

while IFS=: read user _ uid _; do
    exists_in_prevcl=$(user_exists_in_prevcl "$user")
    if $exists_in_prevcl; then
        unlock_if_locked "$user"    
    else 
        ./occ user:add "$user"
    fi

done < <(cat "$source_file" | awk -F':' '$3 >= 1000')

exit 0
