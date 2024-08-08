/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SHELL_H
#define SHELL_H

void shell_help();
void shell_echo(const char *str);
void shell_clear();
void shell_execute_command(const char *command);

#endif // SHELL_H
