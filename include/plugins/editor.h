#ifndef TICO_EDITOR_H
#define TICO_EDITOR_H

#include "../tico.h"
#include "resources.h"
#include "tclua.h"
// #include "editor/panel.h"
// #include "editor/object.h"
// #include "editor/grid.h"
// #include "editor/tilemap.h"
#include "editor/widget.h"

enum {
  TICO_WIDGET_NONE = 0,
  TICO_WIDGET_DOWN = (1 << 0),
  TICO_WIDGET_HOVER = (1 << 1)
};

typedef struct tc_EditorFolder tc_EditorFolder;
typedef struct tc_EditorWindow tc_EditorWindow;

typedef int(*EditorPluginInit)(void*, tc_EditorWindow*);
typedef int(*EditorPluginUpdate)(void*);
#define EditorPluginCreate EditorPluginUpdate
#define EditorPluginDraw EditorPluginUpdate
#define EditorPluginDrawPanel EditorPluginUpdate
#define EditorPluginDestroy EditorPluginUpdate
#define EditorPluginSave EditorPluginUpdate

typedef struct tc_GridTool {
	tc_Camera camera;
	tc_Canvas canvas;
	tc_Canvas bg;
	tc_Vec2 grid;
	tc_Vec2 size;
	tc_Vec2 scrolling;
	int scale;
	// int cell;
} tc_GridTool;

struct tc_EditorFolder {
  char name[64];
  // tc_EditorFolder *folders;
  // list_folder_t folders;
  tc_EditorFolder *child;
  tc_EditorFolder *next;
  list_str_t res;
};


struct tc_EditorPlugin {
  void *editor;
  int size;
  EditorPluginCreate create;
  EditorPluginInit init;
  EditorPluginUpdate update;
  EditorPluginDraw draw;
  EditorPluginDrawPanel draw_panel;
	EditorPluginDestroy destroy;
	EditorPluginSave save;
  // void (*update)(void*);
  // void (*draw)(void*);
  // void (*draw_panel)(void*);
};

typedef struct tc_EditorPlugin tc_EditorPlugin;
// typedef map_t(tc_EditorFo)

struct tc_EditorWindow {
  char title[128];
  int open;
  tc_Resource *res;
  void *editor;
  int flags;
  struct tc_EditorPlugin plugin;
};

typedef map_t(tc_EditorPlugin) map_editorplugin_t;
typedef map_t(tc_EditorWindow) map_editorwindow_t;

#define EDITOR_MAX_WINDOWS 32

typedef struct tc_Editor {
	int size;
	tc_Canvas game_canvas;
	tc_ResourceManager *manager;
	tc_Lua *lua;
  map_editorplugin_t plugins;
  // map_editorwindow_t windows;
  map_int_t windows;
  // tc_EditorFolder folders;
  tc_Field *folders;
  int paused;
  int plugins_window;
  int imgui_demo;

  tc_EditorWindow window_pool[EDITOR_MAX_WINDOWS];
  tc_EditorWindow *current_window;
} tc_Editor;

TIC_API tc_EditorPlugin tico_plugin_editor_create(EditorPluginInit init, EditorPluginUpdate update, EditorPluginDraw draw, EditorPluginDrawPanel draw_panel);

TIC_API int tico_plugin_editor_load(tc_Editor *editor);
TIC_API int tico_plugin_editor_update(tc_Editor *editor);
TIC_API int tico_plugin_editor_draw(tc_Editor *editor);
TIC_API int tico_plugin_editor_add_plugin(tc_Editor *editor, const char *type, tc_EditorPlugin *plugin);
TIC_API int PUBLIC_API(tico_plugin_editor_open, const char *uuid);
TIC_API int INTERNAL_API(tico_plugin_editor_open, tc_Editor *editor, const char *uuid);

TIC_API void tico_editor_set_bg(tc_Canvas canvas, int size, tc_Color col1, tc_Color col2);

/*******************
 * Editor Window
 *******************/

TIC_API void tico_editor_init_window(tc_EditorWindow *window, tc_EditorPlugin *plugin);
TIC_API void tico_editor_destroy_window(tc_EditorWindow *window);
TIC_API int tico_editor_draw_window(tc_EditorWindow *window);
TIC_API void tico_editor_draw_window_panel(tc_EditorWindow *window);

#endif

#if defined(TICO_PLUGIN_IMPLEMENTATION)

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui/cimgui.h"

#include "lua/editor.h"

