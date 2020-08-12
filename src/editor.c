#include "tico.h"

static void editor_set_bg(tc_Canvas canvas, int size, tc_Color col1, tc_Color col2) {
	tic_canvas_attach(canvas);
	tic_graphics_fill_rectangle(0, 0, size, size, col1);
	tic_graphics_fill_rectangle(size, 0, size, size, col2);
	tic_graphics_fill_rectangle(0, size, size, size, col2);
	tic_graphics_fill_rectangle(size, size, size, size, col1);
	tic_canvas_detach();
}

void tic_editor_tilemap_resize(tc_TilemapEditor *editor, int w, int h) {
	if (w <= 0 || h <= 0) return;
	tic_tilemap_resize(editor->map, w, h);
	tic_canvas_destroy(&editor->canvas);
	// TRACELOG("%d %d", w, h);
	editor->canvas = tic_canvas_create(w*editor->map->grid.x, h*editor->map->grid.y);
	editor->size.x = editor->map->width;
	editor->size.y = editor->map->height;
}

static void editor_tileset_draw_panel(tc_TilesetEditor *editor) {
	// tic_ui_indent();
	tic_ui_layout_row(1, ((int[]){-1}), 0);
	// if (editor->name) tic_ui_textbox(editor->name, strlen(editor->name));
	// if (editor->filename) tic_ui_textbox(editor->filename, strlen(editor->filename));
	// tic_ui_button("change##tileset");
	if (tic_ui_button("save##tileset")) {
		tic_editor_tileset_save(editor);
	}
	// tic_editor_draw_tool_panel(&editor->tool);
	tic_ui_number("scale", &editor->scale, 1);
	editor->scale = tic_clamp(editor->scale, 1, 10);
	// tic_ui_unindent();
}

static void editor_tilemap_draw_panel(tc_TilemapEditor *editor) {
	int w = -1;
	// tic_ui_indent();
	tic_ui_layout_row(1, &w, 0);
	if (tic_ui_button("save##tilemap")) {
		// tic_editor_tilemap_save(editor);
		tc_Resource *res = editor->tool->res;
		if (editor->tool) {
			
			if (res && strcmp(res->path, editor->tool->filename)) {
				strcpy(res->path, editor->tool->filename);
			}
			tic_resources_tilemap_store(&Core.resources, res);
		}
	}
	// tic_editor_draw_tool_panel(editor->tool);
	tic_ui_number("map width", &editor->size.x, 1);
	tic_ui_number("map height", &editor->size.y, 1);
	if (tic_ui_button("set")) {
		if (editor->size.x != editor->map->width ||
			  editor->size.y != editor->map->height) {
			// tic_tilemap_resize(editor->map, editor->size.x, editor->size.y);
			tic_editor_tilemap_resize(editor, editor->size.x, editor->size.y);
		}
	}
	// tic_ui_unindent();
}

#define TICO_PLUGIN_IMPLEMENTATION
#include "plugins/editor.h"

void tic_editor_load(tc_Editor *editor) {
	// tic_editor_tilemap_init(&editor->tilemap_editor, tc_Tilemap *tilemap)
	// tic_editor_tileset_init(&editor->tileset_editor);
	// tic_editor_tilemap_init(&editor->tilemap_editor);
	editor->bg = tic_canvas_create(32, 32);
	// editor->active_tab[0] = '\0';
	editor->active_tool = NULL;
	tic_texture_set_wrap(&editor->bg.texture, GL_REPEAT, GL_REPEAT);

	editor->game_canvas = tic_canvas_create(Core.window.width, Core.window.height);
	editor->sidebar_width = 128;
	// memset(editor->tools, 0, MAX_TOOLS * sizeof(tc_Tool));
	// list_init(&editor->tool);
	map_init(&editor->tool);
	map_init(&editor->plugins);

	editor->game_tool = NULL;
	editor->fullscreen_window = 0;

	// tc_EditorPlugin plugin = {0};
	// plugin.editor = malloc(sizeof(tc_GameEditor));
	// plugin.size = sizeof(tc_GameEditor);
	// plugin.init = (void(*)(void*, tc_Tool*))&tic_editor_game_init;
	// plugin.draw = (void(*)(void*))&tic_editor_game_draw;
	// plugin.draw_panel = (void(*)(void*))&tic_editor_game_panel;
	tc_EditorPlugin plugin = tic_plugin_editor_game();


	tc_Tool *tool = malloc(sizeof(*tool));
	tic_editor_init_tool(tool, &plugin);
	// tool->fullscreen = 1;
	tool->init(tool->editor, tool);
	tc_GameEditor *ed = tool->editor;
	ed->canvas = &editor->game_canvas;
	strcpy(tool->name, "game");
	tool->filename[0] = '\0';
	// list_push(&editor->tool, tool);
	map_set(&editor->tool, tool->name, tool);

	// tic_editor_load_state(editor);

	editor->game_tool = tool;
}

void tic_editor_default(tc_Editor *editor) {
	// tc_EditorPlugin tileset_plugin = {0};
	// tileset_plugin.size = sizeof(tc_TilesetEditor);
	// tileset_plugin.init = (void(*)(void*, tc_Tool*))&tic_editor_tileset_init;
	// tileset_plugin.draw = (void(*)(void*))&tic_editor_tileset_draw;
	// tileset_plugin.draw_panel = (void(*)(void*))&editor_tileset_draw_panel;
	tc_EditorPlugin tileset_plugin = tic_plugin_editor_tileset();
	// tileset_plugin.

	// tc_EditorPlugin tilemap_plugin = {0};
	// tilemap_plugin.size = sizeof(tc_TilemapEditor);
	// tilemap_plugin.init = (void(*)(void*, tc_Tool*))&tic_editor;
	// tilemap_plugin.draw = (void(*)(void*))&tic_editor_tilemap_draw;
	// tilemap_plugin.draw_panel = (void(*)(void*))&editor_tilemap_draw_panel;
	tc_EditorPlugin tilemap_plugin = tic_plugin_editor_tilemap();

	tic_editor_add_plugin(editor, "tileset", &tileset_plugin);
	tic_editor_add_plugin(editor, "tilemap", &tilemap_plugin);

	tic_editor_load_state(editor);
}

void tic_editor_destroy(tc_Editor *editor) {
	tic_editor_tileset_destroy(&editor->tileset_editor);
	tic_editor_tilemap_destroy(&editor->tilemap_editor);
}

