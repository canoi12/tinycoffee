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

local color = {}
local currentColor = 3

color[1] = {
  tico.color("#08141e"),
  tico.color("#0f2a3f"),
  tico.color("#20394f"),
  tico.color("#f6d6bd"),
  tico.color("#c3a38a"),
  tico.color("#997577"),
  tico.color("#816271"),
  tico.color("#4e495f")
}

color[2] = {
  tico.color("#2b0f54"),
  tico.color("#ab1f65"),
  tico.color("#ff4f69"),
  tico.color("#fff7f8"),
  tico.color("#ff8142"),
  tico.color("#ffda45"),
  tico.color("#3368dc"),
  tico.color("#49e7ec")
}

color[3] = {
  tico.color("#292418"),
  tico.color("#524839"),
  tico.color("#73654a"),
  tico.color("#8b7d62"),
  tico.color("#a48d6a"),
  tico.color("#bda583"),
  tico.color("#cdba94"),
  tico.color("#e6ceac")
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
  -- audio1 = love.audio.newSource("assets/wind.ogg", "stream")
  shader = tico.graphics.newEffect([[
    const float offset = 1.0 / 180.0;

    uniform vec2 u_time;
    const int count = 8;
    uniform vec4 uCol[count];

    vec4 effect(vec4 color, sampler2D image, vec2 tex) {
      vec2 uv = tex;
      // uv.x += sin(tex.y * 2 + u_time[0] * 10) * 0.05;
      // uv.y += sin(tex.x * 5 + u_time.y * 10) * 0.15;
      vec4 col = texture2D(image, uv);

      float a = texture2D(image, vec2(uv.x + offset, uv.y)).a +
                texture2D(image, vec2(uv.x - offset, uv.y)).a +
                texture2D(image, vec2(uv.x, uv.y + (1.0/95.0))).a +
                texture2D(image, vec2(uv.x, uv.y - (1.0/95.0))).a;

      if (col.a < 1.0 && a > 0.0) col = vec4(1, 1, 1, 1);

      vec4 colors[count];
      float dist[count];
      for (int i = 0; i < count; i++) {
        colors[i] = uCol[i]/255.0;

        float dist_r =  (colors[i].r - col.r) * (colors[i].r - col.r);
        float dist_g =  (colors[i].g - col.g) * (colors[i].g - col.g);
        float dist_b =  (colors[i].b - col.b) * (colors[i].b - col.b);

        dist[i] = dist_r + dist_g + dist_b;
      }

      float d_min = min(dist[0], dist[1]);
      for (int i = 2; i < count; i++) {
        d_min = min(d_min, dist[i]);
      }

      for (int i = 0; i < count; i++) {
        colors[i].a = col.a;
      }

      for (int i = 0; i < count; i++) {
        if (d_min == dist[i]) return colors[i];
      }

      /*float a = texture2D(image, vec2(uv.x + offset, uv.y)).a +
                texture2D(image, vec2(uv.x - offset, uv.y)).a +
                texture2D(image, vec2(uv.x, uv.y + (1.0/95.0))).a +
                texture2D(image, vec2(uv.x, uv.y - (1.0/95.0))).a;
       vec4 col = texture2D(image, SineWave(tex));

      if (col.a < 1.0 && a > 0.0)
        return color * vec4(1, 1, 1, 0.8);
      else
        return color * col;*/
    }
    ]])
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

  if tico.input.isPressed("1") then currentColor = 1 end
  if tico.input.isPressed("2") then currentColor = 2 end
  if tico.input.isPressed("3") then currentColor = 3 end
end

local t = ""
local Player = require "entities.knight"
local Position = require "components.position"
local pos = Player[Position]

local time = 0

function tico.draw()
  tico.graphics.clear(color[currentColor][1])
--   teste(1)
  canvas:attach()
--   canvas:attach()
  tico.graphics.clear(75, 90, 90, 0)
  -- tico.graphics.drawRectangle(96, 32, 16, 32)
  gamescene:draw()
  -- tico.graphics.drawCircle(32, 32, 8)
  -- tico.graphics.drawTriangle(32, 0, 0, 32, 64, 32)
  canvas:detach()

  time = time + (tico.timer.delta() * 0.2)

  shader:attach()
  -- shader:send("u_time", {time, time})
  shader:send("uCol", unpack(color[currentColor]))
  canvas:auto()
  shader:detach()
  -- canvas:draw()
  fps = tico.timer.fps()
  maxFps = math.max(fps, maxFps)
  -- tico.graphics.print('fps: ' .. fps)
  -- tico.graphics.print('maxFps: ' .. maxFps, 0, 16)
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