void tico_editor_set_bg(tc_Canvas canvas, int size, tc_Color col1, tc_Color col2) {
	tico_canvas_attach(canvas);
	tico_graphics_fill_rectangle(0, 0, size, size, col1);
	tico_graphics_fill_rectangle(size, 0, size, size, col2);
	tico_graphics_fill_rectangle(0, size, size, size, col2);
	tico_graphics_fill_rectangle(size, size, size, size, col1);
	tico_canvas_detach();
}

#define TICO_PLUGIN_WIDGET_IMPLEMENTATION
#include "editor/widget.h"

#define TICO_PLUGIN_EDITOR_IMPLEMENTATION
#include "editor/image.h"
#include "editor/tileset.h"
#include "editor/tilemap.h"
#include "editor/object.h"

enum {
	NEXT_FOLDER = 0,
	CHILD_FOLDER
};

static tc_EditorFolder* tico_editor_create_folder(const char *name, tc_EditorFolder *other, int loc) {
	tc_EditorFolder *folder = calloc(1, sizeof(*folder));
	strcpy(folder->name, name);
	// list_init(&folder.folders);
	folder->next = NULL;
	folder->child = NULL;
	tc_EditorFolder *aux = other;
	list_init(&folder->res);
	if (other) {
		switch (loc) {
			case NEXT_FOLDER:
				{
					while (aux->next) {
						aux = aux->next;
					}
					aux->next = folder;
				}
				break;
			case CHILD_FOLDER:
				{
					aux = other->child;
					if (!aux) other->child = folder;
					else {
						while (aux->next) {
							aux = aux->next;
						}
						aux->next = folder;
					}
				}
				break;
		}
	}

	return folder;
}

static void tico_editor_draw_folder(tc_Field *folder);

void tico_editor_draw_folder(tc_Field *field) {
  if (!field) return;

  tc_Field *aux = field;
  if (!field->name[0]) aux = field->child;
  ImVec2 size;
  igGetContentRegionAvail(&size);

  tc_Field *el = NULL;
  int flags = 0;
  if (field->type == TIC_FIELD_TYPE_OBJECT) {
    if (field->name[0] != '\0' && igTreeNodeExStr(field->name, ImGuiTreeNodeFlags_Framed)) {
      tico_field_foreach(el, field) tico_editor_draw_folder(el);
      igTreePop();
    } else if (field->name[0] == '\0') {
      tico_field_foreach(el, field) tico_editor_draw_folder(el);
    }
  } else {
    if (igSelectableBool(field->name, 0, 0, (ImVec2){size.x, 12})) tico_plugin_editor_open(field->string);
  }
  // tico_field_foreach(el, field) {

  // }







  // tico_field_foreach(el, field) {
  //   if (el->type == TIC_FIELD_TYPE_OBJECT && igTreeNodeStr(el->name)) {
  //     if (el->child) tico_editor_draw_folder(el->child);
  //     igTreePop();
  //   } else if (el->type != TIC_FIELD_TYPE_OBJECT) {
  //     if (igSelectableBool(el->name, 0, 0, (ImVec2){size.x, 12})) tico_plugin_editor_open(el->string);
  //   }
  // }
  // while (aux) {
  //   if (aux->type == TIC_FIELD_TYPE_OBJECT && igTreeNodeStr(aux->name)) {
  //     if (aux->child) tico_editor_draw_folder(aux->child);
  //     igTreePop();
  //   } else if (aux->type != TIC_FIELD_TYPE_OBJECT) {
  //     if (igSelectableBool(aux->name, 0, 0, (ImVec2){size.x, 12})) {
  //       tico_plugin_editor_open(aux->string);
  //     }
  //   }

  //   aux = aux->next;
  // }
}

// void tico_editor_draw_folder(tc_EditorFolder *folder) {
// 	if (!folder) return;
// 	tc_EditorFolder *aux = folder;
// 	while (aux) {
// 		if (igTreeNodeStr(aux->name)) {
// 			ImVec2 size;
// 			igGetContentRegionAvail(&size);
// 			tico_editor_draw_folder(aux->child);
// 			list_iter_t iter = list_iter(&aux->res);
// 			char **uuid;
// 			while ((uuid = list_next(&iter))) {
// 				// TRACELOG("%s", *uuid);
// 				tc_Resource *res = tico_plugin_resources_get_by_uuid(*uuid);
// 				if (res && igSelectableBool(res->name, 0, 0, (ImVec2){size.x, 12})) {
// 					// tico_editor_open()
// 					// tico_editor_open(&Core.editor, res->type, res->name);
// 					// tico_editor_open(&Core.editor, uuid);
//           // tico_plugin_editor_op
//           tico_plugin_editor_open(*uuid);
// 				}
// 			}
// 			igTreePop();
// 		}
// 		// igBeginChildStr()
// 		aux = aux->next;

