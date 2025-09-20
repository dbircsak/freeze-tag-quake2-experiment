#include "ui_system.h"
#include "g_local.h"

// Global UI system state
static qboolean ui_initialized = false;
static char motd_lines[MAX_UI_LINES][MAX_UI_TEXT];
static int motd_line_count = 0;

// Per-client UI state storage
static client_ui_t client_ui_states[32]; // Support up to 32 clients

// CVars for UI system
cvar_t* ui_motd_file;
cvar_t* ui_motd_timeout;
cvar_t* ui_menu_timeout;

// Helper function to get client UI state
static client_ui_t* UI_GetClientState(edict_t* ent)
{
    if (!ent || !ent->client)
        return NULL;
        
    int client_num = ent - g_edicts - 1;
    if (client_num < 0 || client_num >= 32)
        return NULL;
        
    return &client_ui_states[client_num];
}

void UI_Init(void)
{
    if (ui_initialized)
        return;

    // Register CVars
    ui_motd_file = gi.cvar("ui_motd_file", "motd.txt", CVAR_SERVERINFO);
    ui_motd_timeout = gi.cvar("ui_motd_timeout", "15", 0);
    ui_menu_timeout = gi.cvar("ui_menu_timeout", "30", 0);

    // Load MOTD from file
    UI_LoadMOTD();

    ui_initialized = true;
    gi.dprintf("UI System initialized\n");
}

void UI_Shutdown(void)
{
    if (!ui_initialized)
        return;

    motd_line_count = 0;
    ui_initialized = false;
    gi.dprintf("UI System shutdown\n");
}

void UI_LoadMOTD(void)
{
    // Simple default MOTD - no file loading to avoid complexity
    strcpy(motd_lines[0], "Welcome to Freeze Tag!");
    strcpy(motd_lines[1], "Work as a team!");
    strcpy(motd_lines[2], "Press INVENTORY to continue");
    motd_line_count = 3;
    
    gi.dprintf("Loaded simple MOTD\n");
}

void UI_ShowMOTD(edict_t* ent)
{
    if (!ent || !ent->client)
        return;

    client_ui_t* ui = UI_GetClientState(ent);
    if (!ui)
        return;
    ui_display_t* display = &ui->current_ui;
    
    // Simple MOTD setup
    display->type = UI_TYPE_MOTD;
    display->active = true;
    display->timeout = 15.0f;
    display->display_time = level.time;
    
    ui->ui_enabled = true;
    ui->last_input = level.time;
    
    gi.dprintf("Showing MOTD to %s\n", ent->client->pers.netname);
}

void UI_ShowMenu(edict_t* ent, const char* title)
{
    if (!ent || !ent->client)
        return;

    client_ui_t* ui = UI_GetClientState(ent);
    if (!ui)
        return;
    ui_display_t* display = &ui->current_ui;
    
    // Simple menu setup
    display->type = UI_TYPE_MENU;
    display->active = true;
    display->timeout = 30.0f;
    display->display_time = level.time;
    
    ui->ui_enabled = true;
    ui->last_input = level.time;
    
    gi.dprintf("Showing menu to %s\n", ent->client->pers.netname);
}

void UI_AddItem(ui_display_t* ui, ui_item_type_t type, const char* text, void (*action)(edict_t*))
{
    if (!ui || ui->item_count >= MAX_UI_ITEMS)
        return;
        
    ui_item_t* item = gi.TagMalloc(sizeof(ui_item_t), TAG_GAME);
    item->type = type;
    strcpy(item->text, text);
    item->selectable = (type == UI_ITEM_BUTTON);
    item->action = action;
    item->next = NULL;
    
    // Set color based on type
    switch (type) {
        case UI_ITEM_TEXT:
            item->color = 7; // White
            break;
        case UI_ITEM_BUTTON:
            item->color = 2; // Green
            break;
        case UI_ITEM_SEPARATOR:
            item->color = 8; // Gray
            break;
    }
    
    ui->items[ui->item_count] = item;
    ui->item_count++;
}

void UI_Update(edict_t* ent)
{
    if (!ent || !ent->client)
        return;
        
    client_ui_t* ui = UI_GetClientState(ent);
    if (!ui)
        return;
    
    if (!ui->ui_enabled)
        return;
        
    ui_display_t* display = &ui->current_ui;
    
    // Check timeout
    if (display->timeout > 0 && 
        level.time - display->display_time > display->timeout) {
        UI_Close(ent);
        return;
    }
    
    // Render UI
    UI_Render(ent);
}

