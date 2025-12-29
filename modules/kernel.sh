#!/bin/bash
# switch module: kernel
# Copyright (C) 2026 AnmiTaliDev <anmitali198@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

# ============================================================
# Module Metadata
# ============================================================

MODULE_NAME="kernel"
MODULE_CATEGORY="system"
MODULE_DESCRIPTION="Manage default boot kernel"
MODULE_LINK="/boot/vmlinuz"

# Additional managed links
MODULE_EXTRA_LINKS="/boot/initramfs.img"

# ============================================================
# Search function
# ============================================================

find_alternatives() {
    local boot_dir="/boot"

    for kernel in "$boot_dir"/vmlinuz-*; do
        [[ ! -f "$kernel" ]] && continue

        local version="${kernel#$boot_dir/vmlinuz-}"

        # Calculate priority from kernel version
        local major=$(echo "$version" | grep -oP '^\d+' 2>/dev/null || echo "0")
        local minor=$(echo "$version" | grep -oP '^\d+\.\d+' 2>/dev/null | cut -d. -f2 || echo "0")
        local priority=$((major * 100 + minor))

        echo "$kernel|$version|$priority"
    done
}
