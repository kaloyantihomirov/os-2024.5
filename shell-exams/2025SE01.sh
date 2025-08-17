#!/bin/bash
#отне ми доста време да съобразя как да си извлека данните от всеки ред
#отне ми доста време да се сетя как да махна последния символ на всеки ред

if [[ ! "$#" -eq 2 ]]; then
    echo "Expected two arguments" >&2
    exit 1
fi

if [[ ! -f "$1" ]]; then
    echo "Config file does not exist (note that it should be a file!, not a directory or sth else)" >&2
    exit 2
fi
#не бих проверил дали съществува "$2", защото условието май ни насочва, че това е по-скоро отговорност на antrl4? Или?

config_file="$1" #предпочитам да носят някаква семантика $1 и $2 със себе си
input_file="$2"
#input_file_no_ext=$(basename "$input_file" | sed -E "s/(\.).*$//") първа идея - ако имаме bar.foo.gar.bar.saz.gr, ще върне bar, а на мене ми се струва, че е по-правилно да махнем само последното .<нещо_си>
input_file_no_ext=$(basename "$input_file" | awk -F'.' '{for (i=1; i<NF; i++) printf "%s%s", $i, (i<NF-1 ? "." : "")}')
#echo "$input_file_no_ext"
buildOptionsStr() { #takes shouldAddListenerOption & shouldAddVisitorOption
    res=""
    if $1; then 
        res="-no-listener"        
    fi

    if $2; then
        if [[ -z "$res" ]]; then
            res="-visitor"
        else 
            res="$res -visitor"
        fi
    fi

    echo "$res"
}

while read -r line; do
    language=$(echo "$line" | cut -d' ' -f1)
    o_dir=$(echo -n "$line" | grep -Eo "'.+'$" | cut -c2- | sed -E "s/\/?'$//")
    types_list=$(echo -n "$line" | cut -d' ' -f2- | cut -d"'" -f1)
    #echo "lang=$language o_dir $o_dir types_list $types_list"
    shouldAddListenerOption=true
    shouldAddVisitorOption=false
    for i in $types_list; do
        if [[ "$i" = 'listener' ]]; then
            shouldAddListenerOption=false
        fi

        if [[ "$i" = 'visitor' ]]; then
            shouldAddVisitorOption=true
        fi
    done
    options_str=$(buildOptionsStr "$shouldAddListenerOption" "$shouldAddVisitorOption")
    #echo "optionsStr $optionsStr"
    antlr4 -Dlanguage="$language" $options_str -o "$o_dir/$input_file_no_ext" "$input_file"
        
done < <(cat "$config_file")

exit 0
