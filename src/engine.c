#include "tico.h"

tc_Tileset tic_tileset_create(tc_Image *image, int w, int h) {
	tc_Tileset tileset = {0};
	tileset.tilesize = tic_vec2_new(w, h);
	tileset.image = image;
	int sx = tileset.image->width / w;
	int sy = tileset.image->height / h;
	tileset.tilecount = sx * sy;

	tileset.columns = sx;
	tileset.bitmasks = malloc(sizeof(int) * tileset.tilecount);
	memset(tileset.bitmasks, -1, sizeof(int) * tileset.tilecount);

	return tileset;
}

tc_Tileset tic_tileset_load(const char *path, tc_Image *image) {
	tc_Tileset tileset = {0};

	cJSON *root = tic_json_open(path);
	// const char *filename = tic_json_get_string(root, "image");
	int tilew = tic_json_get_number(root, "tilewidth");
	int tileh = tic_json_get_number(root, "tileheight");
	// const char *image_name = tic_json_get_string(root, "image");
	tc_Resource *resource = NULL;
	// TRACELOG("%s", image_name);
	// if (image_name) resource = tic_resources_get(&Core.resources, "image", image_name);
	// TRACELOG("%p", resource);
	// if (!resource) tileset = tic_tileset_create(image, tilew, tileh);
	tileset = tic_tileset_create(image, tilew, tileh);
	cJSON *bitmask = tic_json_get_array(root, "bitmasks");
	cJSON *el;
	int i = 0;
	cJSON_ArrayForEach(el, bitmask) {
		tileset.bitmasks[i] = el->valueint;
		// TRACELOG("%d %d", i, el->valueint);
		i++;
	}

	tic_json_delete(root);

	return tileset;
}

void tic_tileset_destroy(tc_Tileset *tileset) {
	free(tileset->bitmasks);
}

void tic_tileset_draw(tc_Tileset tileset, int index, int x, int y, tc_Color color) {
	int xx = fmod(index, tileset.columns) * tileset.tilesize.x;
	int yy = floor(index / tileset.columns) * tileset.tilesize.y;

	tc_Rectf rect = tic_rectf(xx, yy, tileset.tilesize.x, tileset.tilesize.y);

	tic_image_draw_part(*tileset.image, rect, x, y, color);
}

void tic_tileset_calc_mask_array(tc_Tileset tileset, int bitmask, int *bitmask_array) {
	int off = 0;
	int val = -1;

	if (bitmask > -1) bitmask_array[4] = 1;
	else {
		bitmask_array[4] = -1;
		bitmask = 0;
	}

	for (int i = 0; i < 9; i++) {
		if (i == 4) i++;
		bitmask_array[i] = bitmask & 0x1;
		bitmask >>= 1;
	}
}

int tic_tileset_calc_mask(tc_Tileset tileset, int *bitmask_array) {
	int off = 0;
	int val = -1;
	if (bitmask_array[4] > 0) val = 0;

	for (int i = 0; i < 9; i++) {
		if (i == 4) {
			off = 1;
			continue;
		}
		if (bitmask_array[i] > 0) {
			if (val == -1) val = 0;
			val += 1 << (i-off);
		}
	}

	return val;
}

int tic_tileset_get_from_bitmask(tc_Tileset tileset, int bitmask) {
	int bm;
	for (int i = 0; i < tileset.tilecount; i++) {
		bm = tileset.bitmasks[i];
		// TRACELOG("%d %d", i, bm);
		if (bm == bitmask) {
			return i;
		}
	}
	return 0;
}

tc_Rectf tic_tileset_get_rect(tc_Tileset tileset, int index) {
	tc_Rectf rect = tic_rectf(0, 0, tileset.tilesize.x, tileset.tilesize.y);
	if (index < 0 && index >= tileset.tilecount) return rect;

	rect.x = fmod(index, tileset.columns) * tileset.tilesize.x;
	rect.y = floor(index / tileset.columns) * tileset.tilesize.y;

	return rect;
}

/***********************
 * Tilemap
 ***********************/

