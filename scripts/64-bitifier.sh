#!/bin/bash

# Directory containing the kernel source code
SOURCE_DIR="path/to/kernel/source"

# Temporary file for processing
TEMP_FILE="tempfile"

# Find and replace "unsigned long" with "uint64_t" and add #include <stdint.h>
find "$SOURCE_DIR" -type f \( -name "*.c" -o -name "*.h" \) | while read -r file; do
    # Create a temporary file to store changes
    cp "$file" "$TEMP_FILE"

    # Replace "unsigned long" with "uint64_t" in the temporary file
    sed -i 's/\bunsigned long\b/uint64_t/g' "$TEMP_FILE"

    # Check if we need to add #include <stdint.h>
    if ! grep -q '^#include <stdint.h>' "$TEMP_FILE"; then
        # Add #include <stdint.h> at the top of the file
        sed -i '1i #include <stdint.h>' "$TEMP_FILE"
    fi

    # Move the temporary file back to the original file
    mv "$TEMP_FILE" "$file"
done

# Remove the temporary file if it exists
[ -f "$TEMP_FILE" ] && rm "$TEMP_FILE"

echo "The 64-bitification is done."
