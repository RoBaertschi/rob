#!/bin/bash

BUILD="gcc src/main.c \
    src/stb_ds.c\
    src/lexer.c\
    src/parser.c\
    src/common.c\
    src/rob_string.c\
    src/var_string.c\
    -Wall -Wextra -pedantic -o rob"


echo $BUILD
$($BUILD)
