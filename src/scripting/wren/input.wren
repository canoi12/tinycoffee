foreign class Input {
	foreign static isDown(name)
	foreign static isPressed(name)
	foreign static isUp(name)
	foreign static isReleased(name)
	
	foreign static isDown(name, jid)
	foreign static isPressed(name, jid)
	foreign static isUp(name, jid)
	foreign static isReleased(name, jid)
}

foreign class Keyboard {
	foreign static isDown(key)
	foreign static isPressed(key)
	foreign static isUp(key)
	foreign static isReleased(key)
}

foreign class Mouse {
	foreign static isDown(btn)
	foreign static isPressed(btn)
	foreign static isUp(btn)
	foreign static isReleased(btn)
}

foreign class Joystick {
	foreign static isDown(jid, btn)
	foreign static isPressed(jid, btn)
	foreign static isUp(jid, btn)
	foreign static isReleased(jid, btn)
	foreign static leftAxis(jid, axis)
	foreign static rightAxis(jid, axis)
	foreign static getName(jid)
}