void tic_editor_open(tc_Editor *editor, const char *type, const char *name) {
	int size = strlen(type) + strlen(name) + 2;
	char key[size];
	sprintf(key, "%s//%s", type, name);

	tc_Tool **t = map_get(&editor->tool, key);
	if (t) {
		(*t)->open = tc_true;
		mu_Container *cnt = (*t)->cnt;
		if (cnt) {
			cnt->open = tc_true;
			if (!(*t)->fullscreen) mu_bring_to_front(Core.ui.ctx, cnt);
		}
		return;
	}

	tc_Resource *res = tic_resources_get(&Core.resources, type, name);
	if (!res) return;

	tc_EditorPlugin *plugin = map_get(&editor->plugins, type);
	if (!plugin) return;

	tc_Tool *tool = malloc(sizeof(*tool));
	tic_editor_init_tool(tool, plugin);
	tool->res = res;
	strcpy(tool->name, res->name);
	strcpy(tool->filename, res->path);

	tool->init(tool->editor, tool);
	tool->open = tc_true;

	map_set(&editor->tool, key, tool);

	// TRACELOG("%s", tool->name);

}

void tic_editor_add_plugin(tc_Editor *editor, const char *type, tc_EditorPlugin *plugin) {
	tc_EditorPlugin *pl = map_get(&editor->plugins, type);
	if (!pl) map_set(&editor->plugins, type, *plugin);
}

void tic_editor_load_state(tc_Editor *editor) {
	cJSON *root = tic_json_open("editor.json");
	if (!root) return;

	// TRACELOG("editor.json");

	cJSON *props = tic_json_get_object(root, "props");
	editor->sidebar_width = tic_json_get_opt_number(props, "sidebar", 128);
	cJSON *tools = tic_json_get_object(root, "tools");
	cJSON *window = NULL;
	cJSON_ArrayForEach(window, tools) {
		// TRACELOG("%s", window->string);
		char *p = strstr(window->string, "//");
		int sz = strlen(window->string);
		// char name[sz];
		if (p) {
			sz = p - window->string;
			p += 2;
		}
		char *name = p ? p : "";
		char type[sz];
		memcpy(type, window->string, sz);


		cJSON *n = NULL;
		int open = tic_json_get_boolean(window, "open");
		int fullscreen = tic_json_get_boolean(window, "fullscreen");
		tc_Rect r;
		int i = 0;

		cJSON *arr = tic_json_get_array(window, "rect");
		cJSON_ArrayForEach(n, arr) {
			r.data[i++] = n->valueint;
		}
		r.x = tic_clamp(r.x, 0, Core.window.width - 64);
		r.y = tic_clamp(r.y, 0, Core.window.height - 64);
		r.w = tic_clamp(r.w, 128, Core.window.width - 64);
		r.h = tic_clamp(r.h, 128, Core.window.height - 64);
		tic_editor_open(editor, type, name);
		tc_Tool **tool = map_get(&editor->tool, window->string);
		// TRACELOG("%s %s %s %p", window->string, type, name, tool);
		if (tool) {
			(*tool)->win_rect = r;
			(*tool)->open = open;
			(*tool)->fullscreen = fullscreen;
		}
		// tool->win_rect = r;
	}

	tic_json_delete(root);
}

void tic_editor_save_state(tc_Editor *editor) {
	cJSON *root = tic_json_create();

	const char *key;
	map_iter_t iter = map_iter(&editor->tool);
	cJSON *props = tic_json_set_object(root, "props", NULL);
	tic_json_set_number(props, "sidebar", editor->sidebar_width);
	cJSON *tools = tic_json_set_object(root, "tools", NULL);
	while ((key = map_next(&editor->tool, &iter))) {
		// TRACELOG("%s", key);
		tc_Tool *t = *map_get(&editor->tool, key);
		mu_Container *cnt = t->cnt;
		// tc_Resource *r
		// int sz = strlen(t->name) + strlen(t->uuid) + strlen("window:##");
		// char name[sz];
		tc_Rect r = tic_rect(0, 0, 0, 0);
		if (cnt) r = cnt->rect;

		// cJSON *arr = tic_json_set_array(root, (char*)key, NULL);
		cJSON *win = tic_json_set_object(tools, (char*)key, NULL);
		cJSON *arr = tic_json_set_array(win, "rect", NULL);
		for (int i = 0; i < 4; i++) {
			cJSON *n = cJSON_CreateNumber(r.data[i]);
			cJSON_AddItemToArray(arr, n);
		}
		// r.x = tic_clamp(r.x, 0, Core.window.width - 64);
		// r.y = tic_clamp(r.y, 0, Core.window.height - 64);
		tic_json_set_boolean(win, "open", t->open);
		tic_json_set_boolean(win, "fullscreen", t->fullscreen);
		// tic_json_set_array(root, char *name, cJSON *const jsonArray)
	}

	tic_json_save("editor.json", root);
	tic_json_delete(root);
	TRACELOG("editor state saved");
}

tc_Resource* tic_editor_resource_viewer(tc_Editor *editor) {
	map_iter_t iter = map_iter(&Core.resources.resources);
	const char *key;
	// TRACELOG("eita");
	while((key = map_next(&Core.resources.resources, &iter))) {
		tc_Resource *res = map_get(&Core.resources.resources, key);
		// TRACELOG("%s %p", key, res);
		int sz = strlen(res->type) + strlen(res->name) + strlen("###res_button");
		char name[sz];
		sprintf(name, "%s//%s##res_button", res->type, res->name);
		// TRACELOG("%s", name);
		if (tic_ui_button(name)) return res;
	}

	return NULL;
}

