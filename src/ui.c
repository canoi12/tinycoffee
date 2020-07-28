#include "tico.h"
#include "ui_icons.png.h"

void tic_ui_init(tc_Font font) {
  Core.ui.ctx = malloc(sizeof(mu_Context));
  Core.ui.font = font;
//   ui.log[0] = '\0';
  Core.ui.icons.tex = tic_texture_from_memory(ui_icons_png, ui_icons_png_size);
  tc_Rect rect[4] = {
    tic_rect(0, 0, 8, 8),
    tic_rect(8, 0, 8, 8),
    tic_rect(0, 8, 8, 8),
    tic_rect(8, 8, 8, 8)
  };
  memcpy(&Core.ui.icons.rect, &rect, sizeof(rect));
  Core.ui.names = malloc(MU_CONTAINERPOOL_SIZE * sizeof(char**));
//   ui.showWindow = 0;

  mu_init(Core.ui.ctx);
  Core.ui.ctx->text_width = tic_ui_text_width;
  Core.ui.ctx->text_height = tic_ui_text_height;
}

int tic_ui_text_width(mu_Font font, const char *text, int len) {
  if (len == -1) len = strlen(text);
  return tic_font_get_text_width(Core.ui.font, text, len);
}

int tic_ui_text_height(mu_Font font) {
  return Core.ui.font.texture.height;
}

void tic_ui_mouse_pos_callback(double posX, double posY) {
  mu_input_mousemove(Core.ui.ctx, posX, posY);
}

void tic_ui_mouse_btn_callback(int button, int x, int y, int action) {
  if (action == GLFW_PRESS) mu_input_mousedown(Core.ui.ctx, Core.input.mouseState.x, Core.input.mouseState.y, 1 << button);
	else if (action == GLFW_RELEASE) mu_input_mouseup(Core.ui.ctx, x, y, 1 << button);
}

void tic_ui_mouse_scroll_callback(double xoff, double yoff) {
  mu_input_scroll(Core.ui.ctx, xoff*-10, yoff*-10);
}

void tic_ui_key_callback(int key, int action) {
  if (action == GLFW_PRESS) mu_input_keydown(Core.ui.ctx, key);
  else if (action == GLFW_RELEASE) mu_input_keyup(Core.ui.ctx, key);
}

void tic_ui_text_callback(int codepoint) {
  tc_uint8 c[4];
  memset(c, 0, 4);
  tic_utf8_encode(c, codepoint);
  mu_input_text(Core.ui.ctx, c);
}

void tic_ui_begin() {
  mu_begin(Core.ui.ctx);
}

static void tic_mu_draw_icon(int id, tc_Rect rect, tc_Color color) {
  tc_Color col = tic_rgba(color.r, color.g, color.b, color.a);
  tc_Rectf r;
  for (int i = 0; i < 4; i++) r.data[i] = Core.ui.icons.rect[id-1].data[i];
  tc_Rectf rec;
  for (int i = 0; i < 4; i++) rec.data[i] = rect.data[i];
  rec.x += 2;
  rec.y += 2;
  rec.w = 16;
  rec.h = 16;
  tic_texture_draw(Core.ui.icons.tex, rec, r, col);
}

static void tic_mu_draw_image(tc_Texture tex, tc_Rect rect, tc_Rect src, tc_Rect part, tc_Color color) {
  tc_Color col = tic_rgba(color.r, color.g, color.b, color.a);
  tc_Rectf r = {part.x, part.y, part.w, part.h};
  tc_Rectf rec = {0, 0, 0, 0};
  for (int i = 0; i < 4; i++) rec.data[i] = rect.data[i];
  // TRACELOG("%d %f", part.y, r.h);
//   TRACELOG("%d %d %d %d", r.x, r.y, r.width, r.height);
  tic_texture_draw(tex, rec, r, col);
}

void tic_ui_end() {
  mu_end(Core.ui.ctx);
  mu_Command *cmd = NULL;
  while(mu_next_command(Core.ui.ctx, &cmd)) {
    switch(cmd->type) {
      case MU_COMMAND_TEXT: {
//         tc_Color col = rgba(cmd->text.color.r, cmd->text.color.g, cmd->text.color.b, cmd->text.color.a);
        tic_graphics_draw_text(cmd->text.str, cmd->text.pos.x, cmd->text.pos.y, cmd->text.color);
        break;
      }
      case MU_COMMAND_RECT:
      {
//         tc_Color col = rgba(cmd->rect.color.r, cmd->rect.color.g, cmd->rect.color.b, cmd->rect.color.a);
        tic_graphics_fill_rectangle(cmd->rect.rect.x, cmd->rect.rect.y, cmd->rect.rect.w, cmd->rect.rect.h, cmd->text.color);
        break;
      }
      case MU_COMMAND_CLIP: 
        { 
          tic_graphics_scissor(cmd->clip.rect.x, cmd->clip.rect.y, cmd->clip.rect.w, cmd->clip.rect.h); 
          break;
        }
      case MU_COMMAND_ICON: tic_mu_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
      case MU_COMMAND_IMAGE: tic_mu_draw_image(cmd->tex.tex, cmd->tex.rect, cmd->tex.src, cmd->tex.part, cmd->tex.color); break;
    }
  }
}

void tc_ui_savestate() {
  int n = Core.ui.ctx->root_list.idx;
  TRACELOG("%d", n);
  for (int i = 0; i < n; i++) {
    mu_Container ctn = Core.ui.ctx->containers[i];
    TRACELOG("%d: %d", i, ctn.open);
  }
}