Player = require('player')
Carrot = require('carrot')
local Planet = require('planet')

camera = {
  x = 0,
  y = 0
}

carrots = {}

local particle = {}
particle.dx = 0
particle.dy = 0
particle.x = 0
particle.y = 0
particle.dir = 0
particle.speed = 2
particle.life = 10

function particle:new(o)
    o = o or {}
    return setmetatable(o, {__index=self})
end

function particle:update(dt)
    self.life = self.life - 1
    self.dx = self.dx + (math.cos(self.dir)*self.speed)
    self.dy = self.dy + (math.sin(self.dir)*self.speed)
    self.x = self.x + self.dx
    self.y = self.y + self.dy
end

function particle:draw()
    love.graphics.circle("line", self.x, self.y, 2)
end

local particlesystem = {}
particlesystem.freq = 10
particlesystem.dir = 0
particlesystem.dir_var = 30
particlesystem.particles = {}
particlesystem.x = 0
particlesystem.y = 0

function particlesystem:update(dt)
    if #self.particles < self.freq then
        local dir = love.math.random(-self.dir,self.dir)
        table.insert(self.particles, particle:new({x=self.x, y=self.y}))
    end

    for i=#self.particles,1,-1 do
        local v = self.particles[i]
        v:update(dt)
        if v.life <= 0 then
            table.remove(self.particles, i)
        end
    end
end

function particlesystem:draw()
    for i,v in ipairs(self.particles) do
        v:draw()
    end
end

curr_planet = {}

planets = {}

planet = {}
planet.x = 240
planet.y = 320
planet.r = 64

planet_t = {}
planet_t.x = 500
planet_t.y = 130
planet_t.r = 32

--[[player = {}
player.x = 260
player.y = 340
player.dx = 0
player.dy = 0
player.sx = 1
player.r = 8
player.frame = 1
player.animspd = 1
player.frames = {}
player.frames = {}
local angle = 0
local gravity = 800
local jump_force = 2400
local fly_force = 600
local motion = 0

function player:init()
  self.frames = {
    [1] = love.graphics.newQuad(0, 0, 16, 16, self.image:getDimensions()),
    [2] = love.graphics.newQuad(16, 0, 16, 16, self.image:getDimensions()),
    [3] = love.graphics.newQuad(32, 0, 16, 16, self.image:getDimensions()),
    [4] = love.graphics.newQuad(48, 0, 16, 16, self.image:getDimensions()),
    [5] = love.graphics.newQuad(64, 0, 16, 16, self.image:getDimensions()),
    [6] = love.graphics.newQuad(80, 0, 16, 16, self.image:getDimensions())
  }
  self.animspd = 0
end

function player:update(dt)
  if self.animspd < 1 then
    self.animspd = self.animspd + 0.2
  else
    self.animspd = 0
    self.frame = self.frame + 1
    if self.frame > 6 then
      self.frame = 1
    end
  end
end]]

function distance(x0,y0,x1,y1)
    local dx = x1 - x0
    local dy = y1 - y0
    return math.sqrt((dx*dx)+(dy*dy))
end

function tico.load()
    -- love.graphics.setDefaultFilter("nearest", "nearest", 1)
    player = Player:new()
    local pl = Planet:new()
    pl:setRadius(64)
    pl.x = 240
    pl.y = 320
    pl.eyes.left.x = -31
    pl.eyes.left.y = -19
    pl.eyes.right.x = 41
    pl.eyes.right.y = -19
    pl.eyes.radius = 9
    pl.image = tico.graphics.newImage("planet-angry.png")
    --pl.image:setFilter("nearest", "nearest")
    --curr_planet = pl
    planets[1] = pl
    pl = Planet:new()
    pl:setRadius(32)
    pl.x = 500
    pl.y = 130
    pl.image = tico.graphics.newImage("planet-face.png")
    -- pl.image:setFilter("nearest", "nearest")
    planets[2] = pl
    --player.image = love.graphics.newImage("rabbit.png")
    --player.image:setFilter("nearest", "nearest")
    canvas = tico.graphics.newCanvas(320, 240)
    -- canvas:setFilter("nearest", "nearest")
    player.x = 260
    player.y = 340
    --player:init()
end

