#!/bin/bash
# switch module: python
# Copyright (C) 2026 AnmiTaliDev <anmitali198@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# ============================================================
# Module Metadata
# ============================================================

MODULE_NAME="python"
MODULE_CATEGORY="development"
MODULE_DESCRIPTION="Manage Python interpreter"
MODULE_LINK="/usr/bin/python"

# ============================================================
# Search function
# ============================================================

find_alternatives() {
    # Find python3.x executables
    for py in /usr/bin/python3.[0-9]*; do
        if [[ -x "$py" && ! -L "$py" ]]; then
            local name=$(basename "$py")
            local version="${name#python}"
            # Convert 3.11 -> 311 for priority
            local priority=$(echo "$version" | tr -d '.')
            echo "$py|$name|$priority"
        fi
    done

    # python3 if it's a real binary
    if [[ -x "/usr/bin/python3" && ! -L "/usr/bin/python3" ]]; then
        echo "/usr/bin/python3|python3|300"
    fi
}