// 	}
// }

static tc_EditorWindow* tico_plugin_editor_get_window(tc_Editor *editor, const char *uuid) {
  int *window_index = map_get(&editor->windows, uuid);
  if (!window_index) return NULL;

  return &editor->window_pool[*window_index];
}

static void tico_editor_lua_game_update(tc_Editor *editor) {
  lua_State *L = editor->lua->L;
  if (!L) return;
  lua_getglobal(L, "tico");
  if (!lua_isnil(L, -1)) {
    lua_getfield(L, -1, "editor_update");
    if (!lua_isnil(L, -1)) {
      lua_pushnumber(L, tico_timer_get_delta());
      int err = lua_pcall(L, 1, 0, 0);
      if (err) {
//         tc_lua_traceback(L);
        const char *str = lua_tostring(L, -1);
        ERRLOG("error in tico: %s", str);
      }
    }
    lua_pop(L, 1);
    lua_settop(L, 1);
  }
}

int tico_plugin_editor_load(tc_Editor *editor) {
	if (!tico_plugin_is_active("resources")) {
		tico_plugin_enable_plugin("resources");
	}
	tc_Plugin *plugin = tico_plugin_get("resources");
	editor->manager = plugin->data;
  editor->lua = NULL;
  editor->current_window = NULL;
  editor->imgui_demo = 0;
  editor->folders = editor->manager->fields;
  // TRACELOG("%p", editor->folders->child);
  // tc_Field *el = NULL;
  // tico_field_foreach(el, editor->folders) {
  //   TRACELOG("%s %p", el->name, el->child);
  // }


  memset(editor->window_pool, 0, sizeof(tc_EditorWindow) * EDITOR_MAX_WINDOWS);
  map_init(&editor->windows);
  map_init(&editor->plugins);

	tc_Plugin *lua_plugin = tico_plugin_get("lua");
	if (lua_plugin) {
		tico_plugin_lua_add_lib("editor", luaopen_editor);
		editor->lua = lua_plugin->data;
	}

	editor->game_canvas = tico_canvas_create(tico_window_get_width(), tico_window_get_height());
  editor->paused = 0;
  editor->plugins_window = 0;
  // memset(&editor->folders, 0, sizeof(tc_EditorFolder));

  tc_EditorPlugin image_plugin = tico_plugin_editor_image();
  tc_EditorPlugin tileset_plugin = tico_plugin_editor_tileset();
	tc_EditorPlugin tilemap_plugin = tico_plugin_editor_tilemap();
  tc_EditorPlugin object_plugin = tico_plugin_editor_object();

  tico_plugin_editor_add_plugin(editor, "image", &image_plugin);
  tico_plugin_editor_add_plugin(editor, "tileset", &tileset_plugin);
	tico_plugin_editor_add_plugin(editor, "tilemap", &tilemap_plugin);
  tico_plugin_editor_add_plugin(editor, "object", &object_plugin);


 //  tc_EditorFolder *aux = &editor->folders;
	// while (aux) {
	// 	map_iter_t iter = map_iter(&editor->manager->resources);
	// 	const char *key;
	// 	while ((key = map_next(&editor->manager->resources, &iter))) {
	// 		tc_Resource *res = map_get(&editor->manager->resources, key);
	// 		// TRACELOG("%s %s//%s", aux->name, res->type, res->name);
	// 		if (!strcmp(res->type, aux->name)) {
	// 			list_push(&aux->res, res->uuid);
	// 		}
	// 	}

	// 	aux = aux->next;
	// }


	return 0;
}

int tico_plugin_editor_update(tc_Editor *editor) {
  if (!editor->paused) {
  	if (!editor->lua) return 0;
  	lua_pushnumber(editor->lua->L, tico_timer_get_delta());
  	tico_plugin_lua_pcall("editor_update", 1);
  }
  return 1;
}

