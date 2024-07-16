#include <stddef.h>
#include <stdint.h>
#include "shell.h"
#include "../kernel/io.h"
#include "../kernel/string.h"
#include "../fs/simple_fs.h"

void print(const char *str);

void shell_help() {
    print("Available commands:\n");
    print("h - Display this help message\n");
    print("e <message> - Echo the message back to the screen\n");
    print("c <filename> - Create a new file\n");
    print("w <filename> <data> - Write data to a file\n");
    print("r <filename> - Read data from a file\n");
    print("d <filename> - Delete a file\n");
}

void shell_echo(const char *message) {
    print(message);
    print("\n");
}

void shell_clear() {
    // This clears the screen by printing a ton of blank lines
    for (int i = 0; i < 25; ++i) {
        print("\n");
    }
}

void shell_create(const char *name) {
    int result = create_file(name);
    if (result >= 0) {
        print("File created successfully.\n");
    } else {
        print("Error: Could not create file.\n");
    }
}

void shell_write(const char *args) {
    // Parse filename and data manually
    char filename[100]; // Assuming maximum filename length
    char *data = NULL;

    // Copy filename until space or end of string
    int i = 0;
    while (args[i] != ' ' && args[i] != '\0') {
        filename[i] = args[i];
        i++;
    }
    filename[i] = '\0'; // Null-terminate filename

    // Move past the space to get the data
    if (args[i] == ' ') {
        data = args + i + 1;
    }

    if (filename[0] == '\0' || data == NULL || data[0] == '\0') {
        print("write: missing filename or data\n");
        return;
    }

    int file_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (my_strcmp(fs.files[i].name, filename) == 0) {
            file_index = i;
            break;
        }
    }

    if (file_index == -1) {
        print("Error: File not found.\n");
        return;
    }

    int result = write_file(file_index, data, my_strlen(data));
    if (result == 0) {
        print("Data written successfully.\n");
    } else {
        print("Error: Could not write data to file.\n");
    }
}

void shell_read(const char *name) {
    int file_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (my_strcmp(fs.files[i].name, name) == 0) {
            file_index = i;
            break;
        }
    }

    if (file_index == -1) {
        print("Error: File not found.\n");
        return;
    }

    char buffer[BLOCK_SIZE];
    int result = read_file(file_index, buffer, BLOCK_SIZE);
    if (result == 0) {
        print(buffer);
        print("\n");
    } else {
        print("Error: Could not read file.\n");
    }
}

void shell_delete(const char *name) {
    int file_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (my_strcmp(fs.files[i].name, name) == 0) {
            file_index = i;
            break;
        }
    }

    if (file_index == -1) {
        print("Error: File not found.\n");
        return;
    }

    int result = delete_file(file_index);
    if (result == 0) {
        print("File deleted successfully.\n");
    } else {
        print("Error: Could not delete file.\n");
    }
}

void shell_execute_command(const char *command) {
    // Find the first space or end of string to determine the command
    int command_end_index = 0;
    while (command[command_end_index] != ' ' && command[command_end_index] != '\0') {
        command_end_index++;
    }

    // Extract the command
    char command_name[2]; // Assuming single character commands
    strncpy(command_name, command, command_end_index);
    command_name[command_end_index] = '\0'; // Null-terminate the command name

    // Move past the space to get the arguments
    const char *args = command + command_end_index;
    while (*args == ' ') {
        args++; // Move past any leading spaces
    }

    // Execute the command based on the command name
    switch (command_name[0]) {
        case 'h':
            shell_help();
            break;
        case 'e':
            if (*args != '\0') {
                shell_echo(args);
            } else {
                print("echo: missing argument\n");
            }
            break;
        case 'c':
            if (*args != '\0') {
                shell_create(args);
            } else {
                print("create: missing filename\n");
            }
            break;
        case 'w':
            if (*args != '\0') {
                shell_write(args);
            } else {
                print("write: missing filename or data\n");
            }
            break;
        case 'r':
            if (*args != '\0') {
                shell_read(args);
            } else {
                print("read: missing filename\n");
            }
            break;
        case 'd':
            if (*args != '\0') {
                shell_delete(args);
            } else {
                print("delete: missing filename\n");
            }
            break;
        default:
            print("Command not found. Type 'h' for a list of commands.\n");
            break;
    }
}
