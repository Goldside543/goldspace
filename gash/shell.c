#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "shell.h"
#include "../kernel/io.h"
#include "../kernel/string.h"

void print(const char *str);

void shell_help() {
    print("Available commands:\n");
    print("help - Display this help message\n");
    print("echo <message> - Echo the message back to the screen\n");
    print("clear - Clear the console screen\n");
}

void shell_echo(const char *message) {
    print(message);
    print("\n");
}

void shell_clear() {
    // this clears the screen by printing a ton of blank lines
    for (int i = 0; i < 25; ++i) {
        print("\n");
    }
}

void shell_execute_command(const char *command) {

    char *token = my_strtok((char *)command, " ");

    if (token == NULL) {
        return;
    }

    if (my_strcmp(token, "h") == 0) {
        shell_help();
    } else if (my_strcmp(token, "echo") == 0) {
        token = my_strtok(NULL, "");
        if (token != NULL) {
            shell_echo(token);
        } else {
            print("echo: missing argument\n");
        }
    } else if (my_strcmp(token, "c") == 0) {
        shell_clear();
    } else {
        print("Command not found. Type 'help' for a list of commands.\n");
    }
}
