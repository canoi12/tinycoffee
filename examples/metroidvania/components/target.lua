local Concord = require "libs.concord"
local Target = Concord.component(function(c, targetId)
	c.id = targetId
end)

return Target