# switch - alternatives management tool for NurOS
# Copyright (C) 2026 AnmiTaliDev <anmitali198@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

project('switch', 'c',
    version: '1.0.0',
    license: 'GPL-3.0-or-later',
    default_options: [
        'c_std=c11',
        'warning_level=2',
        'buildtype=release'
    ],
    meson_version: '>= 0.60.0'
)

# Configuration
conf_data = configuration_data()
conf_data.set_quoted('SWITCH_VERSION', meson.project_version())
conf_data.set_quoted('SWITCH_MODULES_DIR', get_option('prefix') / get_option('datadir') / 'switch' / 'modules')
conf_data.set_quoted('SWITCH_SYSCONFDIR', get_option('prefix') / get_option('sysconfdir') / 'switch')

configure_file(
    output: 'config_generated.h',
    configuration: conf_data
)

# Source files
src_files = files(
    'src/main.c',
    'src/module.c',
    'src/config.c',
    'src/utils.c'
)

# Build executable
switch_exe = executable('switch',
    src_files,
    include_directories: include_directories('src'),
    install: true
)

# Install modules
install_subdir('modules',
    install_dir: get_option('datadir') / 'switch',
    install_mode: ['rwxr-xr-x', 'root', 'root']
)

# Summary
summary({
    'prefix': get_option('prefix'),
    'bindir': get_option('prefix') / get_option('bindir'),
    'modules_dir': get_option('prefix') / get_option('datadir') / 'switch' / 'modules',
}, section: 'Directories')
