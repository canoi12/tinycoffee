local Concord = require "libs.concord"
local CameraComp = require "components.camera"
local Target = require "components.target"

local Camera = Concord.assemblage(function(e, x, y, w, h, targetId)
	e
		:give(CameraComp, x, y, w, h)
		:give(Target, targetId)
end)

return Camera