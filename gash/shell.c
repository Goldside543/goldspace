// SPDX-License-Identifier: GPL-2.0-only
/*
 * gash/shell.c
 *
 * Main file of Gash, a shell for testing Goldspace.
 *
 * Copyright (C) 2024 Goldside543
 *
 */

#include <stddef.h>
#include <stdint.h>
#include "shell.h"
#include "../kernel/io.h"
#include "../kernel/string.h"
#include "../fs/simple_fs.h"
#include "../drivers/gpu.h"

extern void rust_panic(const void *info);

void print(const char *str);

void shell_help() {
    print("\n");
    print("Available commands:\n");
    print("help - Display this help message\n");
    print("echo <message> - Echo the message back to the screen\n");
    print("create <filename> - Create a new file\n");
    print("write <filename> <data> - Write data to a file\n");
    print("read <filename> - Read data from a file\n");
    print("delete <filename> - Delete a file\n");
    print("clear - Clear the screen\n");
    print("render - Test GPU rendering - Do not run if the GPU driver failed to load.\n");
}

void shell_echo(const char *message) {
    print("\n");
    print(message);
    print("\n");
}

void shell_clear() {
    // This clears the screen by printing a ton of blank lines
    for (int i = 0; i < 25; ++i) {
        print("\n");
    }
}

void shell_panic() {
    // This causes a kernel panic, defined in rust/src/lib.rs
    print("\n");
    print("Triggering kernel panic.\n");
    rust_panic(NULL);
}

void shell_create(const char *name) {
    int result = create_file(name);
    if (result >= 0) {
        print("\n");
        print("File created successfully.\n");
    } else {
        print("\n");
        print("Error: Could not create file.\n");
    }
}

void shell_write(const char *args) {
    // Parse filename and data manually
    char filename[100]; // Assuming maximum filename length
    char data[100];     // Assuming maximum data length
    int filename_idx = 0, data_idx = 0;
    int state = 0; // 0 for filename, 1 for data
    char current_char;

    // Iterate through the input arguments
    while ((current_char = *args++) != '\0') {
        switch (state) {
            case 0: // Parsing filename
                if (current_char == ' ') {
                    filename[filename_idx] = '\0'; // Null-terminate filename
                    state = 1; // Switch to data parsing state
                } else {
                    filename[filename_idx++] = current_char;
                }
                break;
            case 1: // Parsing data
                data[data_idx++] = current_char;
                break;
        }
    }
    data[data_idx] = '\0'; // Null-terminate data

    if (filename[0] == '\0' || data[0] == '\0') {
        print("\n");
        print("write: missing filename or data\n");
        return;
    }

    // Find file index by comparing filename
    int file_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        int match = 1;
        for (int j = 0; fs.files[i].name[j] != '\0' || filename[j] != '\0'; j++) {
            if (fs.files[i].name[j] != filename[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            file_index = i;
            break;
        }
    }

    if (file_index == -1) {
        print("\n");
        print("Error: File not found.\n");
        return;
    }

    // Write data to file
    int result = write_file(file_index, data, my_strlen(data));

    // Handle write_file errors based on the returned result
    switch (result) {
        case 0: // Success
            print("\n");
            print("Data written successfully.\n");
            break;
        case -1:
            print("\n");
            print("Error: Maximum number of files reached.\n");
            break;
        case -2:
            print("\n");
            print("Error: File does not exist.\n");
            break;
        case -3:
            print("\n");
            print("Error: No free blocks available.\n");
            break;
        case -4:
            print("\n");
            print("Error: Data size is too large.\n");
            break;
        case -5:
            print("\n");
            print("Error: Disk write failed.\n");
            break;
        default:
            print("\n");
            print("Error: Unknown error occurred while writing to the file.\n");
            break;
    }
}

void shell_read(const char *args) {
    // Parse filename manually
    char filename[100]; // Assuming maximum filename length
    int filename_idx = 0;
    char current_char;

    // Iterate through the input arguments
    while ((current_char = *args++) != '\0') {
        if (current_char == ' ') {
            filename[filename_idx] = '\0'; // Null-terminate filename
            break;
        }
        filename[filename_idx++] = current_char;
    }

    if (filename[0] == '\0') {
        print("\n");
        print("read: missing filename\n");
        return;
    }

    // Find file index by comparing filename
    int file_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
      // Only compare if the filename is not empty
      if (fs.files[i].name[0] != '\0') {
        int j = 0;
        // Compare characters until a null terminator is reached in either string
        while (fs.files[i].name[j] != '\0' && filename[j] != '\0') {
            if (fs.files[i].name[j] != filename[j]) {
                break; // Mismatch found
            }
            j++;
        }
        
        // If we reached the end of both strings, they match
        if (fs.files[i].name[j] == '\0' && filename[j] == '\0') {
            file_index = i;
            break; // Found the file, break out of the loop
        }
    }
}

    if (file_index == -1) {
        print("\n");
        print("Error: File not found.\n");
       return;
}

    // Read data from file
    char buffer[BLOCK_SIZE];
    int result = read_file(file_index, buffer, BLOCK_SIZE);
    
    // Handle read_file errors based on the returned result
    switch (result) {
        case 0: // Success
            print("\n");
            print(buffer);
            print("\n");
            break;
        case -1:
            print("\n");
            print("Error: Invalid file index.\n");
            break;
        case -2:
            print("\n");
            print("Error: File does not exist.\n");
            break;
        case -3:
            print("\n");
            print("Error: No data written to file.\n");
            break;
        case -4:
            print("\n");
            print("Error: Disk read failed.\n");
            break;
        default:
            print("\n");
            print("Error: Unknown error occurred while reading the file.\n");
            break;
    }
}

