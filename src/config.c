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

#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>

const char *config_get_home(void)
{
    const char *home = getenv("HOME");
    if (home && home[0] != '\0') {
        return home;
    }

    struct passwd *pw = getpwuid(getuid());
    if (pw && pw->pw_dir) {
        return pw->pw_dir;
    }

    return NULL;
}

int config_init(switch_config_t *config)
{
    if (!config) {
        return -1;
    }

    memset(config, 0, sizeof(*config));

    /* System modules directory */
    config->system_modules_dir = strdup(SWITCH_MODULES_DIR);
    if (!config->system_modules_dir) {
        return -1;
    }

    /* User modules directory */
    const char *home = config_get_home();
    if (home) {
        size_t len = strlen(home) + 1 + strlen(SWITCH_USER_MODULES_DIR) + 1;
        config->user_modules_dir = malloc(len);
        if (config->user_modules_dir) {
            snprintf(config->user_modules_dir, len, "%s/%s", home, SWITCH_USER_MODULES_DIR);
        }
    }

    /* Config directory */
    config->config_dir = strdup(SWITCH_SYSCONFDIR);
    if (!config->config_dir) {
        config_free(config);
        return -1;
    }

    /* Color enabled by default */
    config->color_enabled = true;

    return 0;
}

void config_free(switch_config_t *config)
{
    if (!config) {
        return;
    }

    free(config->system_modules_dir);
    free(config->user_modules_dir);
    free(config->config_dir);

    memset(config, 0, sizeof(*config));
}
