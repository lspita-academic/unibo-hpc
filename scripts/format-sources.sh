#!/usr/bin/env bash

DIR=${1:-"."}

find $DIR -type f -regex ".*\.\(c\|cu\|h\)$" -exec clang-format -i {} \;
