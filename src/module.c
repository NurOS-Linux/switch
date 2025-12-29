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

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include "module.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define INITIAL_CAPACITY 32

int module_list_init(module_list_t *list)
{
    if (!list) {
        return -1;
    }

    list->modules = malloc(INITIAL_CAPACITY * sizeof(module_info_t));
    if (!list->modules) {
        return -1;
    }

    list->count = 0;
    list->capacity = INITIAL_CAPACITY;
    return 0;
}

void module_list_free(module_list_t *list)
{
    if (!list) {
        return;
    }

    for (size_t i = 0; i < list->count; i++) {
        free(list->modules[i].name);
        free(list->modules[i].path);
        free(list->modules[i].description);
        free(list->modules[i].category);
        free(list->modules[i].link_path);
        free(list->modules[i].extra_links);
    }

    free(list->modules);
    list->modules = NULL;
    list->count = 0;
    list->capacity = 0;
}

void alternative_list_free(alternative_list_t *list)
{
    if (!list) {
        return;
    }

    for (size_t i = 0; i < list->count; i++) {
        free(list->items[i].path);
        free(list->items[i].name);
    }

    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

static int module_list_add(module_list_t *list, const char *name,
                           const char *path, bool is_user)
{
    if (list->count >= list->capacity) {
        size_t new_capacity = list->capacity * 2;
        module_info_t *new_modules = realloc(list->modules,
                                             new_capacity * sizeof(module_info_t));
        if (!new_modules) {
            return -1;
        }
        list->modules = new_modules;
        list->capacity = new_capacity;
    }

    module_info_t *module = &list->modules[list->count];
    memset(module, 0, sizeof(*module));

    module->name = strdup(name);
    module->path = strdup(path);
    module->is_user = is_user;

    if (!module->name || !module->path) {
        free(module->name);
        free(module->path);
        return -1;
    }

    list->count++;
    return 0;
}

static int scan_directory(module_list_t *list, const char *dir_path, bool is_user)
{
    if (!dir_exists(dir_path)) {
        return 0;
    }

    DIR *dir = opendir(dir_path);
    if (!dir) {
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }

        size_t len = strlen(entry->d_name);
        if (len < 4 || strcmp(entry->d_name + len - 3, ".sh") != 0) {
            continue;
        }

        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        if (!is_executable(path)) {
            continue;
        }

        char name[256];
        size_t name_len = len - 3;
        if (name_len >= sizeof(name)) {
            name_len = sizeof(name) - 1;
        }
        strncpy(name, entry->d_name, name_len);
        name[name_len] = '\0';

        bool exists = false;
        for (size_t i = 0; i < list->count; i++) {
            if (strcmp(list->modules[i].name, name) == 0) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            module_list_add(list, name, path, is_user);
        }
    }

    closedir(dir);
    return 0;
}

int module_scan(module_list_t *list, const switch_config_t *config)
{
    if (!list || !config) {
        return -1;
    }

    if (config->user_modules_dir) {
        scan_directory(list, config->user_modules_dir, true);
    }

    if (config->system_modules_dir) {
        scan_directory(list, config->system_modules_dir, false);
    }

    return 0;
}

const module_info_t *module_find(const module_list_t *list, const char *name)
{
    if (!list || !name) {
        return NULL;
    }

    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->modules[i].name, name) == 0) {
            return &list->modules[i];
        }
    }

    return NULL;
}

static char *read_pipe_output(int fd)
{
    char *output = NULL;
    size_t size = 0;
    size_t capacity = 0;
    char buf[256];
    ssize_t n;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        if (size + n + 1 > capacity) {
            capacity = capacity ? capacity * 2 : 256;
            char *new_output = realloc(output, capacity);
            if (!new_output) {
                free(output);
                return NULL;
            }
            output = new_output;
        }
        memcpy(output + size, buf, n);
        size += n;
    }

    if (output) {
        output[size] = '\0';
        while (size > 0 && (output[size - 1] == '\n' || output[size - 1] == '\r')) {
            output[--size] = '\0';
        }
    }

    return output;
}

static char *module_get_var(const module_info_t *module, const char *var_name)
{
    if (!module || !module->path || !var_name) {
        return NULL;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        return NULL;
    }

    pid_t pid = fork();
    if (pid == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        return NULL;
    }

    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        char cmd[512];
        snprintf(cmd, sizeof(cmd), "source \"%s\" && echo -n \"$%s\"",
                 module->path, var_name);
        execl("/bin/bash", "bash", "-c", cmd, NULL);
        _exit(1);
    }

    close(pipefd[1]);
    char *value = read_pipe_output(pipefd[0]);
    close(pipefd[0]);

    int status;
    waitpid(pid, &status, 0);

    return value;
}