function tico.update(dt)

    --[[for i,v in ipairs(planets) do
        if distance(player.x, player.y, v.x, v.y) < (player.r/2)+(v.r*3) and curr_planet ~= v then
            curr_planet = v
            player.dx = 0
            player.dy = 0
        end
    end]]

    --[[if curr_planet ~= nil then

        angle = math.atan2(curr_planet.y-player.y, curr_planet.x-player.x)

        if distance(player.x, player.y, curr_planet.x, curr_planet.y) > (player.r) + curr_planet.r then
            local old_angle = angle
            --local motion = 0
            if love.keyboard.isDown("left") then
                --angle = angle - math.rad(2)
                motion = -math.rad(1)
                player.sx = -1
            elseif love.keyboard.isDown("right") then
                --angle = angle + math.rad(2)
                motion = math.rad(1)
                player.sx = 1
            else
                --motion = 0
            end

            angle = angle + motion
            --local aux_motion = motion

            player.x = curr_planet.x - (math.cos(angle)*distance(curr_planet.x, curr_planet.y, player.x, player.y))
            player.y = curr_planet.y - (math.sin(angle)*distance(curr_planet.x, curr_planet.y, player.x, player.y))
            player.dx = player.dx + (math.cos(angle)*gravity*dt)
            player.dy = player.dy + (math.sin(angle)*gravity*dt)
            if love.keyboard.isDown("up") then
                player.dx = player.dx - (math.cos(angle)*dt*fly_force*100*dt)
                player.dy = player.dy - (math.sin(angle)*dt*fly_force*100*dt)
            end
        else
            player.dx = 0
            player.dy = 0
            motion = 0
            if love.keyboard.isDown("left") then
                motion = -math.rad(2)
                player.sx = -1
            elseif love.keyboard.isDown("right") then
                motion = math.rad(2)
                player.sx = 1
            end
            angle = angle + motion
            player.x = curr_planet.x - math.cos(angle)*(player.r+curr_planet.r)
            player.y = curr_planet.y - math.sin(angle)*(player.r+curr_planet.r)
            if love.keyboard.isDown("up") then
                player.dx = player.dx - (math.cos(angle)*dt*jump_force*100*dt)
                player.dy = player.dy - (math.sin(angle)*dt*jump_force*100*dt)
            end
        end

        player.x = player.x + (player.dx*dt)
        player.y = player.y + (player.dy*dt)

        if distance(player.x, player.y, curr_planet.x, curr_planet.y) > player.r+4+(curr_planet.r*3) then
            curr_planet = nil
            player.dx = -math.cos(angle)*dt*jump_force*100*dt
            player.dy = -math.sin(angle)*dt*jump_force*100*dt
            motion = 0
        end
    else
        if love.keyboard.isDown("up") then
            player.dy = player.dy - (200 * dt)
        elseif love.keyboard.isDown("down") then
            player.dy = player.dy + (200 * dt)
        end

        if love.keyboard.isDown("left") then
            player.dx = player.dx - (200 * dt)
        elseif love.keyboard.isDown("right") then
            player.dx = player.dx + (200 * dt)
        end

        player.x = player.x + (player.dx*dt)
        player.y = player.y + (player.dy*dt)
        particlesystem.x = player.x
        particlesystem.y = player.y
        particlesystem:update(dt)
    end]]

    player:update(dt)
    for i,v in ipairs(carrots) do
      v:update(dt)
    end
    camera.x = -player.x+160
    camera.y = -player.y+120
end

function tico.draw()
    tico.graphics.clear(34,32,52)
    --tico.graphics.clear(0.1, 0, 0)
    -- tico.graphics.setCanvas(canvas)
    canvas:attach()
    tico.graphics.clear(34,32,52)
    -- tico.graphics.setLineStyle("rough")
    tico.graphics.push()
    tico.graphics.translate(camera.x, camera.y)
    --[[ove.graphics.circle("line", planet.x, planet.y, planet.r)
    love.graphics.circle("line", planet.x, planet.y, planet.r*3)
    love.graphics.circle("line", planet_t.x, planet_t.y, planet_t.r)
    love.graphics.circle("line", planet_t.x, planet_t.y, planet_t.r*3)]]
    for i,v in ipairs(planets) do
      v:draw()
    end

    for i,v in ipairs(carrots) do
      v:draw()
    end
    --love.graphics.circle("line", player.x, player.y, player.r)
    particlesystem:draw()
    --local quad = love.graphics.newQuad(0, 0, 16, 16, player.image:getDimensions())
    --love.graphics.draw(player.image, player.frames[player.frame], player.x, player.y, angle-math.rad(90), player.sx, 1, 8, 8)
    player:draw()
    tico.graphics.pop()
    -- tico.graphics.setCanvas()
    canvas:detach()

    -- love.graphics.draw(canvas, 0, 0, 0, 2, 2)
    canvas:auto()
    -- love.graphics.points(love.mouse.getX(), love.mouse.getY())
    --love.graphics.print(angle, 0, 0)

end
