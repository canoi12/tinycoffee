---@module luna.input
local input = {}

---@type number[]
local mouse_buttons = {1, 2, 3}

local states = {
	mouse = {
		down = {},
		pressed = {},
		released = {}
	},
	key = {

	}
}

local old_state = {
	mouse = {
		down = {}
	},
	key = {}
}

local anykeydown = false
local mouse = {
	x = 0,
	y = 0,
	fix_x = 0,
	fix_y = 0
}

---@type table<string, string[]>
local keys = {}

---Update the input
---@param dt number
function input.update(dt)
	anykeydown = false
	mouse.x, mouse.y = love.mouse.getPosition()
	for i,button in ipairs(mouse_buttons) do
		old_state.mouse.down[button] = states.mouse.down[button]
		states.mouse.down[button] = input.isMouseDown(button)
		states.mouse.pressed[button] = states.mouse.down[button] and not old_state.mouse.down[button]
	end

	for k,key in pairs(states.key) do
		old_state.key[k].down = states.key[k].down
		states.key[k].down = love.keyboard.isDown(k)
		states.key[k].pressed = states.key[k].down and not old_state.key[k].down
		states.key[k].released = not states.key[k].down and old_state.key[k].down
		if states.key[k].down then anykeydown = true end
	end
end

---Register a new key
---@param name string key name
---@vararg KeyConstant
function input.registerKey(name, ...)
	local args = {...}
	keys[name] = {}
	for i,key in ipairs(args) do
		table.insert(keys[name], key)
	end
end

---Check if register key is down
---@param name string
---@return boolean
function input.isDown(name)
	for i,key in ipairs(keys[name]) do
		if input.isKeyDown(key) then return true end
	end
	return false
end

---Check if register key is pressed
---@param name string
---@return boolean
function input.isPressed(name)
	for i,key in ipairs(keys[name]) do
		if input.isKeyPressed(key) then return true end
	end
	return false
end

function input.isReleased(name)
  for i,key in ipairs(keys[name]) do
    if input.isKeyReleased(key) then return true end
  end
  return false
end

---Check if mouse is down
---@param button number
---@return boolean
function input.isMouseDown(button)
	return love.mouse.isDown(button)
end

---Check if mouse is up
---@param button number
---@return boolean
function input.isMouseUp(button)
	return not input.isMouseDown(button)
end

---Check if mouse is clicked
---@param button number
---@return boolean
function input.isMouseClicked(button)
	return states.mouse.pressed[button]
end

---Check if key is down
---@param key KeyConstant
---@return boolean
function input.isKeyDown(key)
	input.initKey(key)
	return states.key[key].down
end

---Check if key is pressed
---@param key KeyConstant
---@return boolean
function input.isKeyPressed(key)
	input.initKey(key)
	return states.key[key].pressed
end

---Check if key is up
---@param key KeyConstant
---@return boolean
function input.isKeyUp(key)
	input.initKey(key)
	return not states.key[key].down
end

function input.isKeyReleased(key)
  input.initKey(key)
  return states.key[key].released
end

function input.initKey(key)
	if states.key[key] == nil then
		states.key[key] = {
			down = false,
			pressed = false
		}
		old_state.key[key] = {
			down = false
		}
	end
end

---Get the mouse pos
---@vararg number[] | number position modifier
function input.mousePos(...)
	local dx = 1
	local dy = 1
	local args = {...}
	for i,arg in ipairs(args) do
		if type(arg) == "table" then
			dx = dx * arg[1]
			dy = dy * arg[2]
		elseif type(arg) == "number" then
			dx = dx * arg
			dy = dy * arg
		end
	end
	return mouse.x/dx, mouse.y/dy
end

---Fix the mouse pos in current position
function input.fixMousePos()
	mouse.fix_x = mouse.x
	mouse.fix_y = mouse.y
end

---Get mouse delta from the previous fixed pos
function input.getMouseDelta()
	local dx = mouse.x - mouse.fix_x
	local dy = mouse.y - mouse.fix_y
	return dx, dy
end


return input
