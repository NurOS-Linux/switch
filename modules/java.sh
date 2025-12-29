#!/bin/bash
# switch module: java
# Copyright (C) 2026 AnmiTaliDev <anmitali198@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# ============================================================
# Module Metadata
# ============================================================

MODULE_NAME="java"
MODULE_CATEGORY="development"
MODULE_DESCRIPTION="Manage Java Runtime Environment"
MODULE_LINK="/usr/bin/java"

# Additional managed links (optional)
MODULE_EXTRA_LINKS="/usr/bin/javac:/usr/lib/jvm/default"

# ============================================================
# Search function
# ============================================================

find_alternatives() {
    local jvm_dirs=(
        "/usr/lib/jvm"
        "/opt/java"
        "/opt/jdk"
    )

    for dir in "${jvm_dirs[@]}"; do
        [[ ! -d "$dir" ]] && continue

        for jvm in "$dir"/*/; do
            jvm="${jvm%/}"
            local name=$(basename "$jvm")

            [[ "$name" == "default" ]] && continue

            if [[ -x "$jvm/bin/java" ]]; then
                local priority=10
                local java_bin="$jvm/bin/java"

                # Extract version number for priority
                local version=$("$java_bin" -version 2>&1 | head -1 | grep -oP '"\K[^"]+' | cut -d. -f1)
                [[ "$version" =~ ^[0-9]+$ ]] && priority=$((version * 10))

                echo "$java_bin|$name|$priority"
            fi
        done
    done
}
