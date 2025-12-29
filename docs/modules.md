# Writing Modules

Modules are shell scripts that define metadata and a search function.

## Module Structure

```bash
#!/bin/bash
# switch module: <name>
# Copyright (C) 2026 Your Name
# SPDX-License-Identifier: GPL-3.0-or-later

# ============================================================
# Module Metadata
# ============================================================

MODULE_NAME="mymodule"
MODULE_CATEGORY="system"           # system, development, desktop
MODULE_DESCRIPTION="Short description"
MODULE_LINK="/path/to/symlink"
MODULE_EXTRA_LINKS="/other/link"   # Optional, colon-separated

# ============================================================
# Search function
# Output: path|name|priority (one per line)
# ============================================================

find_alternatives() {
    # Find and output alternatives
    echo "/usr/bin/prog1|prog1|50"
    echo "/usr/bin/prog2|prog2|40"
}
```

## Required Fields

| Field | Description |
|-------|-------------|
| `MODULE_NAME` | Module identifier |
| `MODULE_DESCRIPTION` | Short description |
| `MODULE_LINK` | Path to managed symlink |
| `find_alternatives()` | Function that outputs alternatives |

## Optional Fields

| Field | Description |
|-------|-------------|
| `MODULE_CATEGORY` | Category (system, development, desktop) |
| `MODULE_EXTRA_LINKS` | Additional symlinks (colon-separated) |

## Output Format

The `find_alternatives()` function must output lines in format:

```
<path>|<name>|<priority>
```

- `path` — Full path to the binary
- `name` — Display name
- `priority` — Integer, higher = preferred

## Example: Custom Module

```bash
#!/bin/bash
# switch module: browser
# SPDX-License-Identifier: GPL-3.0-or-later

MODULE_NAME="browser"
MODULE_CATEGORY="desktop"
MODULE_DESCRIPTION="Manage default web browser"
MODULE_LINK="/usr/bin/x-www-browser"

find_alternatives() {
    local browsers=(
        "/usr/bin/firefox|firefox|80"
        "/usr/bin/chromium|chromium|70"
        "/usr/bin/brave|brave|60"
    )

    for entry in "${browsers[@]}"; do
        local path="${entry%%|*}"
        [[ -x "$path" ]] && echo "$entry"
    done
}
```

## Installation

```bash
# User module
cp browser.sh ~/.local/share/switch/modules/
chmod +x ~/.local/share/switch/modules/browser.sh

# System module (requires root)
sudo cp browser.sh /usr/share/switch/modules/
sudo chmod +x /usr/share/switch/modules/browser.sh
```
