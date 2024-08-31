#!/bin/bash

# Check if a Makefile exists in the current directory
if [ ! -f Makefile ]; then
    echo "Makefile not found in the current directory."
    exit 1
fi

# Replace gcc with i686-linux-gnu-gcc
sed -i 's/\bgcc\b/i686-linux-gnu-gcc/g' Makefile

# Replace ld with i686-linux-gnu-ld
sed -i 's/\bld\b/i686-linux-gnu-ld/g' Makefile

echo "Replaced 'gcc' with 'i686-linux-gnu-gcc' and 'ld' with 'i686-linux-gnu-ld' in the Makefile."
