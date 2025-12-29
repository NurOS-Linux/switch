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

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

static bool g_color_enabled = false;

/* ANSI color codes */
static const char *color_codes[] = {
    [COLOR_RESET]   = "\033[0m",
    [COLOR_RED]     = "\033[31m",
    [COLOR_GREEN]   = "\033[32m",
    [COLOR_YELLOW]  = "\033[33m",
    [COLOR_BLUE]    = "\033[34m",
    [COLOR_MAGENTA] = "\033[35m",
    [COLOR_CYAN]    = "\033[36m",
    [COLOR_WHITE]   = "\033[37m",
    [COLOR_BOLD]    = "\033[1m"
};

void color_init(void)
{
    /* Check if stdout is a terminal */
    if (!isatty(STDOUT_FILENO)) {
        g_color_enabled = false;
        return;
    }

    /* Check TERM environment variable */
    const char *term = getenv("TERM");
    if (!term || strcmp(term, "dumb") == 0) {
        g_color_enabled = false;
        return;
    }

    /* Check NO_COLOR environment variable (standard) */
    if (getenv("NO_COLOR") != NULL) {
        g_color_enabled = false;
        return;
    }

    /* Check SWITCH_NO_COLOR for tool-specific override */
    if (getenv("SWITCH_NO_COLOR") != NULL) {
        g_color_enabled = false;
        return;
    }

    g_color_enabled = true;
}

bool color_enabled(void)
{
    return g_color_enabled;
}

void color_set_enabled(bool enabled)
{
    g_color_enabled = enabled;
}

const char *color_get(color_t color)
{
    if (!g_color_enabled) {
        return "";
    }
    return color_codes[color];
}

void print_color(color_t color, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    if (g_color_enabled) {
        fputs(color_codes[color], stdout);
    }

    vprintf(fmt, args);

    if (g_color_enabled) {
        fputs(color_codes[COLOR_RESET], stdout);
    }

    va_end(args);
}

void print_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    if (g_color_enabled) {
        fputs(color_codes[COLOR_RED], stderr);
        fputs(color_codes[COLOR_BOLD], stderr);
    }

    fputs("error: ", stderr);

    if (g_color_enabled) {
        fputs(color_codes[COLOR_RESET], stderr);
        fputs(color_codes[COLOR_RED], stderr);
    }

    vfprintf(stderr, fmt, args);

    if (g_color_enabled) {
        fputs(color_codes[COLOR_RESET], stderr);
    }

    fputc('\n', stderr);
    va_end(args);
}

void print_warning(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    if (g_color_enabled) {
        fputs(color_codes[COLOR_YELLOW], stderr);
        fputs(color_codes[COLOR_BOLD], stderr);
    }

    fputs("warning: ", stderr);

    if (g_color_enabled) {
        fputs(color_codes[COLOR_RESET], stderr);
        fputs(color_codes[COLOR_YELLOW], stderr);
    }

    vfprintf(stderr, fmt, args);

    if (g_color_enabled) {
        fputs(color_codes[COLOR_RESET], stderr);
    }

    fputc('\n', stderr);
    va_end(args);
}

void print_success(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    if (g_color_enabled) {
        fputs(color_codes[COLOR_GREEN], stdout);
    }

    vprintf(fmt, args);

    if (g_color_enabled) {
        fputs(color_codes[COLOR_RESET], stdout);
    }

    va_end(args);
}

void print_info(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    if (g_color_enabled) {
        fputs(color_codes[COLOR_CYAN], stdout);
    }

    vprintf(fmt, args);

    if (g_color_enabled) {
        fputs(color_codes[COLOR_RESET], stdout);
    }

    va_end(args);
}

bool file_exists(const char *path)
{
    if (!path) {
        return false;
    }

    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

bool dir_exists(const char *path)
{
    if (!path) {
        return false;
    }

    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

bool is_executable(const char *path)
{
    if (!path) {
        return false;
    }

    return access(path, X_OK) == 0;
}