void tic_editor_set_tileset(tc_Editor *editor, const char *name, tc_Tileset *tileset) {
	if (name) {
		tc_Resource *res = tic_resources_get(&Core.resources, "tileset", name);
		// TRACELOG("tileset##%s", name);
		// TRACELOG("%s %p", name, res);
		if (!res) {
			ERRLOG("Tileset %s not loaded", name);
			return;
		}
		// TRACELOG("q");
		tc_EditorPlugin plugin = {0};
		plugin.editor = malloc(sizeof(tc_TilesetEditor));
		plugin.init = (void(*)(void*, tc_Tool *tool))&tic_editor_tileset_init;
		plugin.draw = (void(*)(void*))&tic_editor_tileset_draw;
		plugin.draw_panel = (void(*)(void*))&editor_tileset_draw_panel;

		tc_Tool *tool = malloc(sizeof(*tool));
		tic_editor_init_tool(tool, &plugin);
		// TRACELOG("%s", tool.uuid);
		strcpy(tool->name, res->name);
		strcpy(tool->filename, res->path);
		tool->res = res;
		// // tool->editor = &editor->tileset_editor;
		// tool->editor = malloc(sizeof(tc_TilesetEditor));
		// // tic_editor_tileset_init(tool->editor, tc_Tool *tool)
		// tool->init = (void(*)(void*, tc_Tool *tool))&tic_editor_tileset_init;
		// tool->draw_panel = (void(*)(void*))&editor_tileset_draw_panel;
		// tool->draw = (void(*)(void*))&tic_editor_tileset_draw;

		tool->init(tool->editor, tool);
		map_set(&editor->tool, tool->name, tool);
		// tc_Tool *t = list_last(&editor->tool);
		// TRACELOG("%d", editor->tool.base.size);
		// tool->uuid[TOOL_UUID_SIZE] = '\0';
		// TRACELOG("%s", tool->uuid);
		// tic_editor_tileset_init(tool->editor, tool);
		// editor->tileset_editor.name = res->name;
		// editor->tileset_editor.filename = res->path;
		// strcpy(editor->tools[1].name, res->name);
		// strcpy(editor->tools[1].filename, res->path);
		// editor->tools[1].res = res;
		// editor->tools[1].editor = &editor->tileset_editor;
		// editor->tools[1].draw_panel = &editor_tileset_draw_panel;
		// strcpy(editor->tileset_editor.tool.name, res->name);
		// strcpy(editor->tileset_editor.tool.filename, res->path);
		// editor->tileset_editor.tool.res = res;
		tic_editor_tileset_change(tool->editor, res->data);
	} else {
		editor->tileset_editor.filename = NULL;
		tic_editor_tileset_change(&editor->tileset_editor, tileset);
	}

}

void tic_editor_set_tilemap(tc_Editor *editor, const char *name, tc_Tilemap *tilemap) {
	if (name) {
		tc_Resource *res = tic_resources_get(&Core.resources, "tilemap", name);

		if (!res) {
			ERRLOG("Tilemap %s not loaded", name);
			return ;
		}
		tc_EditorPlugin plugin = {0};
		// plugin.editor = malloc(sizeof(tc_TilemapEditor));
		plugin.size = sizeof(tc_TilemapEditor);
		plugin.init = (EditorPluginInit)tic_editor_tilemap_init;
		plugin.draw = (EditorPluginDraw)tic_editor_tilemap_draw;
		plugin.draw_panel = (EditorPluginDrawPanel)editor_tilemap_draw_panel;

		tc_Tool *tool = malloc(sizeof(*tool));
		tic_editor_init_tool(tool, &plugin);

		strcpy(tool->name, res->name);
		strcpy(tool->filename, res->path);
		tool->res = res;
		tool->fullscreen = tc_true;
		tool->flags = MU_OPT_NOFRONT | MU_OPT_NORESIZE | MU_OPT_NOFRAME;
		// tool->editor = malloc(sizeof(tc_TilemapEditor));
		// tool->draw_panel = (void(*)(void*))&editor_tilemap_draw_panel;
		// tool->draw = (void(*)(void*))&tic_editor_tilemap_draw;


		// list_push(&editor->tool, tool);
		map_set(&editor->tool, tool->name, tool);


		tool->init(tool->editor, tool);

		// tic_editor_tilemap_init(tool->editor, tool);

		// editor->tilemap_editor.name = res->name;
		// editor->tilemap_editor.filename = res->path;
		// strcpy(editor->tilemap_editor.tool.name, res->name);
		// strcpy(editor->tilemap_editor.tool.filename, res->path);
		// editor->tilemap_editor.tool.res = res;
		tic_editor_tilemap_change(tool->editor, res->data);
		return;
	}
	editor->tilemap_editor.filename = NULL;
	editor->tilemap_editor.name = NULL;
	tic_editor_tilemap_change(&editor->tilemap_editor, tilemap);
}

void tic_editor_new_tileset(tc_Editor *editor, int w, int h) {}
void tic_editor_new_tilemap(tc_Editor *editor, int w, int h) {}

void tic_editor_update(tc_Editor *editor) {}

