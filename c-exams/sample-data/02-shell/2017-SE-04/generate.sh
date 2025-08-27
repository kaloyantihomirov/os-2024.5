#!/bin/bash

ln -s /etc/services lbaz
ln -s /etc/passwd lqux
ln -s /etc/hosts lquux

seq 1 34 | xargs -I{} ln -s doesnt-exist broken-{}
