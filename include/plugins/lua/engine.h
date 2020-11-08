#include "../../tico.h"


#if defined(TICO_LUA_IMPLEMENTATION)

/******************
 * Tileset
 ******************/

static int tico_lua_tileset_new(lua_State *L) {
	// tc_Tileset *tileset = lua_newuserdata(L, sizeof(*tileset));
	// luaL_setmetatable(L, TILESET_CLASS);

	// tc_Image *image = luaL_checkudata(L, 1, IMAGE_CLASS);
	// int width = luaL_optinteger(L, 2, 16);
	// int height = luaL_optinteger(L, 3, 16);

	// *tileset = tico_tileset_create(image, width, height);

	if (lua_type(L, 1) == LUA_TTABLE) {
		cJSON *json = tico_lua_json_new_cjson_object(L, 1);
		tc_Tileset *tileset = NULL;
		if (json) {
			// TRACELOG("teste");
			tileset = lua_newuserdata(L, sizeof(*tileset));
			luaL_setmetatable(L, TILESET_CLASS);

			int top = lua_gettop(L);
			tico_tileset_from_json(tileset, json);
			lua_settop(L, top);
			tico_json_delete(json);
		} else {
			lua_pushnil(L);
		}

		return 1;
	}

	// tc_Tileset *tileset = luaL_checkudata(L, 1, TILESET_CLASS);
	// int w = luaL_optinteger(L, 2, 16);
	// int h = luaL_optinteger(L, 3, 16);

	// tc_Tilemap *map = lua_newuserdata(L, sizeof(*map));
	// luaL_setmetatable(L, TILEMAP_CLASS);

	// *map = tico_tilemap_create(tileset, w, h);

	tc_Image *image = luaL_checkudata(L, 1, IMAGE_CLASS);
	int width = luaL_optinteger(L, 2, 16);
	int height = luaL_optinteger(L, 3, 16);

	tc_Tileset *tileset = lua_newuserdata(L, sizeof(*tileset));
	luaL_setmetatable(L, TILESET_CLASS);

	*tileset = tico_tileset_create(image, width, height);

	return 1;
}

static int tico_lua_tileset_load(lua_State *L) {
	tc_Tileset *tileset = lua_newuserdata(L, sizeof(*tileset));
	luaL_setmetatable(L, TILESET_CLASS);

	const char *path = luaL_checkstring(L, 1);
	tc_Image *image = luaL_checkudata(L, 2, IMAGE_CLASS);
	*tileset = tico_tileset_load(path, image);

	return 1;
}

static int tico_lua_tileset__gc(lua_State *L) {
	tc_Tileset *tileset = luaL_checkudata(L, 1, TILESET_CLASS);

	tico_tileset_destroy(tileset);
	return 0;
}

static int tico_lua_tileset_draw(lua_State *L) {
	tc_Tileset *tileset = luaL_checkudata(L, 1, TILESET_CLASS);
	int index = luaL_optinteger(L, 2, 0);
	int x = luaL_optinteger(L, 3, 0);
	int y = luaL_optinteger(L, 4, 0);
	tc_Color color = lua_optcolor(L, 5, WHITE);

	tico_tileset_draw(*tileset, index, x, y, color);

	return 0;
}

static int tico_lua_tileset_get_rect(lua_State *L) {
	tc_Tileset *tileset = luaL_checkudata(L, 1, TILESET_CLASS);
	int index = luaL_optinteger(L, 2, 0);

	// int index = tico_tileset_get_from_bitmask(*tileset, bitmask);
	tc_Rectf rect = tico_tileset_get_rect(*tileset, index);
	lua_newtable(L);
	for (int i = 0; i < 4; i++) {
		lua_pushinteger(L, rect.data[i]);
		lua_rawseti(L, -2, i);
	}

	return 1;
}

static int tico_lua_tileset_from_bitmask(lua_State *L) {
	tc_Tileset *tileset = luaL_checkudata(L, 1, TILESET_CLASS);
	int bitmask = luaL_optinteger(L, 2, -1);

	int index = tico_tileset_get_from_bitmask(*tileset, bitmask);

	lua_pushinteger(L, index);
	return 1;
}

static int tico_lua_tileset_get_rect_from_bitmask(lua_State *L) {
	tc_Tileset *tileset = luaL_checkudata(L, 1, TILESET_CLASS);
	return 0;
}

int luaopen_tileset(lua_State *L) {
	luaL_Reg reg[] = {
		{"__call", tico_lua_tileset_new},
		{"draw", tico_lua_tileset_draw},
		{"get_rect", tico_lua_tileset_get_rect},
		{"from_bitmask", tico_lua_tileset_from_bitmask},
		// {"get_rect_from_bitmask", },
		{"__gc", tico_lua_tileset__gc},
		{NULL, NULL}
	};

	// luaL_newlib(L, reg);

	luaL_newmetatable(L, TILESET_CLASS);
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

	return 1;
}