tc_Tilemap tic_tilemap_create(tc_Tileset *tileset, int w, int h) {
	tc_Tilemap map = {0};
	if (w <= 0) w = 1;
	if (h <= 0) h = 1;
	int data_size = sizeof(int) * w * h;
	map.data = malloc(data_size);
	memset(map.data, -1, data_size);

	map.width = w;
	map.height = h;
	map.tileset = tileset;
	map.grid.x = map.tileset->tilesize.x;
	map.grid.y = map.tileset->tilesize.y;
	map.count = w * h;

	return map;
}

tc_Tilemap tic_tilemap_load(const char *path, tc_Tileset *tileset) {
	tc_Tilemap map = {0};

	cJSON *root = tic_json_open(path);
	// const char *filename = tic_json_get_string(root, "image");
	int width = tic_json_get_number(root, "width");
	int height = tic_json_get_number(root, "height");
	map = tic_tilemap_create(tileset, width, height);

	cJSON *data = tic_json_get_array(root, "data");
	cJSON *el;
	int i = 0;
	cJSON_ArrayForEach(el, data) {
		map.data[i] = el->valueint;
		// TRACELOG("%d %d", i, el->valueint);
		i++;
	}

	tic_json_delete(root);

	return map;
}

void tic_tilemap_resize(tc_Tilemap *tilemap, int w, int h) {
	if (w <= 0 || h <= 0) return;
	int count = tilemap->count;
	int old_data[count];
	memcpy(old_data, tilemap->data, sizeof(int) * count);

	int n_count = w*h;
	int min_count = tic_min(count, n_count);
	int min_width = tic_min(w, tilemap->width);

	tilemap->data = TIC_REALLOC(tilemap->data, sizeof(int) * n_count);
	memset(tilemap->data, -1, sizeof(int) * n_count);
	for (int i = 0; i < min_count; i++) {
		int ox = fmod(i, min_width);
		int oy = floor(i / min_width) * tilemap->width;
		int ny = floor(i / min_width) * w;

		int oindex = ox + oy;
		int nindex = ox + ny;
		if (oindex >= count || nindex >= n_count) break;
		tilemap->data[nindex] = old_data[oindex];
	}

	tilemap->width = w;
	tilemap->height = h;
	tilemap->count = n_count;
}


void tic_tilemap_destroy(tc_Tilemap *tilemap) {
	free(tilemap->data);
}

void tic_tilemap_draw(tc_Tilemap tilemap) {
	for (int i = 0; i < tilemap.count; i++) {
		int x = fmod(i, tilemap.width) * tilemap.tileset->tilesize.x;
		int y = floor(i / tilemap.width) * tilemap.tileset->tilesize.y;
		tic_tileset_draw(*tilemap.tileset, tilemap.data[i], x, y, WHITE);
	}
}

int tic_tilemap_has_tile(tc_Tilemap map, int x, int y) {
	int index = tic_tilemap_get_index(map, x, y);
	if (index < 0) return 0; 
	return map.data[index] >= 0;
}

void tic_tilemap_update(tc_Tilemap *map) {
	for (int i = 0; i < map->count; i++) {
		int tile = tic_tilemap_autotile(*map, i);
		if (map->data[i] >= 0) map->data[i] = tile;
	}
}

