input = require "libs.input"
local GameScene = require "scenes.gamescene"
local Tilemap = require "tilemap"

local sprite = require "tico.sprite"

local Map = require "scenes.map"

-- tico = love
-- tico.input = love.keyboard

if _VERSION == "Lua 5.4" then
  unpack = table.unpack
end

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
local currentColor = 2

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

local gbacolors = {
  {
    tico.color("#332c50"),
    tico.color("#46878f"),
    tico.color("#94e344"),
    tico.color("#e2f3e4"),
  },
  {
    tico.color("#0b0630"),
    tico.color("#6b1fb1"),
    tico.color("#cc3495"),
    tico.color("#f8e3c4"),
  },
  {
    tico.color("#00303b"),
    tico.color("#ff7777"),
    tico.color("#ffce96"),
    tico.color("#f1f2da"),
  },
  {
    tico.color("#2c2137"),
    tico.color("#446176"),
    tico.color("#3fac95"),
    tico.color("#a1ef8c"),
  },
  {
    tico.color("#000000"),
    tico.color("#676767"),
    tico.color("#b6b6b6"),
    tico.color("#ffffff"),
  }
}

local maxFps = 0
local fps = 0

local bump = require "libs.bump"
bumpWorld = bump.newWorld(32)

function tico.load()
  -- gamescene = GameScene()
  gamescene = Map("assets/map.json")
  canvas = tico.graphics.newCanvas(160, 95)
  canvas1 = tico.graphics.newCanvas(160, 95)
  table.insert(logs, "press x to jump")
  image = sprite("assets/images/goblin_run_anim_strip_6.png", 16, 16)
  image:addAnimation("idle", "1-6")
  image1 = tico.graphics.newImage("assets/icons.png")
  -- tilemap = Tilemap("assets/map.json")
  shader = tico.graphics.newOutlineShader()
  gbaShader = tico.graphics.newGBAShader()
  shader2 = tico.graphics.newEffect([[
    const float offset = 1.0 / 160.0;

    uniform vec2 u_time;
    const int count = 8;
    uniform vec4 uCol[count];

    vec4 effect(vec4 color, sampler2D image, vec2 tex) {
      vec2 uv = tex;
      // uv.x += sin(tex.y * 2 + u_time[0] * 10) * 0.05;
      // uv.y += sin(tex.x * 5 + u_time.y * 10) * 0.15;
      vec4 col = texture2D(image, uv);

      /*float a = texture2D(image, vec2(uv.x + offset, uv.y)).a +
                texture2D(image, vec2(uv.x - offset, uv.y)).a +
                texture2D(image, vec2(uv.x, uv.y + (1.0/95.0))).a +
                texture2D(image, vec2(uv.x, uv.y - (1.0/95.0))).a;*/

      //if (col.a < 1.0 && a > 0.0) col = vec4(1, 1, 1, 1);

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

  shader1 = tico.graphics.newEffect([[
      vec4 effect(vec4 color, sampler2D texture, vec2 tex) {
        vec4 col = texture2D(texture, tex);
        return color;
      }
    ]])
end

local ox = 0
local oy = 0

local x = 0
local y = 32

-- local rect = tico.graphics.newRectangle(0, 0, 32, 32)

function tico.update(dt)
  -- input.update(dt)
  -- if tico.ui.isWindowFocused("game") then
    gamescene:update(dt)
--   end

  if tico.input.isDown("left") then ox = ox - 100 * dt end
  if tico.input.isDown("right") then ox = ox + 100 * dt end
  if tico.input.isDown("up") then oy = oy - 100 * dt end
  if tico.input.isDown("down") then oy = oy + 100 * dt end

  if (tico.input.isDown("ctrl") and tico.input.isPressed("down")) or tico.input.isJoyPressed(0, "lb") then if currentColor > 1 then currentColor = currentColor - 1 end end
  if (tico.input.isDown("ctrl") and tico.input.isPressed("up")) or tico.input.isJoyPressed(0, "y")  then if currentColor < #gbacolors then currentColor = currentColor + 1 end end

  -- if tico.input.isJoyDown(0, "dleft") then
  --   x = x - 100 * dt
  -- elseif tico.input.isJoyDown(0, "dright") then
  --   x = x + 100 * dt
  -- end
end

local t = ""
local Player = require "entities.knight"
local Position = require "components.position"
local pos = Player[Position]

local time = 0
local canvas2 = tico.graphics.newCanvas(160, 95)

function tico.draw()
  -- canvas:attach()
  -- tico.graphics.clear()
  -- tico.graphics.push()
  -- tico.graphics.translate(-x, -y)
  -- -- tico.graphics.drawRectangle(x, y, 32, 32)
  -- image:draw(x, y)
  -- tico.graphics.drawRectangle(160, 90, 32, 32)
  -- tico.graphics.drawRectangle(320, 128, 32, 32)
  -- tico.graphics.pop()
  -- canvas:detach()
  -- canvas:auto()
  canvas:attach()
  -- tico.graphics.clear()
  tico.graphics.clear(gbacolors[currentColor][2])
  gamescene:draw()
  canvas:detach()

  -- canvas:auto()

  -- local mx,my = tico.input.mousePos()

  -- canvas1:attach()
  -- tico.graphics.clear(0, 0, 0, 0)
  -- for i=1,10 do
  --   tico.graphics.fillCircle(mx/4, my/4, i+(i*4), {255, 255, 255, 255 - (i*20)})
  -- end
  -- canvas1:detach()

  -- time = time + (tico.timer.delta() * 0.2)



  gbaShader:attach()
  gbaShader:send("u_col", unpack(gbacolors[currentColor]))
  canvas:auto()
  -- canvas:draw(0, 0, 0, 4, 4)
  gbaShader:detach()
  tico.graphics.print("joystick: " .. tico.input.joystickName(0))

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
