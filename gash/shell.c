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
#include "mandelbrot.h"
#include "../kernel/io.h"
#include "../kernel/string.h"
#include "../drivers/graphics.h"
#include "../kernel/panic.h"
#include "../drivers/vga.h"
#include "../drivers/pci.h"

const char *build_date = __DATE__;    // Compile date
const char *build_time = __TIME__;    // Compile time

void print(const char *str);
static void int_to_string(int num, char* str);

void shell_help() {
    print("\n");
    print("Available commands:\n");
    print("help - Display this help message\n");
    print("echo <message> - Echo the message back to the screen\n");
    print("clear - Clear the screen\n");
    print("render - Test rendering\n");
    print("panic - Trigger a kernel panic\n");
    print("builddate - Print build date and time\n");
    print("mode13h - Switch to graphics mode 13h\n");
    print("scan - Scan PCI bus for devices\n");
    print("kunk - Ask Kunk a yes/no question\n");
    print("reboot - Reboot the system\n");
    print("user - Switch to user mode\n");
    print("vendor - Display CPU vendor information\n");
    print("mandel - Render a Mandelbrot set\n");
    print("calculate <num1> <operator> <num2> - Perform basic arithmetic\n");
    print("\n");
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

extern int kunk;

void shell_kunk() {
   print("\n");
   print("Kunk is devising an answer to your question...\n");
   print("Kunk says... ");

   if (kunk == 0)
       print("Yes.\n");

   else if (kunk == 1)
       print("No.\n");
}

void shell_reboot() {
   uint8_t good = 0x02;

   while (good & 0x02)
      good = inb(0x64);

   outb(0x64, 0xFE);
}

void get_cpuid(unsigned int func, unsigned int *eax, unsigned int *ebx, unsigned int *ecx, unsigned int *edx) {
    asm volatile (
        "cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "a" (func)
    );
}

#define CPUID_VENDOR_AMD           "AuthenticAMD"
#define CPUID_VENDOR_AMD_OLD       "AMDisbetter!" // Early engineering samples of AMD K5 processor
#define CPUID_VENDOR_INTEL         "GenuineIntel"
#define CPUID_VENDOR_KVM           " KVMKVMKVM  "
#define CPUID_VENDOR_QEMU          "TCGTCGTCGTCG"
#define CPUID_VENDOR_VMWARE        "VMwareVMware"
#define CPUID_VENDOR_VIRTUALBOX    "VBoxVBoxVBox"
#define CPUID_VENDOR_XEN           "XenVMMXenVMM"
#define CPUID_VENDOR_HYPERV        "Microsoft Hv"


void shell_vendor() {
    print("\n");
    unsigned int eax = 0, ebx, ecx, edx;
    get_cpuid(eax, &eax, &ebx, &ecx, &edx);

    char vendor[13];
    ((unsigned int*)vendor)[0] = ebx;
    ((unsigned int*)vendor)[1] = edx;
    ((unsigned int*)vendor)[2] = ecx;
    vendor[12] = '\0';  // Null-terminate the string

    if (my_strcmp(vendor, CPUID_VENDOR_AMD) == 0)
      print("AuthenticAMD\n");

    else if (my_strcmp(vendor, CPUID_VENDOR_AMD_OLD) == 0)
      print("AuthenticAMD\n");

    else if (my_strcmp(vendor, CPUID_VENDOR_INTEL) == 0)
      print("GenuineIntel\n");

    else if (my_strcmp(vendor, CPUID_VENDOR_KVM) == 0)
      print("Generic KVM processor\n");

    else if (my_strcmp(vendor, CPUID_VENDOR_QEMU) == 0)
      print("Generic QEMU processor\n");

    else if (my_strcmp(vendor, CPUID_VENDOR_VMWARE) == 0)
      print("Generic VMware processor\n");

    else if (my_strcmp(vendor, CPUID_VENDOR_VIRTUALBOX) == 0)
      print("Generic VirtualBox processor\n");

    else if (my_strcmp(vendor, CPUID_VENDOR_XEN) == 0)
      print("Generic Xen processor\n");

    else if (my_strcmp(vendor, CPUID_VENDOR_HYPERV) == 0)
      print("Generic Microsoft Hyper-V processor\n");

    else
      print("Unknown processor\n");
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
                print("Error: Division by zero\n");
                return 0; // Return 0 for error case
            }
        default:
            print("Error: Invalid operator\n");
            return 0; // Return 0 for invalid operator
    }
}

// Helper function to convert integer to string
static void int_to_string(int num, char* str) {
    int i = 0, j = 0;
    char temp[16];
    
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    if (num < 0) {
        str[0] = '-';
        j = 1;
        num = -num;
    }
    
    // Convert number to string in reverse order
    while (num > 0) {
        temp[i++] = (num % 10) + '0';
        num /= 10;
    }
    
    // Reverse the string to get correct order
    for (int k = 0; k < i; k++) {
        str[j++] = temp[i - k - 1];
    }
    str[j] = '\0';
}

int isspace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f');
}

