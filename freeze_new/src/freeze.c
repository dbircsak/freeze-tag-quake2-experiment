//
// freeze.c - Freeze Tag Game Mode Implementation
//

#include "g_local.h"
#include "freeze.h"

// External function declarations
void P_ProjectSource(gclient_t *client, vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);

// CVars for freeze tag configuration
cvar_t* start_weapon;
cvar_t* start_armor;

// Grappling hook CVars
cvar_t* hook_max_length;
cvar_t* hook_min_length;
cvar_t* hook_speed;
cvar_t* hook_pull_speed;
cvar_t* hook_wall_only;

// Global team information array
team_info_t teams[MAX_TEAMS];

//
// FT_InitTeams
// Initialize the team system with default values
//
void FT_InitTeams(void)
{
	teams[TEAM_RED].name = "Red";
	teams[TEAM_RED].skin = "ctf_r";
	teams[TEAM_RED].score = 0;
	teams[TEAM_RED].players = 0;
	teams[TEAM_RED].frozen_count = 0;
	teams[TEAM_RED].eliminated = false;

	teams[TEAM_BLUE].name = "Blue";
	teams[TEAM_BLUE].skin = "ctf_b";
	teams[TEAM_BLUE].score = 0;
	teams[TEAM_BLUE].players = 0;
	teams[TEAM_BLUE].frozen_count = 0;
	teams[TEAM_BLUE].eliminated = false;

	teams[TEAM_GREEN].name = "Green";
	teams[TEAM_GREEN].skin = "ctf_g";
	teams[TEAM_GREEN].score = 0;
	teams[TEAM_GREEN].players = 0;
	teams[TEAM_GREEN].frozen_count = 0;
	teams[TEAM_GREEN].eliminated = false;

	teams[TEAM_YELLOW].name = "Yellow";
	teams[TEAM_YELLOW].skin = "ctf_y";
	teams[TEAM_YELLOW].score = 0;
	teams[TEAM_YELLOW].players = 0;
	teams[TEAM_YELLOW].frozen_count = 0;
	teams[TEAM_YELLOW].eliminated = false;
}

//
// FT_UpdateTeamCounts
// Update player counts for all teams
//
void FT_UpdateTeamCounts(void)
{
	int i, j;
	
	// Reset all team counts
	for (i = 0; i < MAX_TEAMS; i++)
	{
		teams[i].players = 0;
		teams[i].frozen_count = 0;
	}
	
	// Count players on each team
	for (j = 1; j <= maxclients->value; j++)
	{
		edict_t* ent = g_edicts + j;
		if (!ent->inuse || !ent->client)
			continue;
		
		team_t team = FT_GetPlayerTeam(ent);
		if (team >= 0 && team < MAX_TEAMS)
		{
			teams[team].players++;
			if (ent->client->resp.frozen)
				teams[team].frozen_count++;
		}
	}
}

//
// FT_AutoAssignTeam
// Find the team with the fewest players for balanced assignment
//
team_t FT_AutoAssignTeam(void)
{
	int min_players = 999;
	team_t best_team = TEAM_RED;
	int i;
	
	FT_UpdateTeamCounts();
	
	// Find team with fewest players
	for (i = 0; i < MAX_TEAMS; i++)
	{
		if (teams[i].players < min_players)
		{
			min_players = teams[i].players;
			best_team = (team_t)i;
		}
	}
	
	return best_team;
}

//
// FT_JoinTeam
// Assign a player to a specific team
//
void FT_JoinTeam(edict_t* ent, team_t team)
{
	if (!ent || !ent->client || team < 0 || team >= MAX_TEAMS)
		return;
	
	ent->client->resp.team = team;
	ent->client->resp.frozen = false;
	
	// Set team skin in userinfo string
	char userinfo[MAX_INFO_STRING];
	char* info_skin;
	char new_skin[64];
	
	strcpy(userinfo, ent->client->pers.userinfo);
	info_skin = Info_ValueForKey(userinfo, "skin");
	
	// Extract model prefix (e.g., "male/" from "male/grunt")
	char* slash_pos = strchr(info_skin, '/');
	if (slash_pos && (slash_pos - info_skin) < 32)
	{
		// Use model prefix + team skin
		int prefix_len = slash_pos - info_skin + 1;
		strncpy(new_skin, info_skin, prefix_len);
		new_skin[prefix_len] = '\0';
		strcat(new_skin, teams[team].skin + 4); // Remove "ctf_" prefix
	}
	else
	{
		// Fallback to default team skin
		strcpy(new_skin, teams[team].skin);
	}
	
	Info_SetValueForKey(userinfo, "skin", new_skin);
	strcpy(ent->client->pers.userinfo, userinfo);
	ClientUserinfoChanged(ent, userinfo);
	
	// Update team counts
	FT_UpdateTeamCounts();
	
	// Notify player
	gi.cprintf(ent, PRINT_HIGH, "You joined the %s team.\n", teams[team].name);
}