/**************************
 * Tilemap
 **************************/

static int tico_lua_tilemap_new(lua_State *L) {
	// tc_Tilemap *map = lua_newuserdata(L, sizeof(*map));
	// luaL_setmetatable(L, TILEMAP_CLASS);

	if (lua_type(L, 1) == LUA_TTABLE) {
		cJSON *json = tico_lua_json_new_cjson_object(L, 1);
		tc_Tilemap *map = NULL;
		if (json) {
			// TRACELOG("teste");
			map = lua_newuserdata(L, sizeof(*map));
			luaL_setmetatable(L, TILEMAP_CLASS);

			int top = lua_gettop(L);
			tico_tilemap_from_json(map, json);
			lua_settop(L, top);
			tico_json_delete(json);
			// TRACELOG("testandow");
		} else {
			lua_pushnil(L);
		}

		return 1;
	}

	tc_Tileset *tileset = luaL_checkudata(L, 1, TILESET_CLASS);
	int w = luaL_optinteger(L, 2, 16);
	int h = luaL_optinteger(L, 3, 16);

	tc_Tilemap *map = lua_newuserdata(L, sizeof(*map));
	luaL_setmetatable(L, TILEMAP_CLASS);

	*map = tico_tilemap_create(tileset, w, h);

	return 1;
}

static int tico_lua_tilemap_load(lua_State *L) {
	tc_Tilemap *map = lua_newuserdata(L, sizeof(*map));
	luaL_setmetatable(L, TILEMAP_CLASS);

	const char *path = luaL_checkstring(L, 1);
	tc_Tileset *tileset = luaL_checkudata(L, 2, TILESET_CLASS);

	*map = tico_tilemap_load(path, tileset);

	return 1;
}

static int tico_lua_tilemap_draw(lua_State *L) {
	tc_Tilemap *map = luaL_checkudata(L, 1, TILEMAP_CLASS);

	tico_tilemap_draw(*map);

	return 0;
}

static int tico_lua_tilemap_update(lua_State *L) {
	tc_Tilemap *map = luaL_checkudata(L, 1, TILEMAP_CLASS);

	for (int i = 0; i < map->count; i++) {
		if (map->data[i] >= 0) map->data[i] = tico_tilemap_autotile(*map, i);
	}

	return 0;
}

static int tico_lua_tilemap_insert(lua_State *L) {
	tc_Tilemap *map = luaL_checkudata(L, 1, TILEMAP_CLASS);

	int index = luaL_optinteger(L, 2, -1);
	tico_tilemap_insert(map, index);
	// TRACELOG("%d", map->data[index]);

	return 0;
}

static int tico_lua_tilemap_has_tile(lua_State *L) {
	tc_Tilemap *map = luaL_checkudata(L, 1, TILEMAP_CLASS);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	int has = tico_tilemap_has_tile(*map, x, y);

	lua_pushboolean(L, has);

	return 1;
}

int luaopen_tilemap(lua_State *L) {
	luaL_Reg reg[] = {
		{"__call", tico_lua_tilemap_new},
		{"update", tico_lua_tilemap_update},
		{"draw", tico_lua_tilemap_draw},
		{"insert", tico_lua_tilemap_insert},
		{"has_tile", tico_lua_tilemap_has_tile},
		{NULL, NULL}
	};

	luaL_newmetatable(L, TILEMAP_CLASS);
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  return 1;
}

/*********************
 * Camera
 *********************/

static int tico_lua_camera_new(lua_State *L) {

	float x = luaL_optnumber(L, 1, 0);
	float y = luaL_optnumber(L, 2, 0);
	float w = luaL_optnumber(L, 3, Core.window.width);
	float h = luaL_optnumber(L, 4, Core.window.height);

	tc_Camera *camera = lua_newuserdata(L, sizeof(*camera));
	luaL_setmetatable(L, CAMERA_CLASS);

	*camera = tico_camera_create(x, y, w, h);

	return 1;
}

static int tico_lua_camera_attach(lua_State *L) {
	tc_Camera *camera = luaL_checkudata(L, 1, CAMERA_CLASS);

	tico_camera_attach(*camera);

	return 0;
}

static int tico_lua_camera_detach(lua_State *L) {
	tc_Camera *camera = luaL_checkudata(L, 1, CAMERA_CLASS);
	tico_camera_detach();

	return 0;
}

static int tico_lua_camera_move(lua_State *L) {
	tc_Camera *camera = luaL_checkudata(L, 1, CAMERA_CLASS);

	float x = luaL_optnumber(L, 2, camera->area.x);
	float y = luaL_optnumber(L, 3, camera->area.y);

	camera->area.x = x;
	camera->area.y = y;

	return 0;
}

