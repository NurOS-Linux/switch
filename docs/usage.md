# Usage

## Command Line

```
switch [OPTIONS] <module> <action> [arguments...]
```

### Options

| Option | Description |
|--------|-------------|
| `-l, --list-modules` | List all available modules |
| `-h, --help` | Show help message |
| `-V, --version` | Show version |
| `--no-color` | Disable colored output |

### Actions

| Action | Description |
|--------|-------------|
| `list` | List available alternatives |
| `show` | Show current configuration |
| `set <target>` | Set the alternative |
| `help` | Show module help |

## Examples

```bash
# List all modules
switch --list-modules

# Editor management
switch editor list
switch editor show
switch editor set vim

# Java management
switch java list
switch java set java-17-openjdk

# Python management
switch python list
switch python set python3.11

# Kernel management (requires root)
sudo switch kernel list
sudo switch kernel set 6.8.0
```

## Module Directories

- System: `/usr/share/switch/modules/`
- User: `~/.local/share/switch/modules/`

User modules take precedence over system modules.

## Environment Variables

| Variable | Description |
|----------|-------------|
| `NO_COLOR` | Disable colored output (standard) |
| `SWITCH_NO_COLOR` | Disable colored output (switch-specific) |