//
// FT_GetPlayerTeam
// Get the team of a player
//
team_t FT_GetPlayerTeam(edict_t* ent)
{
	if (!ent || !ent->client)
		return TEAM_NONE;
	
	return (team_t)ent->client->resp.team;
}

//
// FT_OnSameTeam
// Check if two players are on the same team
//
qboolean FT_OnSameTeam(edict_t* ent1, edict_t* ent2)
{
	if (!ent1 || !ent2 || !ent1->client || !ent2->client)
		return false;
	
	team_t team1 = FT_GetPlayerTeam(ent1);
	team_t team2 = FT_GetPlayerTeam(ent2);
	
	return (team1 != TEAM_NONE && team1 == team2);
}

//
// FT_GetTeamSkin
// Get the skin name for a team
//
char* FT_GetTeamSkin(team_t team)
{
	if (team >= 0 && team < MAX_TEAMS)
		return teams[team].skin;
	
	return "male/grunt";  // Default skin
}

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
	
	// Initialize grappling hook cvars
	hook_max_length = gi.cvar("hook_max_length", "1000", 0);
	hook_min_length = gi.cvar("hook_min_length", "40", 0);
	hook_speed = gi.cvar("hook_speed", "1200", 0);
	hook_pull_speed = gi.cvar("hook_pull_speed", "80", 0);
	hook_wall_only = gi.cvar("hook_wall_only", "0", 0);
}

//
// FT_FireHook
// Launch a grappling hook projectile
//
void FT_FireHook(edict_t* ent)
{
	vec3_t forward, right, start, offset;
	edict_t* hook;
	
	if (!ent->client || ent->health <= 0)
		return;
		
	// Drop existing hook first
	FT_DropHook(ent);
	
	// Calculate launch position
	AngleVectors(ent->client->v_angle, forward, right, NULL);
	VectorSet(offset, 8, 8, ent->viewheight - 8);
	P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);
	
	// Create hook projectile
	hook = G_Spawn();
	hook->classname = "hook";
	hook->owner = ent;
	hook->movetype = MOVETYPE_FLY;
	hook->solid = SOLID_BBOX;
	hook->clipmask = MASK_SHOT;
	hook->s.modelindex = gi.modelindex("models/objects/debris1/tris.md2"); // Use available model
	hook->s.sound = 0; // No sound for now
	
	VectorCopy(start, hook->s.origin);
	VectorScale(forward, hook_speed->value, hook->velocity);
	VectorCopy(forward, hook->s.angles);
	vectoangles(hook->velocity, hook->s.angles);
	
	hook->nextthink = level.time + FRAMETIME;
	hook->think = FT_HookThink;
	hook->touch = FT_HookTouch;
	
	VectorClear(hook->mins);
	VectorClear(hook->maxs);
	
	gi.linkentity(hook);
	gi.sound(ent, CHAN_WEAPON, gi.soundindex("weapons/blastf1a.wav"), 1, ATTN_NORM, 0);
	
	// Store hook reference in client
	ent->client->hook = hook;
	ent->client->hookstate = HOOK_ON;
}

//
// FT_DropHook
// Release and remove the grappling hook
//
void FT_DropHook(edict_t* ent)
{
	if (!ent->client || !ent->client->hook)
		return;
		
	if (ent->client->hook && ent->client->hook->inuse)
	{
		G_FreeEdict(ent->client->hook);
	}
	
	ent->client->hook = NULL;
	ent->client->hookstate = HOOK_OFF;
	VectorClear(ent->client->ps.pmove.velocity);
}

//
// FT_HookTouch
// Handle hook collision with world/players
//
void FT_HookTouch(edict_t* hook, edict_t* other, cplane_t* plane, csurface_t* surf)
{
	edict_t* owner;
	
	if (!hook->owner)
	{
		G_FreeEdict(hook);
		return;
	}
	
	owner = hook->owner;
	
	// Don't hook the owner
	if (other == owner)
		return;
		
	// Check wall-only restriction
	if (hook_wall_only->value && other->client)
		return;
		
	// Attach hook
	VectorClear(hook->velocity);
	hook->movetype = MOVETYPE_NONE;
	hook->solid = SOLID_NOT;
	hook->touch = NULL;
	
	// Store what we're hooked to
	hook->enemy = other;
	
	gi.sound(owner, CHAN_WEAPON, gi.soundindex("weapons/machgf1b.wav"), 1, ATTN_NORM, 0);
}

