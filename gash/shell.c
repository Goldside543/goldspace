#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "shell.h"
#include "../kernel/io.h"
#include "../kernel/string.h"
#include "../fs/simple_fs.h"

// Function prototypes for file system shell commands
void shell_create(const char *args);
void shell_write(const char *args);
void shell_read(const char *args);
void shell_delete(const char *args);

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
    char *filename = my_strtok((char *)args, " ");
    char *data = my_strtok(NULL, "");
    if (filename != NULL && data != NULL) {
        int file_index = find_file_index(filename);
        if (file_index != -1) {
            int result = write_file(file_index, data, my_strlen(data));
            if (result == 0) {
                print("Data written successfully.\n");
                return;
            }
        }
    }
    print("Error: Could not write data to file.\n");
}

void shell_read(const char *name) {
    int file_index = find_file_index(name);
    if (file_index != -1) {
        char buffer[BLOCK_SIZE];
        int result = read_file(file_index, buffer, BLOCK_SIZE);
        if (result == 0) {
            print(buffer);
            print("\n");
            return;
        }
    }
    print("Error: Could not read file.\n");
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
    char *token = my_strtok((char *)command, " ");

    if (token == NULL) {
        return;
    }

    switch (token[0]) {
        case 'h':
            shell_help();
            break;
        case 'e':
            token = my_strtok(NULL, "");
            if (token != NULL) {
                shell_echo(token);
            } else {
                print("echo: missing argument\n");
            }
            break;
        case 'c':
            token = my_strtok(NULL, "");
            if (token != NULL) {
                shell_create(token);
            } else {
                print("create: missing filename\n");
            }
            break;
        case 'w':
            token = my_strtok(NULL, "");
            if (token != NULL) {
                shell_write(token);
            } else {
                print("write: missing filename or data\n");
            }
            break;
        case 'r':
            token = my_strtok(NULL, "");
            if (token != NULL) {
                shell_read(token);
            } else {
                print("read: missing filename\n");
            }
            break;
        case 'd':
            token = my_strtok(NULL, "");
            if (token != NULL) {
                shell_delete(token);
            } else {
                print("delete: missing filename\n");
            }
            break;
        default:
            print("Command not found. Type 'h' for a list of commands.\n");
            break;
    }
}
