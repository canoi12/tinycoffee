local core = require "core"
local keymap = require "core.keymap"
local console = require "plugins.console"
local command = require "core.command"
local autocomplete = require "plugins.autocomplete"
local config = require "core.config"
local syntax = require "core.syntax"

-- config.theme_name = "rainbow"

command.add(nil, {
  ["project:build-linux-debug"] = function()
    core.log "building.."
    console.run {
      command = "make tico"
    }
  end,
  ["project:build-linux-release"] = function()
    core.log "building release.."
    console.run {
      command = "make tico MODE=Release"
    }
  end,
  ["project:build-windows-debug"] = function()
    core.log "building.."
    console.run {
      command = "make tico PLATFORM=Windows"
    }
  end,
  ["project:build-windows-release"] = function()
    core.log "building.."
    console.run {
      command = "make tico PLATFORM=Windows MODE=Release"
    }
  end,
  ["project:build-release"] = function()
    core.log "building release.."
    console.run {
      command = "./release.sh"
    }
  end,
  ["project:embed-file"] = function()
    core.log "embeding.."
    local filename = core.active_view.doc.filename
    console.run {
      command = "./embed " .. filename,
    }
  end
})

command.add(nil, {
  ["project:run-linux"] = function()
    core.log "running for linux.."
    console.run {
      command = "./bin/tico examples/level"
    }
  end,
  ["project:run-windows"] = function()
    core.log "running for windows.."
    console.run {
      command = "wine ./bin/tico.exe examples/level"
    }
  end
})

local tico_c_items = {
  ["tc_mkdir"] = "TCDEF void tc_mkdir(const char *path)",
  ["tc_rmdir"] = "TCDEF void tc_rmdir(const char *path)",
  ["tc_init"] = "TCDEF tc_bool tc_init(const char *title, int width, int height)",
  ["tc_poll_events"] = "TCDEF void tc_poll_events()",
  ["tc_swap_buffers"] = "TCDEF void tc_swap_buffers()",
  ["tc_clear"] = "TCDEF void tc_clear(tc_color color)",
  ["tc_update_timer"] = "TCDEF void tc_update_timer()",
  ["tc_should_close"] = "TCDEF tc_bool tc_should_close()",
  ["tc_close"] = "TCDEF void tc_close()",
  ["tc_begin_draw"] = "TCDEF void tc_begin_draw()",
  ["tc_end_draw"] = "TCDEF void tc_end_draw()",
  ["tc_draw_image"] = "TCDEF void tc_draw_image(tc_Image image, int x, int y, tc_Color color)",
  ["tc_draw_image_ex"] = "TCDEF void tc_draw_image_ex(tc_Image image, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color)",
  ["tc_draw_image_part"] = "TCDEF void tc_draw_image_part(tc_Image image, tc_Rectf part, int x, int y, tc_Color color)",
  ["tc_draw_image_part_ex"] = "TCDEF void tc_draw_image_part_ex(tc_Image image, tc_Rectf part, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color)",
  ["tc_draw_canvas"] = "TCDEF void tc_draw_canvas(tc_Canvas canvas, int x, int y, tc_Color color)",
  ["tc_draw_canvas_scale"] = "TCDEF void tc_draw_canvas(tc_Canvas canvas, int x, int y, float sx, float sy, tc_Color color)",
  ["tc_draw_canvas_ex"] = "TCDEF void tc_draw_canvas(tc_Canvas canvas, int x, int y, float angle, float sx, float sy, int cx, int cy, tc_Color color)",
}

autocomplete.add {
  name = "tico_wren",
  files = "%.wren",
  items = {
    ["Render"] = "Class",
    ["Render.drawText"] = "Render.drawText(text, x, y, color)",
    ["Render.drawRectangle"] = "Render.drawRectangle(x, y, width, height, [color])",
    ["System.print"] = "System.print(value)",
    ["Vector2.new"] = "Vector2.new(x, y)"
  }
}

autocomplete.add {
  name = "tico_c",
  files = {"%.c", "%.h"},
  items = tico_c_items
}

keymap.add { ["ctrl+b"] = "project:build-linux-debug" }
keymap.add { ["ctrl+shift+b"] = "project:build-windows-debug" }
keymap.add { ["f5"] = "project:run-linux" }
keymap.add { ["shift+f5"] = "project:run-windows" }

