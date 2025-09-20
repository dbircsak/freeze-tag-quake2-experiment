# Agent Instructions

## Project Overview
Quake 2 freeze tag game mode experiment - comprehensive modification of original Q2 source code to implement 4-team freeze tag gameplay with grappling hooks and advanced team mechanics.

## Project Structure
- `q2_orig/src/` - Original Quake 2 source code (76 files)
- `freeze_orig/src/` - Original freeze tag implementation (78+ files)
- `freeze_orig/` - Also contains config files, launch scripts, compiled DLL, and assets
- `freeze_new/` - Working directory for new modifications and improvements

## Key Modifications Made
- **New files**: `freeze.h`, `freeze.c` (~1000 lines of freeze tag logic)
- **Modified core files**: `g_local.h`, `g_cmds.c`, `g_combat.c`, `p_client.c`, `p_hud.c`, `g_main.c`
- **Features added**: 4-team system, freeze/thaw mechanics, grappling hooks, map voting, ready system

### Recent Implementations (freeze_new)
- **Start weapon system**: Configurable starting weapons for newly spawned players
  - Uses bitwise flags in `start_weapon` cvar (1=shotgun, 2=super shotgun, 4=machinegun, etc.)
  - Automatic ammo allocation for granted weapons
  - Default config: start_weapon 35 (shotgun + super shotgun + rocket launcher)
  - Includes start_armor support via `start_armor` cvar

- **Grappling hook system**: Built-in movement ability for all players
  - Hook projectile with physics simulation and collision detection
  - Dynamic pull force based on chain length (force = distance * 5.0 multiplier)
  - Grow/shrink modes for variable chain length control
  - Visual chain rendering using temp entities
  - Bound to MOUSE2 (+hook/-hook) with console commands for advanced control
  - Configurable via CVars: hook_max_length, hook_min_length, hook_speed, hook_pull_speed, hook_wall_only
  - Automatic cleanup on player death/disconnect

## Common Commands
- Build: Open `freeze_orig/src/game.sln` or `freeze_new/src/game.sln` in Visual Studio and build solution
- Clean: Use Visual Studio "Clean Solution" or delete build artifacts
- Launch: Use `freeze.bat` or `freeze - dedicated server.bat`
- Test: Run with custom maps that support freeze tag mechanics

## Code Style
- Follow existing Quake 2 C style conventions
- Use tabs for indentation (existing codebase style)
- Prefix freeze tag functions with descriptive names (`freeze`, `thaw`, `hook`, etc.)
- Maintain compatibility with original Q2 architecture

## Development Notes
- This is a complete multiplayer conversion, not just a mod
- Original single-player/monster code remains unchanged
- Uses CTF skins for team identification (ctf_r, ctf_b, ctf_g, ctf_y)
- Extensive configuration system with CVars and map-specific settings
- Windows build environment with Visual Studio integration
- Compiled DLL suggests this was a working implementation

## Team System
- 4 teams: Red, Blue, Green, Yellow
- Team damage prevention
- Team-based scoring and win conditions
- Player can switch teams via menu system

## Freeze Tag Mechanics
- Players freeze instead of dying
- Teammates can thaw frozen players
- Frozen players can spectate/chase camera
- Break system - frozen players shatter after timer
- Hook system for advanced movement on compatible maps