void UI_HandleInput(edict_t* ent, int key)
{
    if (!ent || !ent->client)
        return;
        
    client_ui_t* ui = UI_GetClientState(ent);
    if (!ui)
        return;
    
    if (!ui->ui_enabled)
        return;
        
    ui_display_t* display = &ui->current_ui;
    ui->last_input = level.time;
    
    switch (key) {
        case 'n': // Next item
        case 'N':
            do {
                display->selected_item = (display->selected_item + 1) % display->item_count;
            } while (!display->items[display->selected_item]->selectable && 
                     display->item_count > 1);
            break;
            
        case 'p': // Previous item  
        case 'P':
            do {
                display->selected_item = (display->selected_item - 1 + display->item_count) % display->item_count;
            } while (!display->items[display->selected_item]->selectable && 
                     display->item_count > 1);
            break;
            
        case 'u': // Use/Select
        case 'U':
        case '\r':
        case '\n':
            if (display->type == UI_TYPE_MOTD) {
                UI_Close(ent);
            } else if (display->selected_item < display->item_count) {
                ui_item_t* item = display->items[display->selected_item];
                if (item->action) {
                    item->action(ent);
                }
            }
            break;
            
        case 'q': // Quit/Close
        case 'Q':
        case 27: // ESC
            UI_Close(ent);
            break;
    }
}

void UI_Close(edict_t* ent)
{
    if (!ent || !ent->client)
        return;
        
    client_ui_t* ui = UI_GetClientState(ent);
    if (!ui)
        return;
    ui_display_t* display = &ui->current_ui;
    
    // Simple cleanup
    display->active = false;
    ui->ui_enabled = false;
    
    gi.dprintf("Closed UI for %s\n", ent->client->pers.netname);
}

void UI_Render(edict_t* ent)
{
    if (!ent || !ent->client)
        return;
        
    client_ui_t* ui = UI_GetClientState(ent);
    if (!ui)
        return;
    ui_display_t* display = &ui->current_ui;
    
    if (!display->active)
        return;
    
    // Simple layout string - keep it very short for Quake 2 limits
    char layout[512] = "";
    
    if (display->type == UI_TYPE_MOTD) {
        // Simple MOTD display
        Com_sprintf(layout, sizeof(layout),
            "xv 32 yv 32 string2 Welcome "
            "xv 32 yv 64 string \"Press INVENTORY to continue\" ");
    } else {
        // Simple menu
        Com_sprintf(layout, sizeof(layout),
            "xv 32 yv 32 string2 Menu "
            "xv 32 yv 64 string \"1. Join Team\" "
            "xv 32 yv 80 string \"2. Spectate\" "
            "xv 32 yv 96 string \"Press INVENTORY to select\" ");
    }
    
    // Set the layout
    gi.WriteByte(svc_layout);
    gi.WriteString(layout);
    gi.unicast(ent, true);
}

// Menu Action Functions
void UI_Action_JoinTeam(edict_t* ent)
{
    // TODO: Implement team selection logic
    gi.cprintf(ent, PRINT_HIGH, "Team selection not yet implemented\n");
    UI_Close(ent);
}

void UI_Action_Spectate(edict_t* ent)
{
    // TODO: Implement spectator mode
    gi.cprintf(ent, PRINT_HIGH, "Spectator mode not yet implemented\n");
    UI_Close(ent);
}

void UI_Action_ServerInfo(edict_t* ent)
{
    //gi.cprintf(ent, PRINT_HIGH, "Server: %s\n", hostname->string);
    gi.cprintf(ent, PRINT_HIGH, "Map: %s\n", level.mapname);
    gi.cprintf(ent, PRINT_HIGH, "Players: %d/%d\n", 
               (int)ClientsInUse(), (int)maxclients->value);
}

void UI_Action_Help(edict_t* ent)
{
    gi.cprintf(ent, PRINT_HIGH, "Freeze Tag Help:\n");
    gi.cprintf(ent, PRINT_HIGH, "- Freeze enemies by shooting them\n");
    gi.cprintf(ent, PRINT_HIGH, "- Thaw teammates by walking into them\n");
    gi.cprintf(ent, PRINT_HIGH, "- Last team standing wins\n");
}

void UI_Action_Close(edict_t* ent)
{
    UI_Close(ent);
}

qboolean UI_IsActive(edict_t* ent)
{
    client_ui_t* ui = UI_GetClientState(ent);
    return ui ? ui->ui_enabled : false;
}

// Helper function to count active clients
int ClientsInUse(void)
{
    int count = 0;
    for (int i = 0; i < maxclients->value; i++) {
        if (g_edicts[i+1].inuse && g_edicts[i+1].client)
            count++;
    }
    return count;
}
