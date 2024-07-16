#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "shell.h"
#include "../kernel/io.h"
#include "../kernel/string.h"
#include "../fs/simple_fs.h"

// Function prototypes for file system shell commands
void shell_create_file(const char *name);
void shell_write_file(const char *name, const char *data);
void shell_read_file(const char *name);
void shell_delete_file(const char *name);

void print(const char *str);

void shell_help() {
    print("Available commands:\n");
    print("help - Display this help message\n");
    print("echo <message> - Echo the message back to the screen\n");
    print("clear - Clear the console screen\n");
    print("create <filename> - Create a new file\n");
    print("write <filename> <data> - Write data to a file\n");
    print("read <filename> - Read data from a file\n");
    print("delete <filename> - Delete a file\n");
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

void shell_create_file(const char *name) {
    int result = create_file(name);
    if (result >= 0) {
        print("File created successfully.\n");
    } else {
        print("Error: Could not create file.\n");
    }
}

void shell_write_file(const char *name, const char *data) {
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

    int result = write_file(file_index, data, my_strlen(data));
    if (result == 0) {
        print("Data written successfully.\n");
    } else {
        print("Error: Could not write data to file.\n");
    }
}

void shell_read_file(const char *name) {
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

void shell_delete_file(const char *name) {
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

    if (my_strcmp(token, "help") == 0) {
        shell_help();
    } else if (my_strcmp(token, "echo") == 0) {
        token = my_strtok(NULL, "");
        if (token != NULL) {
            shell_echo(token);
        } else {
            print("echo: missing argument\n");
        }
    } else if (my_strcmp(token, "clear") == 0) {
        shell_clear();
    } else if (my_strcmp(token, "create") == 0) {
        token = my_strtok(NULL, "");
        if (token != NULL) {
            shell_create_file(token);
        } else {
            print("create: missing filename\n");
        }
    } else if (my_strcmp(token, "write") == 0) {
        char *filename = my_strtok(NULL, " ");
        char *data = my_strtok(NULL, "");
        if (filename != NULL && data != NULL) {
            shell_write_file(filename, data);
        } else {
            print("write: missing filename or data\n");
        }
    } else if (my_strcmp(token, "read") == 0) {
        token = my_strtok(NULL, "");
        if (token != NULL) {
            shell_read_file(token);
        } else {
            print("read: missing filename\n");
        }
    } else if (my_strcmp(token, "delete") == 0) {
        token = my_strtok(NULL, "");
        if (token != NULL) {
            shell_delete_file(token);
        } else {
            print("delete: missing filename\n");
        }
    } else {
        print("Command not found. Type 'help' for a list of commands.\n");
    }
}