//
// FT_HookThink
// Update hook physics and behavior
//
void FT_HookThink(edict_t* hook)
{
	edict_t* owner;
	vec3_t hookdir;
	float hookdist;
	
	if (!hook->owner || !hook->owner->client || !hook->owner->inuse)
	{
		G_FreeEdict(hook);
		return;
	}
	
	owner = hook->owner;
	
	// Check if hook has traveled too far
	VectorSubtract(hook->s.origin, owner->s.origin, hookdir);
	hookdist = VectorLength(hookdir);
	
	if (hookdist > hook_max_length->value)
	{
		FT_DropHook(owner);
		return;
	}
	
	// If hooked to something, pull the player
	if (hook->enemy && hook->enemy->inuse)
	{
		vec3_t chainvec;
		float chainlen;
		float desired_len;
		float force;
		
		// Calculate chain vector and length
		VectorSubtract(hook->s.origin, owner->s.origin, chainvec);
		chainlen = VectorLength(chainvec);
		VectorNormalize(chainvec);
		
		// Set desired chain length based on grow/shrink state
		desired_len = hook_min_length->value;
		if (owner->client->hookstate & HOOK_SHRINK)
			desired_len = hook_min_length->value * 0.5; // Pull closer
		else if (owner->client->hookstate & HOOK_GROW)
			desired_len = hook_max_length->value * 0.8; // Allow more distance
		
		// Apply pull force if chain is longer than desired
		if (chainlen > desired_len)
		{
			force = (chainlen - desired_len) * 5.0; // Same multiplier as original
			VectorMA(owner->velocity, force, chainvec, owner->velocity);
			owner->client->ps.pmove.pm_flags |= PMF_NO_PREDICTION;
		}
	}
	
	// Update hook chain visual
	FT_UpdateHookChain(owner);
	
	hook->nextthink = level.time + FRAMETIME;
}

//
// FT_UpdateHookChain
// Render the chain/cable between player and hook
//
void FT_UpdateHookChain(edict_t* ent)
{
	edict_t* hook;
	
	if (!ent->client || !ent->client->hook)
		return;
		
	hook = ent->client->hook;
	
	// Use temp entity to draw the chain (simplified - just a beam)
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_BFG_LASER);
	gi.WritePosition(hook->s.origin);
	gi.WritePosition(ent->s.origin);
	gi.multicast(ent->s.origin, MULTICAST_PVS);
}

//
// FT_HookCommand
// Handle +hook/-hook commands
//
void FT_HookCommand(edict_t* ent)
{
	char* cmd;
	
	if (!ent->client || ent->health <= 0)
		return;
		
	cmd = gi.argv(1);
	
	if (Q_stricmp(cmd, "fire") == 0)
	{
		FT_FireHook(ent);
	}
	else if (Q_stricmp(cmd, "drop") == 0)
	{
		FT_DropHook(ent);
	}
	else if (Q_stricmp(cmd, "shrink") == 0)
	{
		if (ent->client->hook)
			ent->client->hookstate |= HOOK_SHRINK;
	}
	else if (Q_stricmp(cmd, "grow") == 0)
	{
		if (ent->client->hook)
			ent->client->hookstate |= HOOK_GROW;
	}
	else if (Q_stricmp(cmd, "normal") == 0)
	{
		if (ent->client->hook)
			ent->client->hookstate &= ~(HOOK_SHRINK | HOOK_GROW);
	}
}

//
// FT_CleanupHook
// Clean up hook when player dies or disconnects
//
void FT_CleanupHook(edict_t* ent)
{
	if (!ent->client)
		return;
		
	// Drop our hook
	FT_DropHook(ent);
	
	// Also clean up any hooks that might be attached to us
	edict_t* hook = &g_edicts[1];
	for (int i = 1; i < globals.num_edicts; i++, hook++)
	{
		if (!hook->inuse)
			continue;
			
		if (hook->enemy == ent || hook->owner == ent)
		{
			if (hook->owner && hook->owner->client)
				FT_DropHook(hook->owner);
		}
	}
}