double atof(const char *str) {
    if (!str) return 0.0;
    
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
    if (!args) return 0.0;
    
    int state = 0;  // 0 = reading number 1, 1 = reading operator, 2 = reading number 2
    char current_char;
    char num1_str[100], num2_str[100];  // Temporary storage for numbers as strings
    int num1_idx = 0, num2_idx = 0;
    double num1 = 0, num2 = 0;
    char operator = '\0';

    // Initialize arrays
    for (int i = 0; i < sizeof(num1_str); i++) num1_str[i] = 0;
    for (int i = 0; i < sizeof(num2_str); i++) num2_str[i] = 0;

    while ((current_char = *args++) != '\0') {
        switch (state) {
            case 0: // Reading number 1
                if (current_char == ' ') {
                    if (num1_idx > 0) {
                        num1_str[num1_idx] = '\0';  // Null-terminate the first number string
                        num1 = atof(num1_str);  // Convert the first number to a double
                        state = 1;  // Move to reading operator
                    }
                } else {
                    if (num1_idx < sizeof(num1_str) - 1) {
                        num1_str[num1_idx++] = current_char;
                    }
                }
                break;

            case 1: // Reading operator
                if (!isspace(current_char)) {
                    operator = current_char;  // Save the operator
                    state = 2;  // Move to reading number 2
                }
                break;

            case 2: // Reading number 2
                if (current_char != ' ') {
                    if (num2_idx < sizeof(num2_str) - 1) {
                        num2_str[num2_idx++] = current_char;
                    }
                }
                break;
        }
    }

    // Handle the last number if we're still in state 2
    if (state == 2 && num2_idx > 0) {
        num2_str[num2_idx] = '\0';  // Null-terminate the second number string
        num2 = atof(num2_str);  // Convert the second number to a double
        state = 3;  // We are done, go to the calculation step
    }

    // After parsing the inputs, call the calculate function
    if (state == 3 && operator != '\0') {
        double result = calculate(num1, operator, num2);
        return result;
    } else {
        print("Error: Invalid expression format\n");
        return 0;
    }
}

extern void jump_usermode(void);

void shell_usermode() {
   print("\n");
   jump_usermode();
}

void shell_render() {
    draw_rectangle(50, 75, 100, 50, 1);    
    print("\n");
    print("GPU render executed.\n");
}

void shell_sysinfo() {
    print("\n");
    print("=== Goldspace System Information ===\n");
    print("Kernel Version: Beta 1.2.0\n");
    print("Architecture: x86 (32-bit)\n");
    print("Build Date: ");
    print(build_date);
    print("\n");
    print("Build Time: ");
    print(build_time);
    print("\n");
    
    // Display memory information
    extern uint32_t get_free_memory();
    extern uint32_t get_total_memory();
    char mem_str[32];
    int_to_string(get_total_memory() / 1024, mem_str);
    print("Total Memory: ");
    print(mem_str);
    print(" KB\n");
    
    int_to_string(get_free_memory() / 1024, mem_str);
    print("Free Memory: ");
    print(mem_str);
    print(" KB\n");
    
    // Display process information
    extern int get_process_count();
    int process_count = get_process_count();
    int_to_string(process_count, mem_str);
    print("Active Processes: ");
    print(mem_str);
    print("\n");
    
    print("====================================\n");
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
    } else if (my_strcmp(command_name, "kunk") == 0) {
        shell_kunk();
    } else if (my_strcmp(command_name, "reboot") == 0) {
        shell_reboot();
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
    } else if (my_strcmp(command_name, "vendor") == 0) {
        shell_vendor();
    } else if (my_strcmp(command_name, "mandel") == 0) {
        shell_mandelbrot();
    } else if (my_strcmp(command_name, "calculate") == 0) {
        if (*args != '\0') {
            double result = shell_calculate(args);
            // Convert the result to a string
            char str[32];
            // Simple conversion for integer results
            if (result == (int)result) {
                int_to_string((int)result, str);
            } else {
                // For floating point, we'll just show 2 decimal places
                int whole = (int)result;
                int fractional = (int)((result - whole) * 100);
                if (fractional < 0) fractional = -fractional; // Handle negative
                char whole_str[16], fractional_str[16];
                int_to_string(whole, whole_str);
                int_to_string(fractional, fractional_str);
                
                // Combine strings
                int i = 0, j = 0;
                // Handle negative sign for whole part
                if (result < 0 && whole >= 0) {
                    str[j++] = '-';
                }
                while (whole_str[i] != '\0') {
                    str[j++] = whole_str[i++];
                }
                str[j++] = '.';
                // Ensure two digits for fractional part
                if (fractional < 10) {
                    str[j++] = '0';
                }
                i = 0;
                while (fractional_str[i] != '\0') {
                    str[j++] = fractional_str[i++];
                }
                str[j] = '\0';
            }
            print("\n");
            print(str);
            print("\n");
        } else {
            print("\n");
            print("calculate: missing arguments\n");
        }
    } else if (my_strcmp(command_name, "sysinfo") == 0) {
        shell_sysinfo();
    } else {
        print("\n");
        print("Unknown command: ");
        print(command_name);
        print("\n");
        print("Type 'help' for a list of available commands.\n");
    }
}