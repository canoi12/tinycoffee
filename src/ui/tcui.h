#ifndef TC_UI_H
#define TC_UI_H

// #include "../tinycoffee.h"
#include "microui.h"

typedef struct mu_Context mu_Context;

#define tc_ui_get_container(label) mu_get_container(ui.ctx, label)
#define tc_ui_get_current_container() mu_get_current_container(ui.ctx)

#define tc_ui_layout_row(n, width, height) mu_layout_row(ui.ctx, n, (width), height)
#define tc_ui_layout_width(width) mu_layout_width(ui.ctx, width)
#define tc_ui_layout_height(height) mu_layout_height(ui.ctx, height)
#define tc_ui_begin_column() mu_layout_begin_column(ui.ctx)
#define tc_ui_end_column() mu_layout_end_column(ui.ctx)
#define tc_ui_layout_next() mu_layout_next(ui.ctx)
#define tc_ui_get_layout() mu_get_layout(ui.ctx)

#define tc_ui_button(label) mu_button(ui.ctx, label)
#define tc_ui_slider(value, lo, hi) mu_slider(ui.ctx, value, lo, hi)
#define tc_ui_header(label) mu_header(ui.ctx, label)
#define tc_ui_header_ex(label, opt) mu_header_ex(ui.ctx, label, opt)
#define tc_ui_label(label) mu_label(ui.ctx, label)
#define tc_ui_text(text) mu_text(ui.ctx, text)
#define tc_ui_image(texId, size, src, color) mu_image(ui.ctx, texId, size, src, color)
#define tc_ui_number(number, step) mu_number(ui.ctx, number, step)
#define tc_ui_checkbox(label, value) mu_checkbox(ui.ctx, label, value)
#define tc_ui_checkbox_ex(label, value)
#define tc_ui_textbox(buffer, size) mu_textbox(ui.ctx, buffer, size)
#define tc_ui_textbox_raw(id, buffer, size, r) mu_textbox_raw(ui.ctx, buffer, size, id, r, 0)

#define tc_ui_begin_window(title, rect) mu_begin_window(ui.ctx, title, rect)
#define tc_ui_begin_window_ex(title, rect, opt) mu_begin_window(ui.ctx, title, rect)
#define tc_ui_begin_panel(label) mu_begin_panel(ui.ctx, label)
#define tc_ui_begin_panel_ex(label, opt) mu_begin_panel_ex(ui.ctx, label, opt)
#define tc_ui_begin_treenode(label) mu_begin_treenode(ui.ctx, label)
#define tc_ui_begin_treenode_ex(label, opt) mu_begin_treenode_ex(ui.ctx, label, opt)
#define tc_ui_begin_popup(label) mu_begin_popup(ui.ctx, label)
#define tc_ui_open_popup(label) mu_open_popup(ui.ctx, label)

#define tc_ui_end_window() mu_end_window(ui.ctx)
#define tc_ui_end_panel() mu_end_panel(ui.ctx)
#define tc_ui_end_treenode() mu_end_treenode(ui.ctx)
#define tc_ui_end_popup() mu_end_popup(ui.ctx)

#define tc_ui_get_id(name, size) mu_get_id(ui.ctx, name, size);
#define tc_ui_get_focused() ui.ctx->focus;
#define tc_ui_get_last() ui.ctx->last_id;
#define tc_ui_last_zindex() ui.ctx->last_zindex;

#ifndef TCDEF
  #define TCDEF
#endif

TCDEF void tc_ui_init(tc_Font font);
TCDEF void tc_ui_begin();
TCDEF void tc_ui_end();
TCDEF void tc_ui_loadstate();
TCDEF void tc_ui_savestate();

TCDEF void tc_ui_mouse_pos_callback(double posX, double posY);
TCDEF void tc_ui_mouse_btn_callback(int button, int x, int y, int action);
TCDEF void tc_ui_mouse_scroll_callback(double xoff, double yoff);

TCDEF void tc_ui_key_callback(int key, int action);
TCDEF void tc_ui_text_callback(int codepoint);

TCDEF int tc_ui_text_width(tc_Font font, const char *text, int len);
TCDEF int tc_ui_text_height(tc_Font font);

TCDEF void tc_draw_gui();

typedef struct {
  mu_Context *ctx;
  tc_Font font;
  struct {
    tc_Texture tex;
    tc_Rectf rect[4];
  } icons;
  struct {
    int *animation;
    int *tileset;
    int *tilemap;
  } windows;
  char activeWindow[256];
  unsigned char ** names;
} tc_UI;

extern tc_UI ui;

#endif // TC_UI_H

#if defined(TC_UI_IMPLEMENTATION)

