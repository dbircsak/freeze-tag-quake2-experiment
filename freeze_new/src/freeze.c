//
// freeze.c - Freeze Tag Game Mode Implementation
//

#include "g_local.h"
#include "freeze.h"

// CVars for freeze tag configuration
cvar_t* start_weapon;
cvar_t* start_armor;

//
// FT_GiveWeapon
// Helper function to give a weapon and its ammo to a player
//
static void FT_GiveWeapon(const char* weapon_name, edict_t* ent)
{
	gitem_t* item;
	gitem_t* ammo;
	int index;

	item = FindItem(weapon_name);
	if (item)
	{
		// Give the weapon
		index = ITEM_INDEX(item);
		ent->client->pers.inventory[index] = 1;

		// Give ammo if the weapon uses it
		ammo = FindItem(item->ammo);
		if (ammo)
		{
			index = ITEM_INDEX(ammo);
			ent->client->pers.inventory[index] = ammo->quantity;
		}
		
		// Set as current weapon
		ent->client->newweapon = item;
	}
}

//
// FT_PlayerWeapon
// Give starting weapons and armor to newly spawned player based on cvars
//
void FT_PlayerWeapon(edict_t* ent)
{
	gitem_t* item;

	if (!ent || !ent->client)
		return;

	// Always give blaster
	item = FindItem("blaster");
	if (item)
	{
		ent->client->pers.inventory[ITEM_INDEX(item)] = 1;
		ent->client->newweapon = item;
	}

	// Give starting armor if configured
	if (start_armor->value)
	{
		int armor_index = ITEM_INDEX(FindItem("jacket armor"));
		ent->client->pers.inventory[armor_index] = (int)(start_armor->value / 2) * 2;
	}

	// Give starting weapons based on start_weapon cvar bit flags
	if (start_weapon->value)
	{
		int weapon_flags = (int)start_weapon->value;

		if (weapon_flags & _shotgun)
			FT_GiveWeapon("shotgun", ent);
		
		if (weapon_flags & _supershotgun)
			FT_GiveWeapon("super shotgun", ent);
		
		if (weapon_flags & _machinegun)
			FT_GiveWeapon("machinegun", ent);
		
		if (weapon_flags & _chaingun)
			FT_GiveWeapon("chaingun", ent);
		
		if (weapon_flags & _grenadelauncher)
			FT_GiveWeapon("grenade launcher", ent);
		
		if (weapon_flags & _rocketlauncher)
			FT_GiveWeapon("rocket launcher", ent);
		
		if (weapon_flags & _hyperblaster)
			FT_GiveWeapon("hyperblaster", ent);
		
		if (weapon_flags & _railgun)
			FT_GiveWeapon("railgun", ent);
	}

	// Change to the new weapon
	ChangeWeapon(ent);
}

//
// FT_InitGame
// Initialize freeze tag cvars
//
void FT_InitGame(void)
{
	start_weapon = gi.cvar("start_weapon", "0", 0);
	start_armor = gi.cvar("start_armor", "0", 0);
}
