#!/bin/bash

# Count total lines in all .c and .h files recursively
find . -type f \( -name "*.c" -o -name "*.h" \) -print0 \
    | xargs -0 wc -l

echo "----------------------------------------"

TOTAL=$(find . -type f \( -name "*.c" -o -name "*.h" \) -print0 \
    | xargs -0 cat \
    | wc -l)

echo "Total lines: $TOTAL"