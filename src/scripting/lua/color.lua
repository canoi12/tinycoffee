local color = {}
color.__index = color

-- if _VERSION == "Lua 5.1" then
--   function color.hex(str)
--     if str:len() < 8 then
--       str = str .. "ff"
--     end
--     local nc = tonumber(str:sub(2,-1), 16)
--     local rr = bit.rshift(nc, 24)
--     local gg = bit.band(bit.rshift(nc, 16), 0xff)
--     local bb = bit.band(bit.rshift(nc, 8), 0xff)
--     local aa = bit.band(nc, 0xff)
--     return rr, gg, bb, aa
--   end
-- else
--   function color.hex(str)
--     if str:len() < 8 then
--       str = str .. "ff"
--     end
--     local nc = tonumber(str:sub(2,-1), 16)
--     local rr = nc >> 24
--     local gg = (nc >> 16) & 0xff
--     local bb = (nc >> 8) & 0xff
--     local aa = nc & 0xff
--     return rr, gg, bb, aa
--   end
-- end

function color.hex(str)
  if str:len() < 8 then
    str = str .. "ff"
  end
  local nc = tonumber(str:sub(2,-1), 16)
  local rr = bit.rshift(nc, 24)
  local gg = bit.band(bit.rshift(nc, 16), 0xff)
  local bb = bit.band(bit.rshift(nc, 8), 0xff)
  local aa = bit.band(nc, 0xff)
  return rr, gg, bb, aa
end

local colors = {
  ["white"] = {255, 255, 255, 255},
  ["black"] = {0, 0, 0, 255},
  ["red"] = {255, 0, 0, 255}
}

function color.rgb(r, g, b)
  return color.rgba(r, g, b, 255)
end

function color.rgba(s, r, g, b, a)
  local col = {}
  if colors[r] then
    return colors[r]
  end
  if type(r) == "string" and r:sub(1, 1) == "#" then
    col = {color.hex(r)}
  else
    local rr = r or 255
    local gg = g or 255
    local bb = b or 255
    local aa = a or 255
    col = {rr, gg, bb, aa}
  end

  return col
end

function color.norm(col)
  local rcol = {}
  for i,v in ipairs(col) do
    rcol[i] = v/255
  end

  return rcol
end

color.__call = color.rgba
return setmetatable({}, color)

