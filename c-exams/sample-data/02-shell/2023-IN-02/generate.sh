#!/bin/bash

# a) regular files with identical content
echo foo > identical-1
cp identical-1 identical-2
cp identical-1 identical-3

# b) group of hardlinks
echo bar > hardlink-1
ln hardlink-1 hardlink-2
ln hardlink-1 hardlink-3

# c) mixed files and hardlinks
echo baz > mixed-copy-1
cp mixed-copy-1 mixed-copy-2
cp mixed-copy-1 mixed-hardlink-1
ln mixed-hardlink-1 mixed-hardlink-2

# d) unique files
echo 'unique file 1' > unique-1
echo 'unique file 2' > unique-2