void shell_delete(const char *args) {
    // Parse filename manually
    char filename[100]; // Assuming maximum filename length
    int filename_idx = 0;
    char current_char;

    // Iterate through the input arguments
    while ((current_char = *args++) != '\0') {
        if (current_char == ' ') {
            filename[filename_idx] = '\0'; // Null-terminate filename
            break;
        }
        filename[filename_idx++] = current_char;
    }

    if (filename[0] == '\0') {
        print("\n");
        print("delete: missing filename\n");
        return;
    }

    // Find file index by comparing filename
    int file_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        int match = 1;
        for (int j = 0; fs.files[i].name[j] != '\0' || filename[j] != '\0'; j++) {
            if (fs.files[i].name[j] != filename[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            file_index = i;
            break;
        }
    }

    if (file_index == -1) {
        print("\n");
        print("Error: File not found.\n");
        return;
    }

    // Delete file
    int result = delete_file(file_index);
    if (result == 0) {
        print("\n");
        print("File deleted successfully.\n");
    } else {
        print("\n");
        print("Error: Could not delete file.\n");
    }
}

void shell_render() {
    gpu_render(); // Call the GPU render function
    print("\n");
    print("GPU render executed.\n");
}

void shell_execute_command(const char *command) {
    // Find the first space or end of string to determine the command
    int command_end_index = 0;
    while (command[command_end_index] != ' ' && command[command_end_index] != '\0') {
        command_end_index++;
    }

    // Extract the command
    char command_name[10]; // Assuming command names won't exceed 9 characters
    for (int i = 0; i < command_end_index; i++) {
        command_name[i] = command[i];
    }
    command_name[command_end_index] = '\0'; // Null-terminate the command name

    // Move past the space to get the arguments
    const char *args = command + command_end_index;
    while (*args == ' ') {
        args++; // Move past any leading spaces
    }

    // Execute the command based on the command name
    if (my_strcmp(command_name, "help") == 0) {
        shell_help();
    } else if (my_strcmp(command_name, "echo") == 0) {
        if (*args != '\0') {
            shell_echo(args);
        } else {
            print("\n");
            print("echo: missing argument\n");
        }
    } else if (my_strcmp(command_name, "create") == 0) {
        if (*args != '\0') {
            shell_create(args);
        } else {
            print("\n");
            print("create: missing filename\n");
        }
    } else if (my_strcmp(command_name, "write") == 0) {
        if (*args != '\0') {
            shell_write(args);
        } else {
            print("\n");
            print("write: missing filename or data\n");
        }
    } else if (my_strcmp(command_name, "read") == 0) {
        if (*args != '\0') {
            shell_read(args);
        } else {
            print("\n");
            print("read: missing filename\n");
        }
    } else if (my_strcmp(command_name, "delete") == 0) {
        if (*args != '\0') {
            shell_delete(args);
        } else {
            print("\n");
            print("delete: missing filename\n");
        }
    } else if (my_strcmp(command_name, "clear") == 0) {
        shell_clear();
    } else if (my_strcmp(command_name, "render") == 0) {
        shell_render(); // Execute the render command
    } else if (my_strcmp(command_name, "panic") == 0) { // Handle the panic command
        shell_panic(); // Trigger kernel panic
    } else {
        print("\n");
        print("Unknown command: ");
        print(command_name);
        print("\n");
    }
}