void tic_editor_draw(tc_Editor *editor) {
	int w, h;

	editor_set_bg(editor->bg, 16, tic_color3(255, 218, 112), tic_color3(244, 176, 60));

	tic_window_get_size(&w, &h);
	tic_canvas_draw_part(editor->bg, tic_rectf(0, 0, w, h), 0, 0, WHITE);

	// editor_set_bg(editor->tilemap_editor.bg, 16, tic_color3(102, 59, 147), tic_color3(139, 114, 222));
	// tic_canvas_attach(editor->tilemap_editor.canvas);
	// tic_canvas_draw_part(editor->tilemap_editor.bg, tic_rectf(0, 0, editor->tilemap_editor.canvas.width, editor->tilemap_editor.canvas.height), 0, 0, WHITE);
	// tic_tilemap_draw(*editor->tilemap_editor.map);
	// tic_canvas_detach();
	// tic_canvas_draw(editor->tilemap_editor.canvas, -16, 0, WHITE);
	// if (editor->tilemap_editor.map) tic_editor_tilemap_draw(&editor->tilemap_editor);
	// if (editor->tileset_editor.tileset) tic_editor_tileset_draw(&editor->tileset_editor);
	// list_iter_t iter = list_iter(&editor->tool);
	map_iter_t iter = map_iter(&editor->tool);
	const char *key;
	// tc_Tool **t = NULL;
	tc_Tool *active_tool = NULL;
	if (mu_begin_window_ex(Core.ui.ctx, "!window:fullscreen", mu_rect(0, 0, 256, 64), MU_OPT_NOTITLE | MU_OPT_NOFRONT | MU_OPT_NOFRAME)) {
		mu_Container *fscnt = tic_ui_get_current_container();
		if (fscnt) {
			fscnt->rect.x = editor->sidebar_width;
			fscnt->rect.y = 0;
			fscnt->rect.w = Core.window.width-editor->sidebar_width;
			fscnt->rect.h = Core.window.height;
		}
		int n[editor->fullscreen_window];
		memset(n, 0, sizeof(n));
		// TRACELOG("%i", n[0]);

		tic_ui_layout_row(editor->fullscreen_window, n, 0);
		editor->fullscreen_window = 0;
		while((key = map_next(&editor->tool, &iter))) {
			tc_Tool **t = map_get(&editor->tool, key);
			tc_Resource *res = (*t)->res;
			// (*t)->open = tc_true;
			tc_Tool *at = editor->active_tool;
			if (at && (!at->fullscreen || !at->open)) editor->active_tool = NULL;
			if ((*t)->open && (*t)->fullscreen && (*t)->draw) {
				// (*t)->draw((*t)->editor);
				editor->fullscreen_window++;
				if (!editor->active_tool) editor->active_tool = (*t);
				if (tic_ui_button((*t)->name)) {
					// TRACELOG("%s", (*t)->name);
					editor->active_tool = (*t);
				}
				(*t)->cnt = fscnt;
			} else if ((*t)->open) {
				int sz = strlen("##window_tool_") + strlen(key) + strlen((*t)->uuid);
				char nm[sz];
				sprintf(nm, "%s##window_tool_%s", key, (*t)->uuid);
				if (mu_begin_window_ex(Core.ui.ctx, nm, (*t)->win_rect, (*t)->flags)) {
					mu_Container *cnt = tic_ui_get_current_container();
					(*t)->cnt = cnt;
					if (!(*t)->cnt) cnt->open = (*t)->open;
					(*t)->open = tc_true;
					// cnt = (*t)->cnt;
					if (cnt) if (cnt->rect.x < editor->sidebar_width) cnt->rect.x += (editor->sidebar_width - cnt->rect.x);
					if (t && (*t)->draw) (*t)->draw((*t)->editor);
					tic_ui_end_window();
				} else {
					(*t)->open = tc_false;
				}
			}
			
			
			// TRACELOG("%s", nm);
		}
		tic_ui_layout_row(1, ((int[]){-1}), -1);
		tic_ui_begin_panel("window:fs_panel");
		if (editor->active_tool) editor->active_tool->draw(editor->active_tool->editor);
		tic_ui_end_panel();
		mu_end_window(Core.ui.ctx);
	}
		// TRACELOG("uepa");
	if (mu_begin_window_ex(Core.ui.ctx, "editor_sidebar", mu_rect(0, 0, editor->sidebar_width, Core.window.height), MU_OPT_NOTITLE | MU_OPT_HRESIZE)) {
		mu_Container *cnt = tic_ui_get_current_container();
		// cnt->rect = tic_rect(0, 0, Core.editor.sidebar_width, Core.window.height);
		// cnt->body = cnt->rect;
		cnt->rect.h = Core.window.height;
		cnt->body.h = Core.window.height;
		Core.editor.sidebar_width = cnt->rect.w;
		int change = 0;
		int w = -1;
		tic_ui_layout_row(1, &w, -48);
		tic_ui_begin_panel("editor panel##99124");
		{
			// if (editor->tilemap_editor.tool.draw_panel && tic_ui_header("tilemap editor")) {
			// 	mu_Container *cnt = editor->tilemap_editor.tool.cnt;
			// 	// if (cnt) tic_ui_checkbox("##tilemap_view", &cnt->open);	
			// 	editor->tilemap_editor.tool.draw_panel(&editor->tilemap_editor);
			// 	// tic_ui_end_treenode();
			// }
			// if (editor->tileset_editor.tool.draw_panel && tic_ui_header("tileset editor")) {
			// 	editor->tileset_editor.tool.draw_panel(&editor->tileset_editor);
			// 	// tic_ui_end_treenode();
			// }
			// for (int i = 0; i < MAX_TOOLS; i++) {
			// 	char nm[24] = "editor#tool_";
			// 	char ch[2];
			// 	sprintf(ch, "%x", i);
			// 	// ch[0] = 40 + i;
			// 	// ch[1] = '\0';
			// 	strcat(nm, ch);
			// 	if (editor->tools[i].draw_panel && tic_ui_header(nm)) {
			// 		tic_ui_indent();
			// 		tic_editor_draw_tool_panel(&editor->tools[i]);
			// 		tic_ui_unindent();
			// 		// editor->tools[i].draw_panel(editor->tools[i].editor);
			// 	}
			// }
			map_iter_t iter = map_iter(&editor->tool);
			const char *key;
			// tc_Tool **t = NULL;
			while((key = map_next(&editor->tool, &iter))) {
				tc_Tool **tool = map_get(&editor->tool, key);
				int sz = 12 + TOOL_UUID_SIZE + strlen(key);
				char nm[sz];
				sprintf(nm, "%s##editor_tool_%s", key, (*tool)->uuid);
				// sprintf(nm, "editor#tool_%s", tool->uuid);
				mu_Container *cc = (*tool)->cnt;
				// if (cc) (*tool)->open = cc->open != (*tool)->open ? cc->open : (*tool)->open;
				tic_ui_layout_row(2, ((int[]){18, -1}), 0);
				if (tic_ui_checkbox(nm, &(*tool)->open)) {
					if (cc && !(*tool)->fullscreen) cc->open = (*tool)->open;
				}
				// TRACELOG("%s", key);
				tic_ui_begin_column();
				if ((*tool)->draw_panel && tic_ui_header(nm)) {
					// tic_ui_indent();
					tic_editor_draw_tool_panel((*tool));
					// tic_ui_unindent();
				}
				// if (cc) (*tool)->open = cc->open;
				// if (cc) cc->open = (*tool)->open;
				tic_ui_end_column();
				tic_ui_layout_row(1, ((int[]){-1}), 0);
			}
			tic_ui_end_panel();
		}
		// tic_ui_layout_row(1, &w, -64);
		tic_ui_layout_row(1, &w, -1);
		if (mu_button_ex(Core.ui.ctx, NULL, MU_ICON_MENU, MU_OPT_ALIGNCENTER)) {
			tic_ui_open_popup("window_list");
		}

		if (tic_ui_begin_popup("window_list")) {
			// if (editor->tilemap_editor.tool.cnt) tic_ui_checkbox("tilemap", &((mu_Container*)editor->tilemap_editor.tool.cnt)->open);
			// if (editor->tileset_editor.tool.cnt) tic_ui_checkbox("tileset", &((mu_Container*)editor->tileset_editor.tool.cnt)->open);
			// if (editor->game_cnt) tic_ui_checkbox("game", &((mu_Container*)editor->game_cnt)->open);
			// tc_Resource res = {0};
			// tic_editor_tilemap_create(&res);
			tc_Resource *res = NULL;
			if ((res = tic_editor_resource_viewer(editor))) {
				tic_editor_open(editor, res->type, res->name);
			}
			tic_ui_end_popup();
		}
		tic_ui_end_window();
	}
}

