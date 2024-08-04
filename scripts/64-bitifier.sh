#!/bin/bash

# Directory containing the kernel source code
SOURCE_DIR="/path/to/kernel/source"

# Find and replace "unsigned long" with "uint64_t"
find "$SOURCE_DIR" -type f \( -name "*.c" -o -name "*.h" \) -exec sed -i 's/\bunsigned long\b/uint64_t/g' {} +

echo "The 64-bitification is done."