int module_load_metadata(module_info_t *module)
{
    if (!module) {
        return -1;
    }

    if (!module->description) {
        module->description = module_get_var(module, "MODULE_DESCRIPTION");
    }
    if (!module->category) {
        module->category = module_get_var(module, "MODULE_CATEGORY");
    }
    if (!module->link_path) {
        module->link_path = module_get_var(module, "MODULE_LINK");
    }
    if (!module->extra_links) {
        module->extra_links = module_get_var(module, "MODULE_EXTRA_LINKS");
    }

    return 0;
}

int module_get_alternatives(const module_info_t *module, alternative_list_t *list)
{
    if (!module || !list || !module->path) {
        return -1;
    }

    list->items = malloc(INITIAL_CAPACITY * sizeof(alternative_t));
    if (!list->items) {
        return -1;
    }
    list->count = 0;
    list->capacity = INITIAL_CAPACITY;

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        free(list->items);
        return -1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        free(list->items);
        return -1;
    }

    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        char cmd[512];
        snprintf(cmd, sizeof(cmd), "source \"%s\" && find_alternatives", module->path);
        execl("/bin/bash", "bash", "-c", cmd, NULL);
        _exit(1);
    }

    close(pipefd[1]);
    char *output = read_pipe_output(pipefd[0]);
    close(pipefd[0]);

    int status;
    waitpid(pid, &status, 0);

    if (!output) {
        return 0;
    }

    /* Parse output: path|name|priority */
    char *line = strtok(output, "\n");
    while (line) {
        char *path = line;
        char *name = strchr(path, '|');
        if (!name) {
            line = strtok(NULL, "\n");
            continue;
        }
        *name++ = '\0';

        char *priority_str = strchr(name, '|');
        int priority = 10;
        if (priority_str) {
            *priority_str++ = '\0';
            priority = atoi(priority_str);
        }

        if (list->count >= list->capacity) {
            size_t new_cap = list->capacity * 2;
            alternative_t *new_items = realloc(list->items, new_cap * sizeof(alternative_t));
            if (!new_items) {
                break;
            }
            list->items = new_items;
            list->capacity = new_cap;
        }

        list->items[list->count].path = strdup(path);
        list->items[list->count].name = strdup(name);
        list->items[list->count].priority = priority;
        list->count++;

        line = strtok(NULL, "\n");
    }

    free(output);
    return 0;
}

void module_print_list(const module_list_t *list)
{
    if (!list) {
        return;
    }

    if (list->count == 0) {
        print_info("No modules found.\n");
        return;
    }

    printf("Available modules:\n");

    for (size_t i = 0; i < list->count; i++) {
        module_info_t *module = &list->modules[i];

        module_load_metadata(module);

        printf("  %s%-16s%s",
               color_get(COLOR_GREEN),
               module->name,
               color_get(COLOR_RESET));

        if (module->description) {
            printf(" - %s", module->description);
        }

        if (module->is_user) {
            printf(" %s[user]%s", color_get(COLOR_YELLOW), color_get(COLOR_RESET));
        }

        printf("\n");
    }
}

int module_action_list(const module_info_t *module)
{
    if (!module) {
        return -1;
    }

    module_info_t *m = (module_info_t *)module;
    module_load_metadata(m);

    if (!m->link_path) {
        print_error("Module does not define a link path");
        return -1;
    }

    alternative_list_t alts = {0};
    if (module_get_alternatives(module, &alts) != 0) {
        print_error("Failed to get alternatives");
        return -1;
    }

    if (alts.count == 0) {
        printf("No alternatives found for %s\n", module->name);
        alternative_list_free(&alts);
        return 0;
    }

    /* Get current target */
    char *current = NULL;
    if (file_exists(m->link_path) || is_executable(m->link_path)) {
        char buf[4096];
        ssize_t len = readlink(m->link_path, buf, sizeof(buf) - 1);
        if (len > 0) {
            buf[len] = '\0';
            current = realpath(m->link_path, NULL);
        }
    }

    printf("Available alternatives for %s%s%s:\n",
           color_get(COLOR_CYAN), module->name, color_get(COLOR_RESET));
    printf("  Link: %s\n\n", m->link_path);

    for (size_t i = 0; i < alts.count; i++) {
        char *real_path = realpath(alts.items[i].path, NULL);
        bool is_current = current && real_path && strcmp(current, real_path) == 0;

        if (is_current) {
            printf("  %s[*]%s ", color_get(COLOR_GREEN), color_get(COLOR_RESET));
        } else {
            printf("  [ ] ");
        }

        printf("%s%-12s%s  %s  (priority: %d)\n",
               color_get(COLOR_BOLD),
               alts.items[i].name,
               color_get(COLOR_RESET),
               alts.items[i].path,
               alts.items[i].priority);

        free(real_path);
    }

    free(current);
    alternative_list_free(&alts);
    return 0;
}