int tic_editor_begin_draw(tc_Editor *editor) {
	int res = tic_ui_begin_window("game", tic_rect(0, 0, 240, 190));
	if (res) {
		tic_canvas_attach(editor->game_canvas);
		if (!editor->game_cnt) editor->game_cnt = tic_ui_get_current_container();
	}
	// int res = 1;
	return res;
}

void tic_editor_end_draw(tc_Editor *editor) {
	tic_canvas_detach();
	mu_Container *cnt = tic_ui_get_current_container();
	float cw = editor->game_canvas.width;
	float ch = editor->game_canvas.height;
	// float cw = editor->tilemap_editor.canvas.width;
	// float ch = editor->tilemap_editor.canvas.height;
	float ww = cnt->body.w;
	float hh = cnt->body.h;
	float ratio = tic_min((ww-10)/cw, (hh-12)/ch);

	int i = cw*ratio;
	tic_ui_layout_row(1, &i, ch*ratio);

	// TRACELOG("%f", ratio);

	tic_ui_image(editor->game_canvas.texture, mu_vec2(0, 0), tic_rect(0, ch, cw, -ch), WHITE);
	// tic_ui_image(editor->tilemap_editor.canvas.texture, mu_vec2(0, 0), tic_rect(0, ch, cw, -ch), WHITE);

	tic_ui_end_window();

	// tic_canvas_draw(editor->game_canvas, 0, 0, WHITE);
}

void tic_editor_set_game_canvas(tc_Editor *editor, tc_Canvas *canvas) {
	if (!editor->game_tool || !editor->game_tool->editor) return;
	tc_GameEditor *ed = editor->game_tool->editor;

	ed->canvas = canvas;
}

int tic_editor_is_game_open(tc_Editor *editor) {
	if (!editor->game_tool) return 0;
	mu_Container *cnt = editor->game_tool->cnt;
	// TRACELOG("%p", cnt);

	return cnt && cnt->open;
}

void tic_editor_set_game_open(tc_Editor *editor, int open) {}

void tic_editor_init_tool(tc_Tool *tool, tc_EditorPlugin *plugin) {
	memset(tool, 0, sizeof(*tool));
	memset(tool->uuid, 0, TOOL_UUID_SIZE);
	tic_utils_generate_uuid(tool->uuid, TOOL_UUID_SIZE);
	tool->uuid[TOOL_UUID_SIZE] = '\0';

	// tool->editor = plugin->editor;
	tool->editor = NULL;
	if (plugin->editor) tool->editor = plugin->editor;
	if (!tool->editor && plugin->size > 0) tool->editor = malloc(plugin->size);
	// else if (plugin->editor)
	tool->init = plugin->init;
	tool->update = plugin->update;
	tool->draw = plugin->draw;
	tool->draw_panel = plugin->draw_panel;
	tool->flags = 0;
	tool->fullscreen = 0;
	tool->win_rect = tic_rect(0, 0, 240, 220);
}

void tic_editor_destroy_tool(tc_Tool *tool) {
	if (tool->canvas.id != 0) tic_canvas_destroy(&tool->canvas);
}

void tic_editor_draw_tool_panel(tc_Tool *tool) {
	// mu_Container *cnt = tool->cnt;
	int s = strlen("dependencies##") + strlen(tool->uuid);
	char name[s+1];
	sprintf(name, "dependencies##%s", tool->uuid);
	name[s] = '\0';
	// tic_ui_layout_row(2, ((int[]){24, -1}), 0);
	// if (tool->cnt) {
	// 	int sz = 2 + strlen("deps_") + strlen(tool->name) + 2;
	// 	char nm[sz];
	// 	sprintf(nm, "##deps_%s", tool->name);
	// 	tic_ui_checkbox(nm, &cnt->open);
	// }

	// TRACELOG("%s", name);
	tic_ui_checkbox("fullscreen", &tool->fullscreen);
	if (tool->res && tic_ui_header(name)) {
		tic_ui_begin_column();
		int u[2] = {-32, -1};
		tic_ui_indent();
		tic_ui_layout_row(2, u, 0);
		list_iter_t iter = list_iter(&tool->res->deps);
		tc_ResourceDep *dep;
		// tic_ui_get_layout()
		while((dep = list_next(&iter))) {
			int sz = strlen(dep->res->name) + 2;
			// float ww =
			char nm[sz];
			sprintf(nm, "##%s", dep->res->name);
			// tic_ui_text(nm);
			char *part = strstr(nm, "##") + 2;
			mu_textbox_ex(Core.ui.ctx, part, strlen(part), MU_OPT_NOINTERACT);
			// tic_ui_button("...");
			if (mu_button_ex(Core.ui.ctx, nm, MU_ICON_DOT, MU_OPT_ALIGNCENTER)) {
				tic_editor_open(&Core.editor, dep->res->type, dep->res->name);
				// TRACELOG("%s %s", dep->res->type, dep->res->name);
			}
		}
		tic_ui_unindent();
		tic_ui_end_column();
	}
	if (tool->name[0] != 0) tic_ui_textbox(tool->name, sizeof(tool->name));


	if (tool->filename[0] != 0) {
		tic_ui_begin_column();
		tic_ui_layout_row(2, ((int[]){-32, -1}), 0);
		// tic_ui_textbox(tool->filename, sizeof(tool->filename));
		mu_textbox_ex(Core.ui.ctx, tool->filename, sizeof(tool->filename), MU_OPT_NOINTERACT);
		int sz = strlen(tool->filename) + 2;
		char btn_name[sz];
		sprintf(btn_name, "##%s", tool->filename);
		mu_button_ex(Core.ui.ctx, btn_name, MU_ICON_DOT, MU_OPT_ALIGNCENTER);
		tic_ui_end_column();
	}
	
	if (tool->draw_panel && tool->editor) tool->draw_panel(tool->editor);

	// TRACELOG("%s", name);
}

/********************
 * Game
 ********************/

void tic_editor_game_init(tc_GameEditor *editor, tc_Tool *tool) {
	editor->tool = tool;
	editor->canvas = NULL;
	editor->paused = 0;
	// tool->draw = (void(*)(void*))&tic_editor_game_draw;
	// tool->draw_panel = (void(*)(void*))&tic_editor_game_panel;
}

