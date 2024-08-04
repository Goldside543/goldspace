#!/bin/bash

# Directory containing the kernel source code
SOURCE_DIR="path/to/kernel/source"
MAKEFILE_PATH="$SOURCE_DIR/Makefile"

# Temporary file for processing
TEMP_FILE="tempfile"

# Find and replace "unsigned long" with "uint64_t" and add #include <stdint.h>
find "$SOURCE_DIR" -type f \( -name "*.c" -o -name "*.h" \) | while read -r file; do
    # Create a temporary file to store changes
    cp "$file" "$TEMP_FILE"

    # Replace "unsigned long" with "uint64_t" in the temporary file
    sed -i 's/\bunsigned long\b/uint64_t/g' "$TEMP_FILE"

    # Replace "unsigned int" with "uint64_t"
    sed -i 's/\bunsigned int\b/uint64_t/g' "$TEMP_FILE"

    # Replace "elf_i386" with "elf_x86_64" in linker scripts
    sed -i 's/elf_i386/elf_x86_64/g' "$TEMP_FILE"

    # Replace "boot.asm" with "boot64.asm" in linker scripts and build files
    sed -i 's/boot.asm/boot64.asm/g' "$TEMP_FILE"

    # Replace "print.h" with "print64.h"
    sed -i 's/print.h/print64.h/g' "$TEMP_FILE"

    # Check if we need to add #include <stdint.h>
    if ! grep -q '^#include <stdint.h>' "$TEMP_FILE"; then
        # Add #include <stdint.h> at the top of the file
        sed -i '1i #include <stdint.h>' "$TEMP_FILE"
    fi

    # Move the temporary file back to the original file
    mv "$TEMP_FILE" "$file"
done

# Update the Makefile
if [ -f "$MAKEFILE_PATH" ]; then
    # Create a temporary file to store changes
    cp "$MAKEFILE_PATH" "$TEMP_FILE"

    # Replace "elf_i386" with "elf_x86_64"
    sed -i 's/elf_i386/elf_x86_64/g' "$TEMP_FILE"

    # Replace "boot.asm" with "boot64.asm"
    sed -i 's/boot.asm/boot64.asm/g' "$TEMP_FILE"

    # Replace "print.h" with "print64.h"
    sed -i 's/print.h/print64.h/g' "$TEMP_FILE"

    # Move the temporary file back to the original Makefile
    mv "$TEMP_FILE" "$MAKEFILE_PATH"
else
    echo "Makefile not found at $MAKEFILE_PATH"
fi

# Remove the temporary file if it exists
[ -f "$TEMP_FILE" ] && rm "$TEMP_FILE"

echo "The 64-bitification and Makefile updates are done."
