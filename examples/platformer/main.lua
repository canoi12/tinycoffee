local Player = require "player"
local Key = require "tico.keys"

local Concord = require "lib.concord"
local bump = require "lib.bump"

bumpWorld = bump.newWorld(32)
local A = {name = "A"}

local logs = {}

function tico.load()
  player = Player(16, 16)
  canvas = tico.graphics.newCanvas(160, 95)
  image = tico.graphics.newImage("assets/knight.png")
  quads = {}
  for i=1,10 do
    quads[i] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
  end
end

local wx = 0
local wy = 0

function clamp(x, lo, hi)
  return math.max(lo, math.min(x, hi))
end

local px = 0
local py = 0

function tico.update(dt)
--   player:update(dt)
  if tico.input.isKeyDown(Key.Left) then
    px = px - 100 * dt
  elseif tico.input.isKeyDown(Key.Right) then
    px = px + 100 * dt
  end
  px, py = bumpWorld:move(A, px, py)

  local x, y = tico.input.mousePosition()

  x = (x-wx) / 16
  y = (y-wy) / 16
  x = math.ceil(x)
  y = math.ceil(y)
  if tico.input.isMouseDown(0) then
    local xx = clamp(x, 1, 10)
    local yy = clamp(y, 1, 10)
    quads[yy][xx] = 1
  elseif tico.input.isMouseDown(1) then
    local xx = clamp(x, 1, 10)
    local yy = clamp(y, 1, 10)
    quads[yy][xx] = 0
  end
end

local t = ""
local a = "testando"
local s = 0

local tilesize = 16
local c = true
local sl = 10

function tico.draw()
  local mx, my = tico.input.mousePosition()
  tico.graphics.clear({0, 0, 0})
--   canvas:attach()
--   tico.graphics.clear({75, 90, 90})
--   player:draw()
--   canvas:detach()
  if tico.ui.beginWindow("console") then
    tico.ui.layoutRow(1, {-1}, -25)
    tico.ui.beginPanel("logs")
    tico.ui.layoutRow(1, {-1}, 12)
    for i,l in ipairs(logs) do
      tico.ui.text(l)
    end
    tico.ui.endPanel()
    tico.ui.layoutRow(2, {-64, -1}, -1)
    t,s = tico.ui.textbox(1, t)
    local res = tico.ui.button("submit") or (s == 2)
    if res then
      table.insert(logs, t)
      t = ""
    end
    tico.ui.endWindow()
  end

  if tico.ui.beginWindow("tico") then
    if tico.ui.beginTree("tree") then
      sl = tico.ui.slider(sl, 10, 20)
      c = tico.ui.checkbox("teste", c)
      tico.ui.checkbox("opaaaaa", false)
      sl = tico.ui.number(sl, 1)
      tico.ui.endTree()
    end
    tico.ui.endWindow()
  end

  local x, y, w, h = bumpWorld:getRect(A)
  tico.graphics.drawRectangle(x, y, w, h)
  x, y, w, h = bumpWorld:getRect("wall")
  tico.graphics.drawRectangle(x, y, w, h)
end