int module_action_show(const module_info_t *module)
{
    if (!module) {
        return -1;
    }

    module_info_t *m = (module_info_t *)module;
    module_load_metadata(m);

    if (!m->link_path) {
        print_error("Module does not define a link path");
        return -1;
    }

    printf("Module: %s%s%s\n", color_get(COLOR_CYAN), module->name, color_get(COLOR_RESET));
    if (m->description) {
        printf("  %s\n", m->description);
    }
    printf("\n");

    char buf[4096];
    ssize_t len = readlink(m->link_path, buf, sizeof(buf) - 1);

    if (len > 0) {
        buf[len] = '\0';
        char *real = realpath(m->link_path, NULL);

        printf("Link: %s\n", m->link_path);
        printf("  -> %s\n", buf);
        if (real) {
            printf("  => %s%s%s\n", color_get(COLOR_GREEN), real, color_get(COLOR_RESET));
            free(real);
        }
    } else {
        printf("Link: %s\n", m->link_path);
        printf("  %s(not configured)%s\n", color_get(COLOR_YELLOW), color_get(COLOR_RESET));
    }

    return 0;
}

int module_action_set(const module_info_t *module, const char *target)
{
    if (!module || !target) {
        return -1;
    }

    module_info_t *m = (module_info_t *)module;
    module_load_metadata(m);

    if (!m->link_path) {
        print_error("Module does not define a link path");
        return -1;
    }

    /* Find target in alternatives */
    alternative_list_t alts = {0};
    if (module_get_alternatives(module, &alts) != 0) {
        print_error("Failed to get alternatives");
        return -1;
    }

    char *target_path = NULL;

    /* Match by name or path */
    for (size_t i = 0; i < alts.count; i++) {
        if (strcmp(alts.items[i].name, target) == 0 ||
            strcmp(alts.items[i].path, target) == 0) {
            target_path = strdup(alts.items[i].path);
            break;
        }
    }

    alternative_list_free(&alts);

    /* If not found in list, try as absolute path */
    if (!target_path) {
        if (target[0] == '/' && is_executable(target)) {
            target_path = strdup(target);
        } else {
            print_error("Alternative '%s' not found", target);
            printf("Use 'switch %s list' to see available alternatives.\n", module->name);
            return 1;
        }
    }

    /* Check permissions */
    char *link_dir = strdup(m->link_path);
    char *last_slash = strrchr(link_dir, '/');
    if (last_slash) {
        *last_slash = '\0';
    }

    if (access(link_dir, W_OK) != 0) {
        print_error("Insufficient permissions to modify %s", m->link_path);
        printf("Try running with sudo.\n");
        free(link_dir);
        free(target_path);
        return 1;
    }
    free(link_dir);

    /* Remove old link */
    unlink(m->link_path);

    /* Create new symlink */
    if (symlink(target_path, m->link_path) != 0) {
        print_error("Failed to create symlink: %s", strerror(errno));
        free(target_path);
        return 1;
    }

    print_success("Setting %s to %s\n", module->name, target);
    printf("  %s -> %s\n", m->link_path, target_path);

    free(target_path);
    return 0;
}

int module_action_help(const module_info_t *module)
{
    if (!module) {
        return -1;
    }

    module_info_t *m = (module_info_t *)module;
    module_load_metadata(m);

    printf("Module: %s%s%s", color_get(COLOR_BOLD), module->name, color_get(COLOR_RESET));
    if (m->category) {
        printf(" (%s)", m->category);
    }
    printf("\n");

    if (m->description) {
        printf("\n%s\n", m->description);
    }

    printf("\nUsage: switch %s <action> [arguments]\n", module->name);
    printf("\nActions:\n");
    printf("  list              List available alternatives\n");
    printf("  show              Show current configuration\n");
    printf("  set <target>      Set the alternative\n");
    printf("  help              Show this help\n");

    if (m->link_path) {
        printf("\nManaged link: %s\n", m->link_path);
    }

    if (m->extra_links) {
        printf("Extra links: %s\n", m->extra_links);
    }

    return 0;
}
