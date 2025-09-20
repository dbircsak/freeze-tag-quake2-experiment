# Agent Instructions

## Project Overview
Quake 2 freeze tag game mode experiment - comprehensive modification of original Q2 source code to implement 4-team freeze tag gameplay with grappling hooks and advanced team mechanics.

## Project Structure
- `q2_orig/src/` - Original Quake 2 source code (76 files)
- `freeze_orig/src/` - Original freeze tag implementation (78+ files)
- `freeze_orig/` - Also contains config files, launch scripts, compiled DLL, and assets
- `freeze_new/` - Working directory for new modifications and improvements

## Key Modifications Made
- **New files**: `freeze.h` (39 lines), `freeze.c` (2470 lines of freeze tag logic)
- **New config files**: `freeze.cfg`, `freeze.bat`, `freeze.ini`, `pak2.pak`
- **Modified core files**: `g_local.h`, `p_client.c`, `g_main.c`, `g_spawn.c`, `p_hud.c`, `p_view.c`, `p_weapon.c`, `g_save.c`, `g_cmds.c`, `g_chase.c`, `g_items.c`, `g_combat.c`, `q_shared.h`
- **Features added**: 4-team system, freeze/thaw mechanics, grappling hooks, map voting, ready system, flood protection, team switching menus

## freeze_new Enhancements
- **New UI System**: `ui_system.h`, `ui_system.c` - Modern MOTD and menu system
- **MOTD Support**: Configurable welcome messages loaded from `motd.txt`
- **Interactive Menus**: Navigate with inventory keys (prev/next/use)
- **Input Integration**: Intercepts inventory commands when UI is active
- **Memory Management**: Proper cleanup and timeout support
- **Command Added**: `menu` command to open main menu

## Common Commands
- Build: Open `freeze_orig/src/game.sln` in Visual Studio and build solution
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
- **IMPORTANT**: This is a Windows environment - use Windows commands only (dir, findstr, powershell) not Linux commands (ls, grep, head, tail, etc.)
- **WORK RESTRICTION**: Only modify AGENTS.md and files within freeze_new/ directory - do NOT modify q2_orig/ or freeze_orig/ files

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
