/*
 * switch - alternatives management tool for NurOS
 * Copyright (C) 2026 AnmiTaliDev <anmitali198@gmail.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "module.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

static void print_version(void)
{
    printf("switch %s\n", SWITCH_VERSION);
    printf("Copyright (C) 2026 AnmiTaliDev\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n\n");
    printf("Part of the NurOS project.\n");
    printf("Repository: https://github.com/NurOS-Linux/switch\n");
}

static void print_usage(const char *progname)
{
    printf("Usage: %s [OPTIONS] <module> <action> [arguments...]\n", progname);
    printf("\n");
    printf("Alternatives management tool for NurOS.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -l, --list-modules    List all available modules\n");
    printf("  -h, --help            Show this help message\n");
    printf("  -V, --version         Show version information\n");
    printf("  --no-color            Disable colored output\n");
    printf("\n");
    printf("Module actions:\n");
    printf("  list                  List available alternatives\n");
    printf("  show                  Show current alternative\n");
    printf("  set <target>          Set alternative to target\n");
    printf("  help                  Show module help\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s --list-modules       List all modules\n", progname);
    printf("  %s editor list          List available editors\n", progname);
    printf("  %s editor show          Show current editor\n", progname);
    printf("  %s editor set vim       Set vim as default editor\n", progname);
    printf("\n");
    printf("Module directories:\n");
    printf("  System: %s\n", SWITCH_MODULES_DIR);
    printf("  User:   ~/%s\n", SWITCH_USER_MODULES_DIR);
}

static struct option long_options[] = {
    {"list-modules", no_argument,       NULL, 'l'},
    {"help",         no_argument,       NULL, 'h'},
    {"version",      no_argument,       NULL, 'V'},
    {"no-color",     no_argument,       NULL, 'C'},
    {NULL,           0,                 NULL, 0}
};

int main(int argc, char *argv[])
{
    int opt;
    bool list_modules = false;
    bool no_color = false;

    /* Parse command line options */
    while ((opt = getopt_long(argc, argv, "lhV", long_options, NULL)) != -1) {
        switch (opt) {
        case 'l':
            list_modules = true;
            break;
        case 'h':
            print_usage(argv[0]);
            return 0;
        case 'V':
            print_version();
            return 0;
        case 'C':
            no_color = true;
            break;
        default:
            fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
            return 1;
        }
    }

    /* Initialize color support */
    color_init();
    if (no_color) {
        color_set_enabled(false);
    }

    /* Initialize configuration */
    switch_config_t config;
    if (config_init(&config) != 0) {
        print_error("Failed to initialize configuration");
        return 1;
    }

    /* Initialize module list */
    module_list_t modules;
    if (module_list_init(&modules) != 0) {
        print_error("Failed to initialize module list");
        config_free(&config);
        return 1;
    }

    /* Scan for modules */
    if (module_scan(&modules, &config) != 0) {
        print_error("Failed to scan modules");
        module_list_free(&modules);
        config_free(&config);
        return 1;
    }

    int ret = 0;

    /* Handle --list-modules */
    if (list_modules) {
        module_print_list(&modules);
        goto cleanup;
    }

    /* Check for module and action arguments */
    if (optind >= argc) {
        print_usage(argv[0]);
        ret = 1;
        goto cleanup;
    }

    const char *module_name = argv[optind];
    const char *action = (optind + 1 < argc) ? argv[optind + 1] : "help";

    /* Find module */
    const module_info_t *module = module_find(&modules, module_name);
    if (!module) {
        print_error("Module '%s' not found", module_name);
        printf("\nUse '%s --list-modules' to see available modules.\n", argv[0]);
        ret = 1;
        goto cleanup;
    }

    /* Execute module action */
    if (strcmp(action, "list") == 0) {
        ret = module_action_list(module);
    } else if (strcmp(action, "show") == 0) {
        ret = module_action_show(module);
    } else if (strcmp(action, "set") == 0) {
        if (optind + 2 >= argc) {
            print_error("Missing argument for 'set' action");
            printf("Usage: %s %s set <target>\n", argv[0], module_name);
            ret = 1;
        } else {
            ret = module_action_set(module, argv[optind + 2]);
        }
    } else if (strcmp(action, "help") == 0) {
        ret = module_action_help(module);
    } else {
        print_error("Unknown action '%s'", action);
        printf("Available actions: list, show, set, help\n");
        ret = 1;
    }

cleanup:
    module_list_free(&modules);
    config_free(&config);
    return ret;
}
