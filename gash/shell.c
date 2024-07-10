#include <stddef.h>
#include <stdint.h>
#include <string.h>

void print(const char *str);

void shell_help() {
    print("Available commands:\n");
    print("help - Display this help message\n");
    print("echo <message> - Echo the message back to the screen\n");
}

void shell_echo(const char *message) {
    print(message);
    print("\n");
}

void shell_execute_command(const char *command) {
    char *token = strtok((char *)command, " ");

    if (token == NULL) {
        return;
    }

    if (strcmp(token, "help") == 0) {
        shell_help();
    } else if (strcmp(token, "echo") == 0) {
        token = strtok(NULL, "");
        if (token != NULL) {
            shell_echo(token);
        } else {
            print("echo: missing argument\n");
        }
    } else {
        print("Command not found. Type 'help' for a list of commands.\n");
    }
}