int luaopen_camera(lua_State *L) {
	luaL_Reg reg[] = {
		{"__call", tico_lua_camera_new},
		{"attach", tico_lua_camera_attach},
		{"detach", tico_lua_camera_detach},
		{"move", tico_lua_camera_move},
		{NULL, NULL}
	};

	luaL_newmetatable(L, CAMERA_CLASS);
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  return 1;
}

/**********************
 * Sprite
 **********************/

static int tico_lua_sprite_new(lua_State *L) {
	if (lua_type(L, 1) == LUA_TTABLE) {
		cJSON *json = tico_lua_json_new_cjson_object(L, 1);
		tc_Sprite *sprite = NULL;
		if (json) {
			// TRACELOG("teste");
			sprite = lua_newuserdata(L, sizeof(*sprite));
			luaL_setmetatable(L, SPRITE_CLASS);

			tico_sprite_from_json(sprite, json);
		} else {
			lua_pushnil(L);
		}

		return 1;
	}
	tc_Image *image = luaL_checkudata(L, 1, IMAGE_CLASS);
	int w = luaL_checknumber(L, 2);
	int h = luaL_checknumber(L, 3);

	tc_Sprite *sprite = lua_newuserdata(L, sizeof(*sprite));
	luaL_setmetatable(L, SPRITE_CLASS);

	*sprite = tico_sprite_create(image, w, h);
	

	return 1;
}

static int tico_lua_sprite_update(lua_State *L) {
	tc_Sprite *sprite = luaL_checkudata(L, 1, SPRITE_CLASS);

	tico_sprite_update(sprite);

	return 0;
}

static int tico_lua_sprite_draw(lua_State *L) {
	int index = 0;
	tc_Sprite *sprite = luaL_checkudata(L, ++index, SPRITE_CLASS);
  tc_Color color = WHITE;

  float x = luaL_optnumber(L, ++index, 0);
  float y = luaL_optnumber(L, ++index, 0);
  if (lua_type(L, index+1) != LUA_TNUMBER) {
    color = lua_optcolor(L, ++index, WHITE);
    tico_sprite_draw(*sprite, x, y, color);
    return 0;
  }
  float angle = luaL_optnumber(L, ++index, 0);
  float sx = luaL_optnumber(L, ++index, 1);
  float sy = luaL_optnumber(L, ++index, 1);
  float cx = luaL_optnumber(L, ++index, 0);
  float cy = luaL_optnumber(L, ++index, 0);
  color = lua_optcolor(L, ++index, WHITE);
  // TRACELOG("%p", sprite);
  tico_sprite_draw_ex(*sprite, x, y, angle, sx, sy, cx, cy, color);

	return 0;
}

static int tico_lua_sprite_add_animation(lua_State *L) {
	tc_Sprite *sprite = luaL_checkudata(L, 1, SPRITE_CLASS);
	const char *name = luaL_checkstring(L, 2);
	int from = luaL_checknumber(L, 3);
	int to = luaL_checknumber(L, 4);

	tico_sprite_add_animation(sprite, name, from, to);

	return 0;
}

static int tico_lua_sprite_play(lua_State *L) {
	tc_Sprite *sprite = luaL_checkudata(L, 1, SPRITE_CLASS);
	const char *name = luaL_checkstring(L, 2);

	tico_sprite_play(sprite, name);

	return 0;
}

static int tico_lua_sprite_stop(lua_State *L) {
	tc_Sprite *sprite = luaL_checkudata(L, 1, SPRITE_CLASS);

	tico_sprite_stop(sprite);

	return 0;
}

static int tico_lua_sprite_get_animation(lua_State *L) {
	tc_Sprite *sprite = luaL_checkudata(L, 1, SPRITE_CLASS);

	const char *name = tico_sprite_get_animation(sprite);

	lua_pushstring(L, name);

	return 1;
}

int luaopen_sprite(lua_State *L) {
	luaL_Reg reg[] = {
		{"__call", tico_lua_sprite_new},
		{"update", tico_lua_sprite_update},
		{"draw", tico_lua_sprite_draw},
		{"add", tico_lua_sprite_add_animation},
		{"play", tico_lua_sprite_play},
		{"stop", tico_lua_sprite_stop},
		{"getAnimation", tico_lua_sprite_get_animation},
		{NULL, NULL}
	};

	luaL_newmetatable(L, SPRITE_CLASS);
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  // TRACELOG("%s", SPRITE_CLASS);
  return 1;
}

int luaopen_engine(lua_State *L) {
	luaL_Reg reg[] = {
		{"newSprite", tico_lua_sprite_new},
		{"newCamera", tico_lua_camera_new},
		{"newTileset", tico_lua_tileset_new},
		{"loadLileset", tico_lua_tileset_load},
		{"newTilemap", tico_lua_tilemap_new},
		{"loadTilemap", tico_lua_tilemap_load},
		{NULL, NULL}
	};

	luaL_newlib(L, reg);

	return 1;
}

#endif
