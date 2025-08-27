#!/bin/bash

touch file

# a) hardlinks
touch hardlink-{a,b,c}-1
ln hardlink-a-1 hardlink-a-2
seq 2 3 | xargs -I{} ln hardlink-b-1 hardlink-b-{}
seq 2 5 | xargs -I{} ln hardlink-c-1 hardlink-c-{}

# b) symlinks
seq 1 3 | xargs -I{} ln -s file symlink-{}
seq 1 4 | xargs -I{} ln -s doesnt-exist broken-symlink-{}
