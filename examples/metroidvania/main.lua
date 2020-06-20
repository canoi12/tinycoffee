input = require "libs.input"
local GameScene = require "scenes.gamescene"
-- ffi = require "ffi"

local logs = {}

local map = {
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
}

local bump = require "libs.bump"
bumpWorld = bump.newWorld(32)

function tico.load()
--   initInput()
--   print("error")
  gamescene = GameScene()
  bumpWorld:add("floor", 0, 95, 640, 10)
  bumpWorld:add("wall_1", -32, 0, 32, 390)
  bumpWorld:add("wall_2", 640, 0, 32, 390)
  canvas = tico.graphics.newCanvas(160, 95)
  table.insert(logs, "press x to jump")
--   print(canvas)
--   canvas = tico.graphics.newCanvas(160, 95)
end

local ox = 0
local oy = 0

function tico.update(dt)
--   input.update(dt)
--   if tico.ui.isWindowFocused("game") then
    gamescene:update(dt)
--   end

  if tico.input.isKeyDown("left") then ox = ox - 100 * dt end
  if tico.input.isKeyDown("right") then ox = ox + 100 * dt end
  if tico.input.isKeyDown("up") then oy = oy - 100 * dt end
  if tico.input.isKeyDown("down") then oy = oy + 100 * dt end
end

local t = ""
local Player = require "entities.knight"
local Position = require "components.position"
local pos = Player[Position]


function tico.draw()
  tico.graphics.clear({0, 0, 0})
  canvas:attach()
--   canvas:attach()
  tico.graphics.clear({75, 90, 90})
  gamescene:draw()
  canvas:detach()
  canvas:auto()
--   drawEditor()
end

function initInput()
  input.registerKey("left", "left")
  input.registerKey("right", "right")
  input.registerKey("jump", "x")
  input.registerKey("attack", "c")
  input.registerKey("dash", "v")
  input.registerKey("select", "enter", "x", "c")
end

function drawEditor()
  if tico.ui.beginWindow("game", {256, 128, 320, 190+32}) then
    local w, h = tico.ui.windowSize()
    local cw, ch = canvas:getSize()
    local ratio = math.min((w-10)/canvas:getWidth(), (h-12)/canvas:getHeight())
    tico.ui.layoutRow(1, {(canvas:getWidth())*ratio}, canvas:getHeight()*ratio)
    tico.ui.image(canvas)
    tico.ui.endWindow()
  end
  if tico.ui.beginWindow("debug", {256, 0, 200, 112}) then
    tico.ui.layoutRow(1, {-1}, 0)

    local fps = tico.timer.fps()
    local delta = tico.timer.delta()

    tico.ui.text("FPS: " .. fps)
    tico.ui.text("delta: " .. delta)
    tico.ui.text("path: " .. tico.filesystem.getPath())

    tico.ui.endWindow()
  end
  if tico.ui.beginWindow("console") then
    tico.ui.layoutRow(1, {-1}, -25)
    tico.ui.beginPanel("console_panel")
    tico.ui.layoutRow(1, {-1}, 12)
    for i,log in ipairs(logs) do
      tico.ui.text(log)
    end
    tico.ui.endPanel()
    tico.ui.layoutRow(2, {-64, -1}, -1)
    t,enter = tico.ui.textbox(1, t)
    local res = tico.ui.button("submit") or (enter == 2)
    if res then
      table.insert(logs, t)
      t = ""
    end
    tico.ui.endWindow()
  end
end

-- function tico.load()
-- end

-- function tico.update(dt)
-- end

-- function tico.draw()
-- end
