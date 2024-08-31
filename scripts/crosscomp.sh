#!/bin/bash

# Check if a Makefile exists in the current directory
if [ ! -f Makefile ]; then
    echo "Makefile not found in the current directory."
    exit 1
fi

# Replace gcc with i686-linux-gnu-gcc
sed -i 's/\bgcc\b/i686-linux-gnu-gcc/g' Makefile

echo "Replaced 'gcc' with 'i686-linux-gnu-gcc' in the Makefile."