int tic_editor_game_draw(tc_GameEditor *editor) {
	// int sz = 14 + TOOL_UUID_SIZE;
	// char nm[sz];
	// strcpy(nm, "window##tool_");
	// strcat(nm, editor->tool->uuid);
	// nm[sz-1] = '\0';


	// if (tic_ui_begin_window(nm, mu_rect(128, 64, 240, 260))) {
		// TRACELOG("%s", nm);
		// mu_Container *cnt = tic_ui_get_current_container();
		// // if (!editor->cnt) editor->cnt = cnt;
		// if (editor->tool && !editor->tool->cnt) editor->tool->cnt = cnt;
	mu_Container *cnt = editor->tool->cnt;
		if (editor->canvas) {
			// tic_ui_layout_row(1, ((int[]){-1}), -1);
			tc_Canvas *c = editor->canvas;
			float cw = c->width;
			float ch = c->height;
			// float cw = editor->tilemap_editor.canvas.width;
			// float ch = editor->tilemap_editor.canvas.height;
			float ww = cnt->body.w;
			float hh = cnt->body.h;
			float ratio = tic_min((ww-10)/cw, (hh-12)/ch);

			int i = cw*ratio;
			tic_ui_layout_row(1, &i, ch*ratio);

			// TRACELOG("%f", ratio);

			tic_ui_image(c->texture, mu_vec2(0, 0), tic_rect(0, ch, cw, -ch), WHITE);
		}

	// 	tic_ui_end_window();
	// }
}

int tic_editor_game_panel(tc_GameEditor *editor) {

}


/*********************
 * Tileset
 *********************/

void tic_editor_tileset_init(tc_TilesetEditor *editor, tc_Tool *tool) {

	editor->cell = -1;
	memset(editor->mask, 0, sizeof(int) * 9);
	// tic_editor_init_tool(editor->tool);
	editor->tool = tool;
	// if (tool) tool->draw = (void(*)(void*))&tic_editor_tileset_draw;
	// if (tool) tool->draw_panel = (void (*)(void*))&editor_tileset_draw_panel;
	editor->tileset = NULL;
	// if (tool->res) editor->tileset = tool->res->data;
	editor->image_name = NULL;
	// editor->filename = NULL;
	editor->scale = 3;
	editor->cnt = NULL;

	// editor->draw_panel = &editor_tileset_draw_panel;
	editor->draw_panel = NULL;
	if (tool->res) tic_editor_tileset_change(editor, tool->res->data);
}

void tic_editor_tileset_destroy(tc_TilesetEditor *editor) {
	tic_canvas_destroy(&editor->canvas);
}

void tic_editor_tileset_change(tc_TilesetEditor *editor, tc_Tileset *tileset) {
	tic_canvas_destroy(&editor->canvas);
	editor->tileset = tileset;
	memset(editor->mask, 0, sizeof(int) * 9);
	editor->canvas = tic_canvas_create(editor->tileset->image->width, editor->tileset->image->height);
}

static void tic_editor_tileset_draw_canvas(tc_TilesetEditor editor) {
	// tic_ui_layout_row(1, ((int[]){-1}), 0);
	tic_canvas_attach(editor.canvas);
	tic_image_draw(*editor.tileset->image, 0, 0, WHITE);
	tic_canvas_detach();
}

