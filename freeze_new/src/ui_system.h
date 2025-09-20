#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include "g_local.h"

// UI System Constants
#define MAX_UI_LINES 32
#define MAX_UI_TEXT 256
#define MAX_UI_ITEMS 16
#define UI_TIMEOUT 30.0f

// UI Types
typedef enum {
    UI_TYPE_NONE,
    UI_TYPE_MOTD,
    UI_TYPE_MENU,
    UI_TYPE_INFO
} ui_type_t;

// UI Item Types
typedef enum {
    UI_ITEM_TEXT,
    UI_ITEM_BUTTON,
    UI_ITEM_SEPARATOR
} ui_item_type_t;

// UI Item Structure
typedef struct ui_item_s {
    ui_item_type_t type;
    char text[MAX_UI_TEXT];
    int color;
    qboolean selectable;
    void (*action)(edict_t* ent);
    struct ui_item_s* next;
} ui_item_t;

// UI Display Structure
typedef struct ui_display_s {
    ui_type_t type;
    char title[MAX_UI_TEXT];
    ui_item_t* items[MAX_UI_ITEMS];
    int item_count;
    int selected_item;
    float display_time;
    float timeout;
    qboolean active;
    qboolean auto_close;
} ui_display_t;

// Client UI State
typedef struct {
    ui_display_t current_ui;
    qboolean ui_enabled;
    float last_input;
} client_ui_t;

// Function Prototypes
void UI_Init(void);
void UI_Shutdown(void);
void UI_LoadMOTD(void);
void UI_ShowMOTD(edict_t* ent);
void UI_ShowMenu(edict_t* ent, const char* title);
void UI_AddItem(ui_display_t* ui, ui_item_type_t type, const char* text, void (*action)(edict_t*));
void UI_Update(edict_t* ent);
void UI_HandleInput(edict_t* ent, int key);
void UI_Close(edict_t* ent);
void UI_Render(edict_t* ent);
qboolean UI_IsActive(edict_t* ent);
int ClientsInUse(void);

// Menu Actions
void UI_Action_JoinTeam(edict_t* ent);
void UI_Action_Spectate(edict_t* ent);
void UI_Action_ServerInfo(edict_t* ent);
void UI_Action_Help(edict_t* ent);
void UI_Action_Close(edict_t* ent);

#endif // UI_SYSTEM_H
