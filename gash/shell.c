// SPDX-License-Identifier: GPL-2.0-only
/*
 * gash/shell.c
 *
 * Main file of Gash, a shell for testing Goldspace.
 *
 * Copyright (C) 2024-2025 Goldside543
 *
 */

#include <stddef.h>
#include <stdint.h>
#include "shell.h"
#include "../kernel/io.h"
#include "../kernel/string.h"
#include "../drivers/graphics.h"
#include "../kernel/panic.h"
#include "../drivers/vga.h"
#include "../drivers/pci.h"

const char *build_date = __DATE__;    // Compile date
const char *build_time = __TIME__;    // Compile time

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
    print("render - Test rendering\n");
    print("panic - Trigger a kernel panic\n");
    print("builddate - Print build date and time\n");
    print("mode13h - Switch to graphics mode 13h\n");
    print("scan - Scan PCI bus for devices\n");
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
    print("\n");
    print("Triggering kernel panic.\n");
    panic("Triggered by running panic.");
}

void shell_date() {
    print("\n");
    print("This kernel was compiled on ");
    print(build_date);
    print(" at ");
    print(build_time);
    print(".\n");
}

void shell_graphics() {
    print("\n");
    print("Switching graphics mode...\n");
    set_mode_13h();
    draw_rectangle(110, 75, 100, 50, 4);
}

void shell_scan() {
    print("\n");
    print("Scanning PCI bus...\n");
    pci_scan_bus();
    find_rtl8139_dma_address();
}

double calculate(double num1, char operator, double num2) {
    switch (operator) {
        case '+':
            return num1 + num2;
        case '-':
            return num1 - num2;
        case '*':
            return num1 * num2;
        case '/':
            if (num2 != 0)
                return num1 / num2;
            else {
                return 0; // Return 0 for error case
            }
        default:
            return 0; // Return 0 for invalid operator
    }
}

int isspace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f');
}

double atof(const char *str) {
    double result = 0.0;
    double sign = 1.0;
    int i = 0;

    // Handle leading spaces
    while (isspace(str[i])) {
        i++;
    }

    // Handle optional sign
    if (str[i] == '-') {
        sign = -1.0;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    // Convert integer part
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10.0 + (str[i] - '0');
        i++;
    }

    // Handle the decimal point and fractional part
    if (str[i] == '.') {
        i++;
        double decimal_place = 0.1;
        while (str[i] >= '0' && str[i] <= '9') {
            result += (str[i] - '0') * decimal_place;
            decimal_place /= 10.0;
            i++;
        }
    }

    return result * sign;
}

double shell_calculate(const char *args) {
    int state = 0;  // 0 = reading number 1, 1 = reading operator, 2 = reading number 2
    char current_char;
    char num1_str[100], num2_str[100];  // Temporary storage for numbers as strings
    int num1_idx = 0, num2_idx = 0;
    double num1 = 0, num2 = 0;
    char operator = '\0';

    while ((current_char = *args++) != '\0') {
        switch (state) {
            case 0: // Reading number 1
                if (current_char == ' ') {
                    num1_str[num1_idx] = '\0';  // Null-terminate the first number string
                    num1 = atof(num1_str);  // Convert the first number to a double
                    state = 1;  // Move to reading operator
                } else {
                    num1_str[num1_idx++] = current_char;
                }
                break;

            case 1: // Reading operator
                if (!isspace(current_char)) {
                    operator = current_char;  // Save the operator
                    state = 2;  // Move to reading number 2
                }
                break;

            case 2: // Reading number 2
                if (current_char != '\0' && current_char != ' ') {
                    num2_str[num2_idx++] = current_char;
                } else if (current_char == '\0') {
                    num2_str[num2_idx] = '\0';  // Null-terminate the second number string
                    num2 = atof(num2_str);  // Convert the second number to a double
                    state = 3;  // We are done, go to the calculation step
                }
                break;
        }
    }

    // After parsing the inputs, call the calculate function
    if (state == 3 && operator != '\0') {
        double result = calculate(num1, operator, num2);
        return result;
    } else {
        return 0;
    }
}

extern void jump_usermode(void);

void shell_usermode() {
   jump_usermode();
}

void shell_render() {
    draw_rectangle(50, 75, 100, 50, 1);    
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
    } else if (my_strcmp(command_name, "clear") == 0) {
        shell_clear();
    } else if (my_strcmp(command_name, "render") == 0) {
        shell_render(); // Execute the render command
    } else if (my_strcmp(command_name, "panic") == 0) { // Handle the panic command
        shell_panic(); // Trigger kernel panic
    } else if (my_strcmp(command_name, "builddate") == 0) {
        shell_date();
    } else if (my_strcmp(command_name, "mode13h") == 0) {
        shell_graphics();
    } else if (my_strcmp(command_name, "user") == 0) {
        shell_usermode();
    } else if (my_strcmp(command_name, "scan") == 0) {
        shell_scan();
    } else if (my_strcmp(command_name, "calculate") == 0) {
        if (*args != '\0') {
            char calculated_number = shell_calculate(args);
            char str[15] = {calculated_number, '\0'};
            print("\n");
            print(str);
            print("\n");
        } else {
            print("\n");
            print("calculate: missing arguments\n");
        }
    } else {
        print("\n");
        print("Unknown command: ");
        print(command_name);
        print("\n");
    }
}