int tic_editor_tileset_draw(tc_TilesetEditor *editor) {
	int res = 0;
	tic_canvas_attach(editor->canvas);
	tic_graphics_clear(tic_color(0, 0, 0, 0));
	// tic_texture_draw(tileset->tex, tic_rectf(0, 0, tileset->tex.width, tileset->tex.height), tic_rectf(0, 0, tileset->tex.width, tileset->tex.height), WHITE);
	tic_image_draw(*editor->tileset->image, 0, 0, WHITE);
	// float ww = editor->tileset->image.width / editor->tileset->tilesize.x;
	float hh = editor->tileset->image->height / editor->tileset->tilesize.y;
	float mw = editor->tileset->tilesize.x/3;
	float mh = editor->tileset->tilesize.y/3;

	int columns = editor->tileset->columns;
	for (int i = 0; i < editor->tileset->tilecount; i++) {
		float xx = fmod(i, columns) * editor->tileset->tilesize.x;
		float yy = floor(i / columns) * editor->tileset->tilesize.y;
		// int val = calc_mask(tileset->bitmask[i]);
		// TRACELOG("%d %d %d", i, tileset->bitmask[i][4], val);
		// if (val > -1) tic_graphics_fill_rectangle(xx, yy, tileset->size.x, tileset->size.y, tic_color(0, 0, 0, 125));
		int mask[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		tic_tileset_calc_mask_array(*editor->tileset, editor->tileset->bitmasks[i], mask);
		for (int j = 0; j < 9; j++) {
			float mx = xx + (fmod(j, 3) * mw);
			float my = yy + (floor(j / 3) * mh);

			if (mask[j] > 0) tic_graphics_fill_rectangle(mx, my, mw, mh, tic_color(128, 12, 83, 175));
		}
	}
	tic_canvas_detach();


	// int sz = 14 + TOOL_UUID_SIZE;
	// char nm[sz];
	// // if (!editor->name)
	// strcpy(nm, "window##tool_");
	// strcat(nm, editor->tool->uuid);
	// nm[sz-1] = '\0';

	// if (tic_ui_begin_window(nm, mu_rect(128, 64, 240, 260))) {
	// 		mu_Container *cnt = tic_ui_get_current_container();
	// 		if (!editor->cnt) editor->cnt = cnt;
	// 		if (editor->tool && !editor->tool->cnt) editor->tool->cnt = cnt;

		mu_Container *cnt = editor->tool->cnt;

		if (Core.ui.ctx->hover_root == cnt) {
			if (tic_input_key_down(KEY_LEFT_CONTROL)) {
				float y;
				tic_input_get_mouse_scroll_delta(NULL, &y);
				if (y) editor->scale += y/10.f;
				editor->scale = tic_clamp(editor->scale, 1, 10);
			}
		}

			tic_ui_layout_row(1, ((int[]){-1}), -1);
  		// mu_layout_row(Core.ui.ctx, 2, (int[]){64, -1}, -1);

  		// tic_ui_begin_column();
  		// tic_ui_number("scale", &editor->scale, 1);
  		// editor->scale = tic_clamp(editor->scale, 1, 10);
  		// if (tic_ui_button("save")) {
  		// 	tic_editor_tileset_save(editor);
  		// }
  		// tic_ui_end_column();

  		tic_ui_begin_column();
  		mu_layout_row(Core.ui.ctx, 1, (int[]){-1}, -1);
  		tic_ui_begin_panel("tileset_panel");

  		tc_Tileset *tileset = editor->tileset;

	  		mu_layout_row(Core.ui.ctx, 1, (int[]){tileset->image->width*editor->scale}, tileset->image->height*editor->scale);
	  		int tw = tileset->image->width / tileset->tilesize.x;
	  		int th = tileset->image->height / tileset->tilesize.y;
	  		if (tic_ui_image_grid_ex("tileset_grid", editor->canvas.texture, tic_rect(0, editor->canvas.height, editor->canvas.width, -editor->canvas.height), mu_vec2(tw, th), &editor->cell, 0)) {
	  			// memset(editor->mask, 0, sizeof(int) * 9);
	  			tic_tileset_calc_mask_array(*editor->tileset, tileset->bitmasks[editor->cell], editor->mask);
	  			tic_ui_open_popup("tile_bitmask");
	  		}


	  		tc_Tool *t = editor->tool;
	  		if (t && t->res) t->res->changed = tc_false;
	  		if (tic_ui_begin_popup("tile_bitmask")) {
	  			int r = 96;
	  			tic_ui_layout_row(1, &r, 96);
	  			// TRACELOG("%d %d", tileset->cell, tileset->bitmask[tileset->cell][1]);
	  			// int mask[9] = {0, 0, 0, 0, 0, 0, 0, 0};
	  			//if (tic_ui_grid_ex("bitmask_grid", mu_vec2(3, 3), editor->mask, MU_OPT_INPUTDOWN | MU_OPT_GRIDMULTI)) {
          // int index = tic_tileset_get_from_bitmask(*tileset, tileset->bitmasks[editor->cell]);
          tc_Rectf rect = tic_tileset_get_rect(*tileset, editor->cell);
          // TRACELOG("%f %f %f %f", rect.x, rect.h, rect.w, rect.y-rect.h);
          if (tic_ui_image_grid_ex("bitmask_grid", tileset->image->texture, tic_rect(rect.x, rect.y, rect.w, rect.h), mu_vec2(3, 3), editor->mask, MU_OPT_INPUTDOWN | MU_OPT_GRIDMULTI)) {
	  				tileset->bitmasks[editor->cell] = tic_tileset_calc_mask(*tileset, editor->mask);
	  				if (t && t->res) t->res->changed = tc_true;
	  			}
	  			mu_layout_row(Core.ui.ctx, 1, (int[]){-1}, 0);
	  			if (tic_ui_button("clear")) {
	  				memset(editor->mask, 0, sizeof(int) * 9);
	  				tileset->bitmasks[editor->cell] = -1;
	  				if (t && t->res) t->res->changed = tc_true;
	  			}
	  			res = 1;
	  			tic_ui_end_popup();
	  		} else {
	  			editor->cell = -1;
	  		}
	  	tic_ui_end_panel();
  		tic_ui_end_column();

  return res;
}

void tic_editor_tileset_save(tc_TilesetEditor *editor) {
	if (editor->tool && editor->tool->name[0]) {
		// TRACELOG("%s", editor->name);
		tic_resources_tileset_store(&Core.resources, editor->tool->res);
		return;
	}
	cJSON *root = NULL;
	if (editor->tool && editor->tool->filename[0]) root = tic_json_open(editor->tool->filename);
	else root = tic_json_create();

	tc_Tileset *tileset = editor->tileset;
	tic_json_set_number(root, "tilewidth", tileset->tilesize.x);
	tic_json_set_number(root, "tileheight", tileset->tilesize.y);
	tic_json_set_number(root, "imagewidth", tileset->image->width);
	tic_json_set_number(root, "imageheight", tileset->image->height);
	// if (editor->image_name) tic_json_set_string(root, "image", editor->image_name);
	tic_json_set_number(root, "columns", tileset->columns);
	tic_json_set_number(root, "tilecount", tileset->tilecount);

	// cJSON *bitmask = tic_json_create_array();
	// tic_json_set_object(root, "bitmasks", bitmask);
	cJSON *bitmask = tic_json_get_array(root, "bitmasks");
	if (bitmask) {
		for (int i = 0; i < tileset->tilecount; i++) {
			cJSON_DeleteItemFromArray(bitmask, 0);
		}
	} else {
		bitmask = tic_json_create_array();
		tic_json_set_array(root, "bitmasks", bitmask);
	}

	for (int i = 0; i < tileset->tilecount; i++) {
		cJSON *n = cJSON_CreateNumber(tileset->bitmasks[i]);
		cJSON_InsertItemInArray(bitmask, i, n);
	}

	// char val[25];
	// memset(val, 0, 25);
	// TRACELOG("%d", tileset->tilecount);

	if (editor->filename) tic_json_save(editor->filename, root);
	else tic_json_save("tileset.json", root);
	tic_json_delete(root);
}

/**********************
 * Tilemap
 **********************/

int tmap_editor_w = 16;
int tmap_editor_h = 16;

void *tic_editor_tilemap_create(tc_Resource *res) {
	tic_ui_textbox(res->name, sizeof(res->name));
	tic_ui_textbox(res->path, sizeof(res->path));


	tic_ui_button("ok##tilemap_editor");
}

void tic_editor_tilemap_init(tc_TilemapEditor *editor, tc_Tool *tool) {
	editor->tileset_name = NULL;
	// tic_editor_init_tool(editor->tool);
	// editor->tool->draw_panel = (void(*)(void*))&editor_tilemap_draw_panel;
	editor->tool = tool;
	// tool->fullscreen = tc_true;
	// tool->flags = MU_OPT_NORESIZE | MU_OPT_NOFRONT | MU_OPT_NOFRAME;
	editor->filename = NULL;
	editor->map = NULL;
	editor->cell = -1;
	editor->scale = 1;
	editor->canvas = tic_canvas_create(16*16, 16*16);
	editor->bg = tic_canvas_create(32, 32);
	tic_texture_set_wrap(&editor->bg.texture, GL_REPEAT, GL_REPEAT);
	editor->cnt = NULL;
	// editor->draw_panel = &editor_tilemap_draw_panel;
	editor->draw_panel = NULL;
	if (tool->res) tic_editor_tilemap_change(editor, tool->res->data);
}
void tic_editor_tilemap_destroy(tc_TilemapEditor *editor) {
	tic_canvas_destroy(&editor->canvas);
}

void tic_editor_tilemap_load(const char *path) {

}

void tic_editor_tilemap_change(tc_TilemapEditor *editor, tc_Tilemap *tilemap) {
	editor->map = tilemap;
	tic_canvas_destroy(&editor->canvas);
	editor->canvas = tic_canvas_create(tilemap->width*tilemap->grid.x, tilemap->height*tilemap->grid.y);
	editor->size.x = tilemap->width;
	editor->size.y = tilemap->height;
	// TRACELOG("teste");
}

int tic_editor_tilemap_draw(tc_TilemapEditor *editor) {
	int width,height;
	tic_window_get_size(&width, &height);
	char *title = "tilemap";
	int cw = editor->canvas.width;
	int ch = editor->canvas.height;

	tc_Resource *tres = editor->tool->res;
	list_iter_t iter = tres ? list_iter(&tres->deps) : (list_iter_t){0};
	tc_ResourceDep *dep = NULL;
	while((dep = list_next(&iter))) {
		if (dep->res->changed) tic_tilemap_update(editor->map);
	}

	// tic_canvas_attach(editor->bg);
	// tic_graphics_fill_rectangle(0, 0, 16, 16, tic_color3(102, 59, 147));
	// tic_graphics_fill_rectangle(16, 0, 16, 16, tic_color3(139, 114, 222));
	// tic_graphics_fill_rectangle(0, 16, 16, 16, tic_color3(139, 114, 222));
	// tic_graphics_fill_rectangle(16, 16, 16, 16, tic_color3(102, 59, 147));
	// tic_canvas_detach();
	editor_set_bg(editor->bg, 16, tic_color3(102, 59, 147), tic_color3(139, 114, 222));

	tic_canvas_attach(editor->canvas);
	tic_graphics_clear(tic_color(0, 0, 0, 0));
	tic_canvas_draw_part(editor->bg, tic_rectf(0, 0, cw, ch), 0, 0, WHITE);
	if (editor->map) tic_tilemap_draw(*editor->map);
	// tic_graphics_fill_rectangle(0, 0, 32, 32, WHITE);
	tic_canvas_detach();


	// tic_canvas_draw(editor->canvas, -32, 32, WHITE);

	// if (mu_begin_window_ex(Core.ui.ctx, title, mu_rect(0, 0, width, height), (MU_OPT_NOFRONT | MU_OPT_NOFRAME | MU_OPT_NORESIZE))) {
		// mu_Container *cnt = tic_ui_get_current_container();
		// if (editor->tool && !editor->tool->cnt) editor->tool->cnt = cnt;
	mu_Container *cnt = editor->tool->cnt;
		// mu_Id id = tic_ui_get_id(title, strlen(title));
		if (Core.ui.ctx->hover_root == cnt) {
			if (tic_input_key_down(KEY_LEFT_CONTROL)) {
				float y;
				tic_input_get_mouse_scroll_delta(NULL, &y);
				if (y) editor->scale += y/10.f;
				editor->scale = tic_clamp(editor->scale, 1, 10);
			}
		}

		// TRACELOG("%d %d", cw, ch);

		// tic_canvas_draw(editor->canvas, -190, 0, WHITE);

		// TRACELOG("%d %d", cw, ch);
		int tesw = editor->map ? editor->map->width * editor->map->grid.x : 16 * 16;
		int tesh = editor->map ? editor->map->height * editor->map->grid.y : 16 * 16;

		tic_ui_layout_row(1, ((int[]){tesw*editor->scale}), tesh*editor->scale);
		// mu_Container *container = tic_ui_get_current_container();
		// cnt->rect = tic_rect(Core.editor.sidebar_width, 0, Core.window.width-Core.editor.sidebar_width, Core.window.height);
		// cnt->rect = tic_rect(Core.editor.sidebar_width, 0, cw-Core.sidebar_width., ch);
		// cnt->body = tic_rect(Core.editor.sidebar_width, 0, Core.window.width-Core.editor.sidebar_width, Core.window.height);
		// cnt->rect = cnt->body;
		// cnt->rect.x = Core.editor.sidebar_width;
		// cw = cnt->rect.w;
		// ch = cnt->rect.h;
		int tw = cw / 16;
		int th = ch / 16;
		// tic_ui_image(editor->canvas.texture, mu_vec2(0, 0), mu_rect(0, ch, cw, -ch), WHITE);
		int res = tic_ui_image_grid_ex("tilemap_grid", editor->canvas.texture, mu_rect(0, ch, cw, -ch), mu_vec2(tw, th), &editor->cell, MU_OPT_INPUTDOWN);
		if (res & MU_RES_CHANGE) {
			// TRACELOG("%d", res & MU_RES_MOUSELEFT);
			if (res & MU_RES_MOUSELEFT) tic_tilemap_insert(editor->map, editor->cell);
			else if (res & MU_RES_MOUSERIGHT) tic_tilemap_remove(editor->map, editor->cell);
		}
		else editor->cell = -1;

	// 	tic_ui_end_window();
	// }
}

void tic_editor_tilemap_save(tc_TilemapEditor *editor) {
	cJSON *root = tic_json_create();
	tc_Tilemap *tilemap = editor->map;
	tic_json_set_number(root, "width", tilemap->width);
	tic_json_set_number(root, "height", tilemap->height);
	tic_json_set_number(root, "tilewidth", tilemap->grid.x);
	tic_json_set_number(root, "tileheight", tilemap->grid.y);
	if (editor->tileset_name) tic_json_set_string(root, "tileset", editor->tileset_name);
	tic_json_set_number(root, "count", tilemap->count);
	// tic_json_set_number(root, "tilecount", tileset->tilecount);

	cJSON *data = tic_json_create_array();
	// tic_json_set_object(root, "bitmasks", bitmask);
	tic_json_set_array(root, "data", data);

	// char val[25];
	// memset(val, 0, 25);
	// TRACELOG("%d", tileset->tilecount);
	for (int i = 0; i < tilemap->count; i++) {
		cJSON *n = cJSON_CreateNumber(tilemap->data[i]);
		cJSON_InsertItemInArray(data, i, n);
	}

	if (editor->filename) tic_json_save(editor->filename, root);
	else tic_json_save("tilemap.json", root);

	tic_json_delete(root);
}
