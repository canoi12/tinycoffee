input = require "libs.input"
local GameScene = require "scenes.gamescene"
local Tilemap = require "tilemap"

-- tico = love
-- tico.input = love.keyboard

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

local maxFps = 0
local fps = 0

local bump = require "libs.bump"
bumpWorld = bump.newWorld(32)

function tico.load()
--   initInput()
--   print("error")
  gamescene = GameScene()
  canvas = tico.graphics.newCanvas(160, 95)
  -- canvas = require("libs.canvas")(160, 95)
  table.insert(logs, "press x to jump")
  tilemap = Tilemap()
  audio1 = tico.audio.newSound("assets/wind.ogg")
  -- audio1 = love.audio.newSource("assets/wind.ogg", "stream")
end

local ox = 0
local oy = 0

function tico.update(dt)
--   input.update(dt)
--   if tico.ui.isWindowFocused("game") then
    gamescene:update(dt)
--   end
  if tico.input.isPressed("p") then audio1:play() end

  if tico.input.isDown("left") then ox = ox - 100 * dt end
  if tico.input.isDown("right") then ox = ox + 100 * dt end
  if tico.input.isDown("up") then oy = oy - 100 * dt end
  if tico.input.isDown("down") then oy = oy + 100 * dt end
end

local t = ""
local Player = require "entities.knight"
local Position = require "components.position"
local pos = Player[Position]


function tico.draw()
  tico.graphics.clear({0, 0, 0})
--   teste(1)
  canvas:attach()
--   canvas:attach()
  tico.graphics.clear(75, 90, 90)
  tilemap:draw()
  gamescene:draw()
  canvas:detach()
  canvas:auto()
  -- canvas:draw()
  fps = tico.timer.fps()
  maxFps = math.max(fps, maxFps)
  tico.graphics.print('fps: ' .. fps)
  tico.graphics.print('maxFps: ' .. maxFps, 0, 16)
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