int tic_tilemap_autotile(tc_Tilemap map, int index) {
	if (index < 0 || index > (map.width*map.height)) return -1;
	// if (map.data[index] < 0) return -1;

	tc_Vec2 pos;
	tic_tilemap_get_position(map, index, &pos);
	int sides[4] = {0, 0, 0, 0};
	int mask[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	mask[4] = 1;

	sides[0] = tic_tilemap_has_tile(map, pos.x, pos.y - 1);
	sides[1] = tic_tilemap_has_tile(map, pos.x + 1, pos.y);
	sides[2] = tic_tilemap_has_tile(map, pos.x, pos.y + 1);
	sides[3] = tic_tilemap_has_tile(map, pos.x - 1, pos.y);

	mask[1] = sides[0];
	mask[5] = sides[1];
	mask[7] = sides[2];
	mask[3] = sides[3];

	mask[0] = tic_tilemap_has_tile(map, pos.x-1, pos.y-1) && sides[0] && sides[3];
	mask[2] = tic_tilemap_has_tile(map, pos.x+1, pos.y-1) && sides[0] && sides[1];
	mask[6] = tic_tilemap_has_tile(map, pos.x-1, pos.y+1) && sides[2] && sides[3];
	mask[8] = tic_tilemap_has_tile(map, pos.x+1, pos.y+1) && sides[2] && sides[1];

	int bm = tic_tileset_calc_mask(*map.tileset, mask);
	// TRACELOG("%d %d", index, bm);
	return tic_tileset_get_from_bitmask(*map.tileset, bm);
}

void tic_tilemap_insert(tc_Tilemap *map, int index) {
	if (index < 0 || index >= map->width*map->height) return;
	map->data[index] = 1;
	map->data[index] = tic_tilemap_autotile(*map, index);
	// TRACELOG("%d", index);
	// TRACELOG("%d", map->data[index]);

	tc_Vec2 pos;
	tic_tilemap_get_position(*map, index, &pos);
	// TRACELOG("%d %d", index, map->data[index]);

	mu_Vec2 ad[8] = {
		{ 0, 1},
		{ 0,-1},
		{ 1, 0},
		{-1, 0},
		{-1,-1},
		{	1,-1},
		{-1, 1},
		{	1, 1}
	};

	for (int i = 0; i < 8; i++) {
		int n_index = tic_tilemap_get_index(*map, pos.x+ad[i].x, pos.y+ad[i].y);
		// TRACELOG("%d index: %d", i, n_index);
		if (n_index >= 0 && map->data[n_index] >= 0) map->data[n_index] = tic_tilemap_autotile(*map, n_index);
	}
}

void tic_tilemap_remove(tc_Tilemap *map, int index) {
	if (index < 0 || index >= map->width*map->height) return;
	map->data[index] = -1;
	// TRACELOG("%d", map->data[index]);

	tc_Vec2 pos;
	tic_tilemap_get_position(*map, index, &pos);
	// TRACELOG("%d %d", index, map->data[index]);

	mu_Vec2 ad[8] = {
		{ 0, 1},
		{ 0,-1},
		{ 1, 0},
		{-1, 0},
		{-1,-1},
		{	1,-1},
		{-1, 1},
		{	1, 1}
	};

	for (int i = 0; i < 8; i++) {
		int n_index = tic_tilemap_get_index(*map, pos.x+ad[i].x, pos.y+ad[i].y);
		// TRACELOG("%d index: %d", i, n_index);
		if (n_index >= 0 && map->data[n_index] >= 0) map->data[n_index] = tic_tilemap_autotile(*map, n_index);
	}
}

void tic_tilemap_get_position(tc_Tilemap map, int index, tc_Vec2 *pos) {
	int x = fmod(index, map.width);
	int y = floor(index / map.width);
	pos->x = x;
	pos->y = y;
}

int tic_tilemap_get_index(tc_Tilemap map, int x, int y) {
	int index = 0;
	if (x >= 0 && x < map.width &&
		  y >= 0 && y < map.height) {
		index = x + (y * map.width);
	}

	return index;
}

/**************************
 * Camera
 **************************/

tc_Camera tic_camera_create(float x, float y, float w, float h) {
	tc_Camera camera = {0};
	camera.area = tic_rectf(x, y, w, h);
	camera.size = (tc_Vec2){1, 1};
	camera.offset = (tc_Vec2){0, 0};
	camera.angle = 0;

	return camera;
}

void tic_camera_attach(tc_Camera camera) {
	tic_graphics_push();
	tic_graphics_translate(camera.offset.x, camera.offset.y);
	tic_graphics_scale(camera.size.x, camera.size.y);
	tic_graphics_rotate(camera.angle);
	tic_graphics_translate(camera.area.x, camera.area.y);
	// TRACELOG("%f %f", camera.area.x, camera.area.y);
	tic_graphics_scissor(0, 0, camera.area.w, camera.area.h);
}

void tic_camera_detach() {
	tic_graphics_pop();
	tc_Canvas canvas = Core.render.state.currentCanvas;
	if (canvas.id != 0) tic_graphics_scissor(0, 0, canvas.width, canvas.height);
	else tic_graphics_scissor(0, 0, Core.window.width, Core.window.height);
}

// void tic_camera_set_scale(tc_Camera *camera, float x, float y) {}

// void tic_camera_set_size(tc_Camera *camera, float w, float h) {}

/******************
 * Sprite
 ******************/

tc_Sprite tic_sprite_create(tc_Image *image, int w, int h) {
	tc_Sprite spr = {0};
}