local traceback = debug.traceback

local path = tico.filesystem.getPath()
package.path = package.path .. ";" .. path .. "/?.lua;" .. path .. "/?/init.lua"

-- local errmsg_width =
local function _error(msg)
  trace = traceback("", 1)
--   errmsg_width = tico.graphics.textWidth(trace)
  tico.error(msg, trace)
end

local function _step()
  local dt = tico.timer.delta()
  if tico.update then tico.update(dt) end
  if tico.draw then tico.draw() end
end

function tico._load()
  if tico.load then xpcall(tico.load, _error) end
end

function tico._step()
  xpcall(_step, _error)
end

function tico.error(msg, trace)

  local errmsg = msg .. '\n\nTraceback\n' .. trace:gsub('\t', '')

  function tico.update(dt)

  end

  function tico.draw()
    local w, h = tico.graphics.getDimensions()
    tico.graphics.clear({90, 90, 75})
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
    tico.graphics.print("No file loaded", 320, 190)
  end
end