int tico_plugin_editor_draw(tc_Editor *editor) {
	

	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  ImGuiViewport *viewport = igGetMainViewport();
  ImVec2 pos;
  ImGuiViewport_GetWorkPos(&pos, viewport);
  igSetNextWindowPos(pos, 0, (ImVec2){0, 0});
  ImGuiViewport_GetWorkSize(&pos, viewport);
  igSetNextWindowSize(pos, 0);
  igPushStyleVarFloat(ImGuiStyleVar_WindowRounding, 0);
  igPushStyleVarFloat(ImGuiStyleVar_WindowBorderSize, 0);
  igPushStyleVarVec2(ImGuiStyleVar_WindowPadding, (ImVec2){0, 0});

  flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  // tc_GameEditor *ed = Core.editor.game_tool->editor;
  if (igBegin("dockspace", NULL, flags)) {
    // igDockSpace(123, const ImVec2 size, ImGuiDockNodeFlags flags, const ImGuiWindowClass *window_class)
    igPopStyleVar(3);

    ImGuiID dockspace_id = igGetIDStr("MyDockspace");
    igDockSpace(dockspace_id, (ImVec2){0, 0}, 0, NULL);

    if (igBeginMenuBar()) {
      if (igBeginMenu("File", 1)) {
        igMenuItemBool("Save", "Ctrl+S", 0, 1);
        igMenuItemBool("Quit", "Ctrl+Q", 0, 1);
        igEndMenu();
      }

      if (igBeginMenu("Game", 1)) {
        igMenuItemBoolPtr("Paused", NULL, (bool*)&editor->paused, 1);
        igEndMenu();
      }
      if (igBeginMenu("Help", 1)) {
      	igMenuItemBoolPtr("ImGui Demo", NULL, (bool*)&editor->imgui_demo, 1);
      	igEndMenu();
      }
      igEndMenuBar();
    }

    igEnd();
  }

  map_iter_t iter = map_iter(&editor->windows);
  const char *key;
  char del[64];
  del[0] = '\0';

  // tc_EditorWindow *current = NULL;

	while ((key = map_next(&editor->windows, &iter))) {
		// tc_EditorWindow *window = map_get(&editor->windows, key);
		tc_EditorWindow *window = tico_plugin_editor_get_window(editor, key);
		if (tico_editor_draw_window(window) && editor->current_window != window) editor->current_window = window;
		if (!window->open) strcpy(del, key);
	}

	if (del[0]) {
    tc_EditorWindow *window = tico_plugin_editor_get_window(editor, del);
    if (editor->current_window == window) editor->current_window = 0;
    tico_editor_destroy_window(window);
    map_remove(&editor->windows, del);
  }

	if (igBegin("props", NULL, 0)) {
		// iter = map_iter(&editor->windows);
		// while ((key = map_next(&editor->windows, &iter))) {
		// 	// tc_EditorWindow *window = map_get(&editor->windows, key);
		// 	tc_EditorWindow *window = tico_plugin_editor_get_window(editor, key);
		// 	// tico_editor_draw_window(window);
		// 	tico_editor_draw_window_panel(window);
		// }
    tico_editor_draw_window_panel(editor->current_window);
	}
	igEnd();

	
	// tico_editor_draw_window_panel(window);

  

  if (igBegin("resources", NULL, 0)) {
		// tc_EditorFolder *folder = &editor->folders;
		tico_editor_draw_folder(editor->folders);
  }
  igEnd();

  tico_canvas_attach(editor->game_canvas);
  tico_plugin_lua_pcall("editor_draw", 0);
	tico_canvas_detach();

  if (igBegin("game", NULL, 0)) {

		tc_Texture tex = editor->game_canvas.tex;
		ImVec2 cPos;
		igGetCursorScreenPos(&cPos);
    ImVec2 size;
    igGetContentRegionAvail(&size);
    float ratio = tico_utils_get_ratio(tex.width, tex.height, size.x, size.y);
		// igImage(tex.id, (ImVec2){tex.width*ratio, tex.height*ratio}, (ImVec2){0, 1}, (ImVec2){1, 0}, (ImVec4){1, 1, 1, 1}, (ImVec4){0, 0, 0, 0});
  	ImDrawList *list = igGetWindowDrawList();

  	// igPushClipRect()
  	ImDrawList_AddImage(list, tex.id, cPos, (ImVec2){cPos.x + tex.width*ratio, cPos.y + tex.height*ratio}, (ImVec2){0, 1}, (ImVec2){1, 0}, 0xffffffff);
  }
  igEnd();

  if (igBegin("logs", NULL, 0)) {
		// igInputText("logs_filter", editor->)
		// igText("[16:20] main.c:18: eita caraaaai");
  }
  igEnd();

  // if (igBegin("plugins", &editor->plugins_window, 0)) {
  //   map_iter_t iter = map_iter(&Core.plugins.plugins);
  //   const char *key;
  //   while ((key = map_next(&Core.plugins.plugins, &iter))) {
  //     float width = igGetWindowWidth();
  //     igSelectableBool(key, 0, 0, (ImVec2){width, 16});
  //   }
  // }
  // igEnd();
	if (editor->imgui_demo) igShowDemoWindow((bool*)&editor->imgui_demo);
	// LOG("%d", editor->imgui_demo);

	return 0;
}

