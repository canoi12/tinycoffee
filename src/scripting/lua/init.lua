local traceback = debug.traceback
local core = {}

local path = tico.filesystem.getPath()
package.path = package.path .. ";" .. path .. "/?.lua;" .. path .. "/?/init.lua"

-- local errmsg_width =
local function _error(msg)
  trace = traceback("", 1)
  print(msg, trace)
--   errmsg_width = tico.graphics.textWidth(trace)
  tico.error(msg, trace)
end

function requireModules()
  tico.class = require "tico.class"
  tico.color = require "tico.color"
end

xpcall(requireModules, _error)

local function _step()
  local dt = tico.timer.delta()
  -- if core.step then core.step() end
  if tico.update then tico.update(dt) end
  if tico.draw then tico.draw() end
end

local function _init_callbacks()
  tico.callbacks = {}

  tico.callbacks["keypressed"] = function()
    if tico.keypressed then tico.keypressed("key") end
  end

  tico.callbacks["textinput"] = function()
    if tico.textinput then tico.textinput("tico") end
  end

  tico.callbacks["mousescroll"] = function()
    local x, y = tico.input.mouseScroll()
    if tico.mousescroll then tico.mousescroll(x, y) end
  end

  tico.callbacks["mousemoved"] = function()
    local x, y = tico.input.mousePos()
    if tico.mousemoved then tico.mousemoved(x, y) end
  end

  tico.callbacks["resized"] = function()
    local w, h = tico.graphics.getSize()
    if tico.resized then tico.resized(w, h) end
  end

  tico.callbacks["moved"] = function()
    local x, y = tico.window.getPos()
    if tico.moved then tico.moved(x, y) end
  end

end

_init_callbacks()

-- function tico.wheelmoved(x, y) end

function tico._doCallback(callback)
  local fn = tico.callbacks[callback]
  if fn then
    xpcall(fn, _error)
  end
end

function tico._load()
  -- if core.init then xpcall(core.init, _error) end
  if tico.load then xpcall(tico.load, _error) end
end

function tico._step()
  xpcall(_step, _error)
end

function tico.error(msg, trace)

  local errmsg = msg .. '\n\nTraceback\n' .. trace:gsub('\t', '')
  for i,callback in pairs(tico.callbacks) do
    tico.callbacks[i] = function() end
  end

  function tico.update(dt)

  end

  function tico.draw()
    local w, h = tico.graphics.getSize()
    tico.graphics.clear()
    tico.graphics.print("Error", 32, 32, 2, 2)
    tico.graphics.printf(errmsg, 32, 64, w-96)
  end

end

if tico.filesystem.fileExists("main.lua") then
  xpcall(function() require "main" end, _error)
else
  function tico.load()
  end

  function tico.update(dt)
  end

  function tico.draw()
    local w, h = tico.graphics.getSize()
    tico.graphics.print("no code loaded", w/2 - 40, h/2)
  end
end
