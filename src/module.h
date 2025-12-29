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

#ifndef SWITCH_MODULE_H
#define SWITCH_MODULE_H

#include "config.h"
#include <stdbool.h>
#include <stddef.h>

/* Maximum number of modules/alternatives */
#define MAX_MODULES 256
#define MAX_ALTERNATIVES 128

/* Alternative entry */
typedef struct {
    char *path;      /* Full path to the binary */
    char *name;      /* Display name */
    int priority;    /* Priority (higher = preferred) */
} alternative_t;

/* Alternatives list */
typedef struct {
    alternative_t *items;
    size_t count;
    size_t capacity;
} alternative_list_t;

/* Module information structure */
typedef struct {
    char *name;         /* Module name (without .sh extension) */
    char *path;         /* Full path to module script */
    char *description;  /* Module description */
    char *category;     /* Module category */
    char *link_path;    /* Path to the managed symlink */
    char *extra_links;  /* Additional managed links (colon-separated) */
    bool is_user;       /* True if from user directory */
} module_info_t;

/* Module list structure */
typedef struct {
    module_info_t *modules;
    size_t count;
    size_t capacity;
} module_list_t;

/* Initialize module list */
int module_list_init(module_list_t *list);

/* Free module list */
void module_list_free(module_list_t *list);

/* Scan directories for modules */
int module_scan(module_list_t *list, const switch_config_t *config);

/* Find module by name */
const module_info_t *module_find(const module_list_t *list, const char *name);

/* Load module metadata (description, link_path, etc.) */
int module_load_metadata(module_info_t *module);

/* Get alternatives from module */
int module_get_alternatives(const module_info_t *module, alternative_list_t *list);

/* Free alternatives list */
void alternative_list_free(alternative_list_t *list);

/* Print list of all modules */
void module_print_list(const module_list_t *list);

/* Actions */
int module_action_list(const module_info_t *module);
int module_action_show(const module_info_t *module);
int module_action_set(const module_info_t *module, const char *target);
int module_action_help(const module_info_t *module);

#endif /* SWITCH_MODULE_H */
