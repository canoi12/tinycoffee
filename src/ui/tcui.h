#ifndef TC_UI_H
#define TC_UI_H

// #include "../tinycoffee.h"
#include "microui.h"

#ifndef TCDEF
  #define TCDEF
#endif

TCDEF void tc_ui_init();
TCDEF void tc_ui_begin();
TCDEF void tc_ui_end();
TCDEF void tc_ui_loadstate();
TCDEF void tc_ui_savestate();

TCDEF void tc_ui_mouse_pos_callback(double posX, double posY);
TCDEF void tc_ui_mouse_btn_callback(int button, int x, int y, int action);
TCDEF void tc_ui_mouse_scroll_callback(double xoff, double yoff);

TCDEF void tc_ui_key_callback(int key, int action);
TCDEF void tc_ui_text_callback(int codepoint);

TCDEF int tc_ui_text_width(mu_Font font, const char *text, int len);
TCDEF int tc_ui_text_height(mu_Font font);

TCDEF void tc_draw_gui();

typedef struct {
  mu_Context *ctx;
  tc_font font;
  struct {
    tc_texture tex;
    tc_rect rect[4];
  } icons;
  struct {
    int *animation;
    int *tileset;
    int *tilemap;
  } windows;
  unsigned char ** names;
} tc_ui;

extern tc_ui ui;

#endif // TC_UI_H

#if defined(TC_UI_IMPLEMENTATION)

tc_ui ui;

TCDEF void tc_ui_init(tc_font font) {
  ui.ctx = malloc(sizeof(mu_Context));
  ui.font = font;
//   ui.log[0] = '\0';
  ui.icons.tex = tc_load_texture("assets/icons.png");
  tc_rect rect[4] = {
    rect(0, 0, 8, 8),
    rect(8, 0, 8, 8),
    rect(0, 8, 8, 8),
    rect(8, 8, 8, 8)
  };
  memcpy(&ui.icons.rect, &rect, sizeof(rect));
  ui.names = malloc(MU_CONTAINERPOOL_SIZE * sizeof(char**));
//   ui.showWindow = 0;

  mu_init(ui.ctx);
  ui.ctx->text_width = tc_ui_text_width;
  ui.ctx->text_height = tc_ui_text_height;
}

TCDEF int tc_ui_text_width(mu_Font font, const char *text, int len) {
  if (len == -1) len = strlen(text);
  return tc_font_get_text_width(ui.font, text, len);
}

TCDEF int tc_ui_text_height(mu_Font font) {
  return ui.font.texture.height;
}

TCDEF void tc_ui_mouse_pos_callback(double posX, double posY) {
  mu_input_mousemove(ui.ctx, posX, posY);
}

TCDEF void tc_ui_mouse_btn_callback(int button, int x, int y, int action) {
  if (action == GLFW_PRESS) mu_input_mousedown(ui.ctx, CORE.input.mouseState.x, CORE.input.mouseState.y, 1 << button);
	else if (action == GLFW_RELEASE) mu_input_mouseup(ui.ctx, x, y, 1 << button);
}

TCDEF void tc_ui_mouse_scroll_callback(double xoff, double yoff) {
  mu_input_scroll(ui.ctx, xoff*-10, yoff*-10);
}

TCDEF void tc_ui_key_callback(int key, int action) {
  if (action == GLFW_PRESS) mu_input_keydown(ui.ctx, key);
  else if (action == GLFW_RELEASE) mu_input_keyup(ui.ctx, key);
}

TCDEF void tc_ui_text_callback(int codepoint) {
  tc_uint8 c[4];
  tc_utf8_encode(c, codepoint);
  mu_input_text(CORE.muCtx, c);
}

TCDEF void tc_ui_begin() {
  mu_begin(ui.ctx);
}

static void tc_mu_draw_icon(int id, tc_recti rect, tc_color color) {
  tc_color col = rgba(color.r, color.g, color.b, color.a);
  tc_rect r;
  for (int i = 0; i < 4; i++) r.data[i] = ui.icons.rect[id-1].data[i];
  tc_draw_texture_part_scale(CORE.icons.tex, r, rect.x+1, rect.y+3, 2, 2, col);
}

static void tc_mu_draw_image(int id, tc_recti rect, tc_recti src, tc_recti part, tc_color color) {
  tc_color col = rgba(color.r, color.g, color.b, color.a);
  tc_rect r = {part.x, part.y, part.w, part.h};
//   TRACELOG("%d %d %d %d", r.x, r.y, r.width, r.height);
  tc_render_draw_quad(&CORE.render, id, r, rect.x, rect.y, src.w, src.h, col);
}

TCDEF void tc_ui_end() {
  mu_end(ui.ctx);
  mu_Command *cmd = NULL;
  while(mu_next_command(ui.ctx, &cmd)) {
    switch(cmd->type) {
      case MU_COMMAND_TEXT: {
//         tc_color col = rgba(cmd->text.color.r, cmd->text.color.g, cmd->text.color.b, cmd->text.color.a);
        tc_draw_text(cmd->text.str, cmd->text.pos.x, cmd->text.pos.y, cmd->text.color);
        break;
      }
      case MU_COMMAND_RECT:
      {
//         tc_color col = rgba(cmd->rect.color.r, cmd->rect.color.g, cmd->rect.color.b, cmd->rect.color.a);
        tc_fill_rectangle(cmd->rect.rect.x, cmd->rect.rect.y, cmd->rect.rect.w, cmd->rect.rect.h, cmd->text.color);
        break;
      }
      case MU_COMMAND_CLIP: tc_scissor(cmd->clip.rect.x, cmd->clip.rect.y, cmd->clip.rect.w, cmd->clip.rect.h); break;
      case MU_COMMAND_ICON: tc_mu_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
      case MU_COMMAND_IMAGE: tc_mu_draw_image(cmd->tex.id, cmd->tex.rect, cmd->tex.src, cmd->tex.part, cmd->tex.color); break;
    }
  }
}

TCDEF void tc_ui_savestate() {
  int n = ui.ctx->root_list.idx;
  TRACELOG("%d", n);
  for (int i = 0; i < n; i++) {
    mu_Container ctn = ui.ctx->containers[i];
    TRACELOG("%d: %d", i, ctn.open);
  }
}

TCDEF void tc_draw_gui() {
}

#endif
