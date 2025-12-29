/*
 * switch - alternatives management tool for NurOS
 * Copyright (C) 2026 AnmiTaliDev <anmitali198@gmail.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef SWITCH_UTILS_H
#define SWITCH_UTILS_H

#include <stdbool.h>

/* Color codes */
typedef enum {
    COLOR_RESET = 0,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE,
    COLOR_BOLD
} color_t;

/* Initialize color support (auto-detect terminal capabilities) */
void color_init(void);

/* Check if colors are enabled */
bool color_enabled(void);

/* Enable or disable colors */
void color_set_enabled(bool enabled);

/* Get ANSI escape sequence for color */
const char *color_get(color_t color);

/* Print colored text to stdout */
void print_color(color_t color, const char *fmt, ...);

/* Print error message (red) to stderr */
void print_error(const char *fmt, ...);

/* Print warning message (yellow) to stderr */
void print_warning(const char *fmt, ...);

/* Print success message (green) to stdout */
void print_success(const char *fmt, ...);

/* Print info message (cyan) to stdout */
void print_info(const char *fmt, ...);

/* Check if file exists */
bool file_exists(const char *path);

/* Check if directory exists */
bool dir_exists(const char *path);

/* Check if file is executable */
bool is_executable(const char *path);

#endif /* SWITCH_UTILS_H */
