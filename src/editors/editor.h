#ifndef TC_EDITOR_H
#define TC_EDITOR_H

#include "../tinycoffee.h"

#ifndef TCDEF
  #define TCDEF extern
#endif

typedef struct {
  struct {
    tc_texture current;
    float width;
    float height;
    float scale;
    tc_uint8 speed;
  } sprite;

  struct {
    int sprite;
    int console;
    int tileset;
    int tilemap;
  } window;

  tc_uint8 log[48000];
} tc_editor;

TCDEF void tc_editor_init();

TCDEF void tc_open_sprite(tc_texture tex);
TCDEF void tc_sprite_editor_window();
TCDEF void tc_console_window();

TCDEF void tc_editor_write_log(const tc_uint8 *log);
TCDEF void tc_editor_clear_log();

TCDEF void tc_editor_draw();

#endif // TC_EDITOR_H

#if defined(TC_EDITOR_IMPLEMENTATION)

static tc_editor editor;

TCDEF void tc_editor_init() {
  editor.window.sprite = 1;
  editor.window.console = 1;

  editor.sprite.width = 32;
  editor.sprite.height = 32;
  editor.sprite.scale = 2;
}

TCDEF void tc_open_sprite(tc_texture tex) {
  editor.sprite.current = tex;
}

TCDEF void tc_sprite_editor_window() {
  mu_Container *cnt = tc_ui_get_container("sprite");
  cnt->open = 1;
  if (tc_ui_begin_window("sprite", mu_rect(120, 0, 250, 120))) {
    int sw = mu_get_current_container(ui.ctx)->body.w * 0.22;
    int w[4] = {sw, sw, sw, sw};
    tc_ui_layout_row(4, w, 0);
    tc_ui_label("width");
    tc_ui_number(&editor.sprite.width, 1);
    tc_ui_label("height");
    tc_ui_number(&editor.sprite.height, 1);
    tc_ui_layout_row(1, (int[]){-1}, 0);
    tc_ui_number(&editor.sprite.scale, 1);
    tc_texture tex = editor.sprite.current;
    float width = editor.sprite.width;
    float height = editor.sprite.height;
    float scale = editor.sprite.scale;
    tc_recti part = {0, 0, width * scale, height * scale};
    mu_Vec2 size = {tex.width * scale, tex.height * scale};
    tc_ui_image(tex.id, size, part, WHITE);
//     tc_ui_image(tex.id, size, part, WHITE);
    if (tc_ui_header("animations")) {
      tc_ui_layout_row(4, w, 0);
      tc_ui_text("idle");
      tc_uint8 buf[20];
      tc_ui_textbox(buf, 20);
      tc_ui_text("idle");
      tc_ui_text("idle");
    }
    tc_ui_end_window();
  }
}

TCDEF void tc_debug_window() {
  if (tc_ui_begin_window_ex("debug", mu_rect(25, 40, 120, 150), MU_OPT_NOCLOSE)) {
    tc_ui_layout_row(1, (int[]) { -1 }, 0);
    tc_uint8 fps[15];
    tc_uint8 delta[15];
    sprintf(fps, "FPS: %d", tc_get_fps());
    sprintf(delta, "delta: %.5f", tc_get_delta());
    tc_ui_text(fps);
    tc_ui_text(delta);
    if (tc_ui_header("editors")) {
      tc_ui_checkbox("sprite", &editor.window.sprite);
      tc_ui_checkbox("console", &editor.window.console);
    }
    tc_ui_end_window();
  }
}

TCDEF void tc_console_window() {
  if (tc_ui_begin_window("log", mu_rect(240, 250, 250, 120))) {
    tc_ui_layout_row(1, (int[]) { -1 }, -25);
    tc_ui_begin_panel("log output");
    tc_ui_layout_row(1, (int[]){-1}, -1);
    tc_ui_text(editor.log);
    tc_ui_end_panel();
    tc_ui_end_window();
  }
}

TCDEF void tc_editor_write_log(const tc_uint8 *log) {
  if (editor.log[0]) strcat(editor.log, "\n");
  strcat(editor.log, log);
}

TCDEF void tc_editor_clear_log() {
  editor.log[0] = '\0';
}

TCDEF void tc_editor_draw() {
  tc_debug_window();
  if (editor.window.sprite) tc_sprite_editor_window();
  if (editor.window.console) tc_console_window();
}

#endif
