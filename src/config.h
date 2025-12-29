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

#ifndef SWITCH_CONFIG_H
#define SWITCH_CONFIG_H

#include <stdbool.h>

/* Try to include generated config first */
#if __has_include("config_generated.h")
#include "config_generated.h"
#endif

/* Default paths - can be overridden by meson build */
#ifndef SWITCH_VERSION
#define SWITCH_VERSION "1.0.0"
#endif

#ifndef SWITCH_MODULES_DIR
#define SWITCH_MODULES_DIR "/usr/share/switch/modules"
#endif

#ifndef SWITCH_SYSCONFDIR
#define SWITCH_SYSCONFDIR "/etc/switch"
#endif

/* User modules directory (relative to HOME) */
#define SWITCH_USER_MODULES_DIR ".local/share/switch/modules"

/* Configuration structure */
typedef struct {
    char *system_modules_dir;
    char *user_modules_dir;
    char *config_dir;
    bool color_enabled;
} switch_config_t;

/* Initialize configuration */
int config_init(switch_config_t *config);

/* Free configuration resources */
void config_free(switch_config_t *config);

/* Get user home directory */
const char *config_get_home(void);

#endif /* SWITCH_CONFIG_H */