int tico_plugin_editor_add_plugin(tc_Editor *editor, const char *type, tc_EditorPlugin *plugin) {
	tc_EditorPlugin *pl = map_get(&editor->plugins, type);
	if (!pl) map_set(&editor->plugins, type, *plugin);
	tc_EditorFolder *folder = NULL;


	// if (!editor->folders.name[0]) strcpy(editor->folders.name, type);
	// else folder = tico_editor_create_folder(type, &editor->folders, 0);
}

static int tico_plugin_editor_get_window_avail(tc_Editor *editor) {
  for (int i = 0; i < EDITOR_MAX_WINDOWS; i++) {
    if (!editor->window_pool[i].open) return i;
  }

  return -1;
}

int PUBLIC_API(tico_plugin_editor_open, const char *uuid) {
  tc_Plugin *plugin = tico_plugin_get("editor");
  if (!plugin) return -1;
  return INTERNAL_API(tico_plugin_editor_open, plugin->data, uuid);
}
int INTERNAL_API(tico_plugin_editor_open, tc_Editor *editor, const char *uuid) {
  ASSERT(editor != NULL);
  ASSERT(uuid != NULL);

  tc_EditorWindow *window = tico_plugin_editor_get_window(editor, uuid);
  if (window) return -1;

  tc_Resource *res = tico_plugin_resources_get_by_uuid(uuid);
  if (!res) return -1;

  tc_EditorPlugin *plugin = map_get(&editor->plugins, res->type);
  if (!plugin) return -1;

  int window_index = tico_plugin_editor_get_window_avail(editor);
  // TRACELOG("%d", window_index);
  if (!window_index < 0) return -1;

  window = &editor->window_pool[window_index];
  // TRACELOG("%p", win);
  tico_editor_init_window(window, plugin);
  window->res = res;
  sprintf(window->title, "%s: %s", res->type, res->name);
  
  if (window->plugin.init) window->plugin.init(window->editor, window);

  map_set(&editor->windows, uuid, window_index);
  return 1;
}


/******************
 * Editor Window
 ******************/

void tico_editor_init_window(tc_EditorWindow *window, tc_EditorPlugin *plugin) {
  ASSERT(window != NULL);
  ASSERT(plugin != NULL);
	memset(&window->plugin, 0, sizeof(tc_EditorPlugin));
	memcpy(&window->plugin, plugin, sizeof(*plugin));
	window->open = 1;
	window->flags = 0;
	window->editor = malloc(plugin->size);
	strcpy(window->title, "tileset: forest");
}

void tico_editor_destroy_window(tc_EditorWindow *window) {
  if (!window) return;
	if (window->plugin.destroy) window->plugin.destroy(window->editor);
	if (window->editor) free(window->editor);
}

int tico_editor_draw_window(tc_EditorWindow *window) {
  if (!window) return 0;
  int ret = 0;
	if (igBegin(window->title, (bool*)&window->open, window->flags)) {
		if (window->plugin.draw) window->plugin.draw(window->editor);
    ret = igIsWindowFocused(0);
	}
	igEnd();

  return ret;
}

void tico_editor_draw_window_panel(tc_EditorWindow *window) {
  if (!window) return;
  tc_Resource *res = window->res;
  if (res) {
    igText("name: %s", res->name);
    igSameLine(0, 6);
    igButton("...", (ImVec2){26, 16});
    if (igButton("reload##resource_reload", (ImVec2){64, 16})) {
    	lua_State *L = tico_plugin_lua_get_state();
    	lua_pushstring(L, res->type);
    	lua_pushstring(L, res->uuid);
    	tico_lua_json_new_object(L, res->data);

    	tico_plugin_lua_pcall("editor_change", 3);
    }
    igSameLine(0, 6);
    if (igButton("save##window_panel", (ImVec2){64, 16})) {
    	if (window->plugin.save) window->plugin.save(window->editor);
    	tico_plugin_resources_store(res);
      tc_Plugin *plugin = tico_plugin_get("lua");
    	if (!strcmp(res->type, "tilemap") && plugin) {
        tc_Lua *lua = plugin->data;
    		tico_lua_json_new_object(lua->L, res->data);
    		tico_resource_change_lua(res);
    	}
    }
  }
	if (window->plugin.draw_panel) window->plugin.draw_panel(window->editor);
}

#endif