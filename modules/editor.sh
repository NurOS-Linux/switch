#!/bin/bash
# switch module: editor
# Copyright (C) 2026 AnmiTaliDev <anmitali198@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# ============================================================
# Module Metadata
# ============================================================

# Module name
MODULE_NAME="editor"

# Module category (system, development, desktop, etc.)
MODULE_CATEGORY="system"

# Short description
MODULE_DESCRIPTION="Manage default text editor"

# Path to the managed symlink
MODULE_LINK="/usr/bin/editor"

# ============================================================
# Search function - finds available alternatives
# Output format: one alternative per line
#   path|name|priority
# ============================================================

find_alternatives() {
    local candidates=(
        "/usr/bin/nvim"
        "/usr/bin/vim"
        "/usr/bin/nano"
        "/usr/bin/emacs"
        "/usr/bin/vi"
        "/usr/bin/micro"
        "/usr/bin/helix"
        "/usr/bin/kate"
        "/usr/bin/gedit"
        "/usr/bin/code"
    )

    for editor in "${candidates[@]}"; do
        if [[ -x "$editor" ]]; then
            local name=$(basename "$editor")
            local priority=10

            case "$name" in
                nvim)  priority=80 ;;
                vim)   priority=70 ;;
                nano)  priority=60 ;;
                emacs) priority=50 ;;
                micro) priority=45 ;;
                helix) priority=40 ;;
                vi)    priority=30 ;;
                code)  priority=20 ;;
                kate)  priority=15 ;;
                gedit) priority=15 ;;
            esac

            echo "$editor|$name|$priority"
        fi
    done
}
