local Player = require "player"

local Concord = require "lib.concord"
local bump = require "lib.bump"

bumpWorld = bump.newWorld(32)
local A = {name = "A"}

local logs = {}

local color = {
  tico.color("#000000"),
  tico.color("#676767"),
  tico.color("#b6b6b6"),
  tico.color("#ffffff"),
}

function tico.load()
  bumpWorld:add(A, 0, 0, 32, 32)
  bumpWorld:add("wall", 128, 0, 32, 128)
  player = Player(16, 16)
  canvas = tico.graphics.newCanvas(160, 95)
  shader = tico.graphics.newOutlineShader()
  -- image = tico.graphics.newImage("assets/knight.png")
--   rect = tico.graphics.newRectangle(0, 0, 32, 32)
--   quads = {}
--   for i=1,10 do
--     quads[i] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
--   end

end

local wx = 0
local wy = 0

function clamp(x, lo, hi)
  return math.max(lo, math.min(x, hi))
end

local px = 0
local py = 0

function tico.update(dt)
  player:update(dt)
  if tico.input.isKeyDown("left") then
    px = px - 100 * dt
  elseif tico.input.isKeyDown("right") then
    px = px + 100 * dt
  end
  px, py = bumpWorld:move(A, px, py)

--   local x, y = tico.input.mousePosition()

--   x = (x-wx) / 16
--   y = (y-wy) / 16
--   x = math.ceil(x)
--   y = math.ceil(y)
--   if tico.input.isMouseDown(0) then
--     local xx = clamp(x, 1, 10)
--     local yy = clamp(y, 1, 10)
--     quads[yy][xx] = 1
--   elseif tico.input.isMouseDown(2) then
--     local xx = clamp(x, 1, 10)
--     local yy = clamp(y, 1, 10)
--     quads[yy][xx] = 0
--   end
end

local t = ""
local a = "testando"
local s = 0

local tilesize = 16
local c = true
local sl = 10

function tico.draw()
  local mx, my = tico.input.mousePos()
  tico.graphics.clear(75, 90, 90)
  canvas:attach()
  tico.graphics.clear(255, 0, 0, 0)
  player:draw()
  canvas:detach()

  -- canvas:attach()
  -- tico.graphics.clear({75, 90, 90})
  -- local x, y, w, h = bumpWorld:getRect(A)
  -- tico.graphics.drawRectangle(x, y, w, h)
  -- x, y, w, h = bumpWorld:getRect("wall")
  -- tico.graphics.drawRectangle(x, y, w, h)
  -- tico.graphics.draw(image, rect)
  -- canvas:detach()
  shader:attach()
  shader:send("u_col", {tico.color("#ffffffff")})
  shader:send("u_size", {canvas:getSize()})
  canvas:auto()
  shader:detach()
end

function drawEditor()
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

  for i=1,10 do
    print('opa')
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

end
