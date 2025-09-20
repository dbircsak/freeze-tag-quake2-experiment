//
// freeze.h - Freeze Tag Game Mode Header
//

#ifndef FREEZE_H
#define FREEZE_H

// Weapon bit flags for start_weapon cvar
#define	_shotgun		0x00000001	// 1
#define	_supershotgun	0x00000002	// 2
#define	_machinegun		0x00000004	// 4
#define	_chaingun		0x00000008	// 8
#define	_grenadelauncher 0x00000010	// 16
#define	_rocketlauncher	0x00000020	// 32
#define	_hyperblaster	0x00000040	// 64
#define	_railgun		0x00000080	// 128

// Grappling hook states
#define HOOK_OFF		0x00000000
#define HOOK_ON			0x00000001
#define HOOK_SHRINK		0x00000002
#define HOOK_GROW		0x00000004

// Function declarations
void FT_PlayerWeapon(edict_t* ent);
void FT_HookCommand(edict_t* ent);
void FT_HookThink(edict_t* hook);
void FT_HookTouch(edict_t* hook, edict_t* other, cplane_t* plane, csurface_t* surf);
void FT_FireHook(edict_t* ent);
void FT_DropHook(edict_t* ent);
void FT_UpdateHookChain(edict_t* ent);
void FT_CleanupHook(edict_t* ent);
static void FT_GiveWeapon(const char* weapon_name, edict_t* ent);

// External cvars
extern cvar_t* start_weapon;
extern cvar_t* start_armor;
extern cvar_t* hook_max_length;
extern cvar_t* hook_min_length;
extern cvar_t* hook_speed;
extern cvar_t* hook_pull_speed;
extern cvar_t* hook_wall_only;

#endif // FREEZE_H
