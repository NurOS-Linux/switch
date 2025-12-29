# switch

Alternatives management tool for NurOS.

## Overview

**switch** is a modular system for managing symbolic links to select between different versions of programs, similar to Gentoo's eselect.

## Building

```bash
meson setup build --prefix=/usr
meson compile -C build
sudo meson install -C build
```

## Quick Start

```bash
switch --list-modules        # List available modules
switch editor list           # List editor alternatives
switch editor set vim        # Set default editor
```

## Documentation

See [docs/](docs/) for detailed documentation.

## License

GNU GPL-3.0-or-later

Copyright (C) 2026 AnmiTaliDev <anmitali198@gmail.com>

## Links

- Repository: https://github.com/NurOS-Linux/switch
- Part of the [NurOS](https://github.com/NurOS-Linux) project
