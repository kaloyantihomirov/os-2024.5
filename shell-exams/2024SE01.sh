#!/bin/bash

#parse arguments -> we don't know how many arguments we have
#loop and get $1, $2, $3, ... while $x is not empty
#we need to have the files first and then begin processing them with the -R<word1>=<word2> arguments

find_unique_str() {
    #didn't quite understand how to use pwgen to generate a str that for sure won't occur in the file, so let's build our own unique str
    #not a very clever algorithm but it will work for sure
    built_str="kOOOkIIi"
    while grep -q "$built_str" "$1"; do
        built_str="${build_str}a"
    done
    echo "$built_str"
}

i=1
files=""
replacements=""
while [[ -n "${!i}" ]]; do
    count=$(echo "${!i}" | grep -Ec '^-R[a-zA-Z0-9]+=[a-zA-Z0-9]+$')
    if [[ "$count" -eq 0 ]]; then
        files="$files "${!i}""
    else
        replacements="$replacements "${!i}""
    fi
    i=$((i + 1))
done

# files=" file1 file2 file3 file4"
for file in $files; do
    [[ ! -f "$file" ]] && { echo "file "$file" does not exist, so we will skip it :(" >&2; exit 1; }
    curr_unique_str=$(find_unique_str "$file")
    for replacement in $replacements; do
        target=$(echo "$replacement" | sed -E 's/^-R([a-zA-Z0-9]+)=[a-zA-Z0-9]+$/\1/')
        replacement=$(echo "$replacement" | sed -E 's/^-R[a-zA-Z0-9]+=([a-zA-Z0-9]+)$/\1/')
        sed -E -i "/^#/! s/\<$target\>/${replacement}${curr_unique_str}/g" "$file"    
        #cat "$file"
    done

    sed -i "s/$curr_unique_str//g" "$file"
done

exit 0

