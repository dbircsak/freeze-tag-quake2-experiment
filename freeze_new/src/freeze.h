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

// Function declarations
void FT_PlayerWeapon(edict_t* ent);
static void FT_GiveWeapon(const char* weapon_name, edict_t* ent);

// External cvars
extern cvar_t* start_weapon;
extern cvar_t* start_armor;

#endif // FREEZE_H