static const char ui_icons[] = {
137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,16,0,
0,0,16,8,6,0,0,0,31,243,255,97,0,0,0,1,115,82,71,66,
0,174,206,28,233,0,0,0,125,73,68,65,84,56,141,213,147,221,13,128,
32,12,132,169,113,16,144,21,96,255,1,116,5,127,54,57,159,74,26,172,
85,162,49,241,222,154,114,119,132,47,56,247,150,0,192,154,47,53,248,4,
54,1,192,224,211,33,0,66,102,136,102,190,21,192,230,179,3,90,64,47,
151,49,100,183,108,19,197,144,1,0,68,68,50,32,134,172,22,151,235,91,
243,143,212,202,191,123,90,88,94,89,107,150,20,234,61,239,10,70,70,52,
175,163,138,203,68,40,213,252,7,106,125,206,127,7,234,54,108,79,62,63,
186,100,0,0,0,0,73,69,78,68,174,66,96,130,0
};

tc_UI ui;

TCDEF void tc_ui_init(tc_Font font) {
  ui.ctx = malloc(sizeof(mu_Context));
  ui.font = font;
//   ui.log[0] = '\0';
  ui.icons.tex = tc_texture_from_memory((void *)ui_icons, sizeof(ui_icons));
  tc_Rectf rect[4] = {
    tc_rectf(0, 0, 8, 8),
    tc_rectf(8, 0, 8, 8),
    tc_rectf(0, 8, 8, 8),
    tc_rectf(8, 8, 8, 8)
  };
  memcpy(&ui.icons.rect, &rect, sizeof(rect));
//   ui.names = malloc(MU_CONTAINERPOOL_SIZE * sizeof(char**));
//   ui.showWindow = 0;

  mu_init(ui.ctx);
  ui.ctx->text_width = tc_ui_text_width;
  ui.ctx->text_height = tc_ui_text_height;
}

TCDEF int tc_ui_text_width(tc_Font font, const char *text, int len) {
  if (len == -1) len = strlen(text);
  return tc_font_get_text_width(ui.font, text, len);
}

TCDEF int tc_ui_text_height(tc_Font font) {
  return ui.font.texture.height;
}

TCDEF void tc_ui_mouse_pos_callback(double posX, double posY) {
  mu_input_mousemove(ui.ctx, posX, posY);
}

TCDEF void tc_ui_mouse_btn_callback(int button, int x, int y, int action) {
  if (action == GLFW_PRESS) mu_input_mousedown(ui.ctx, Core.input.mouseState.x, Core.input.mouseState.y, 1 << button);
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
  mu_input_text(ui.ctx, c);
}

TCDEF void tc_ui_begin() {
  mu_begin(ui.ctx);
}

static void tc_mu_draw_icon(int id, tc_Rect rect, tc_Color color) {
  tc_Color col = tc_rgba(color.r, color.g, color.b, color.a);
  tc_Rectf r;
  for (int i = 0; i < 4; i++) r.data[i] = ui.icons.rect[id-1].data[i];
  tc_Rectf u = tc_rectf(rect.x+1, rect.y+3, r.w * 2, r.h * 2);
  tc_draw_texture(ui.icons.tex, u, r, col);
//   tc_draw_texture_part_scale(CORE.icons.tex, r, rect.x+1, rect.y+3, 2, 2, col);
}

static void tc_mu_draw_image(tc_Texture tex, tc_Rect dest, tc_Rect src, tc_Color color) {
//   tc_Color col = tc_rgba(color.r, color.g, color.b, color.a);
  tc_Rectf s = tc_rectf(src.x, src.y, src.w, src.h);
  tc_Rectf d = tc_rectf(dest.x, dest.y, dest.w, dest.h);
  if (dest.h < 0) d.y += (-dest.h);
  tc_draw_texture(tex, d, s, color);
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
//         TRACELOG("%d %d %d %d", cmd->rect.rect.x, cmd->rect.rect.y, cmd->rect.rect.w, cmd->rect.rect.h);
        tc_fill_rectangle(cmd->rect.rect.x, cmd->rect.rect.y, cmd->rect.rect.w, cmd->rect.rect.h, cmd->rect.color);
        break;
      }
      case MU_COMMAND_CLIP: tc_scissor(cmd->clip.rect.x, cmd->clip.rect.y, cmd->clip.rect.w, cmd->clip.rect.h); break;
      case MU_COMMAND_ICON: tc_mu_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
      case MU_COMMAND_IMAGE: tc_mu_draw_image(cmd->tex.tex, cmd->tex.dest, cmd->tex.src, cmd->tex.color); break;
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
