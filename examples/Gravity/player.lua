local player = {}

function player:new(o)
  o = o or {}
  setmetatable(o, {__index=self})
  o:init()
  return o
end

function player:init()
  self.frames = {}
  self.frame = 1
  self.animspd = 1
  self.jetpack_cooldown = 30
  self.x = 0
  self.y = 0
  self.sx = 1
  self.sy = 1
  self.dx = 0
  self.dy = 0
  self.r = 8
  self.angle = 0
  self.curr_planet = nil
  self.state = nil
  self.motion = 0
  self.gravity = 800
  self.jump_force = 2400
  self.fly_force = 600
  self.animation = {}
  self.curr_anim = ""
  self.image = tico.graphics.newImage("rabbit.png")
  -- self.image:setFilter("nearest", "nearest")

  self:create_animation("idle", 0, 0, 6, 16)
  self:create_animation("run", 0, 16, 6, 16)
  local img = tico.graphics.newImage('part.png')
  -- img:setFilter("nearest", "nearest")
  -- self.part = love.graphics.newParticleSystem(img, 32)
  -- self.part:setParticleLifetime(0.1,0.3)
  -- self.part:setEmissionRate(0)
  -- self.part:setSizes(0.5, 1)
  -- self.part:setSpeed(-80)
  -- self.part:setColors(255, 255, 255, 255)
end

function player:create_animation(name, x, y, nFrames, frameSize)
  self.animation[name] = {}
  local frame = 1
  local frames = x+(nFrames*frameSize)-1
  for i=x,frames,frameSize do
    if i > self.image:getWidth() then
      y = y + frameSize
    end
    table.insert(self.animation[name], tico.graphics.newRectangle(i, y, frameSize, frameSize))
    frame = frame + 1
  end
  if self.curr_anim == "" then
    self.curr_anim = name
  end
end

function player:play_animation(name)
  if self.curr_anim ~= name and self.animation[name] ~= nil then
    self.curr_anim = name
  end
end

function player:update_anim(dt)
  if self.animspd < 1 then
    self.animspd = self.animspd + 0.2
  else
    self.animspd = 0
    self.frame = self.frame + 1
    if self.frame > #self.animation[self.curr_anim] then
      self.frame = 1
    end
  end
end

function player:in_orbit_state(dt)
  self.angle = math.atan(self.curr_planet.y-self.y, self.curr_planet.x-self.x)
  self.curr_planet.angle_player = self.angle

  if distance(self.x, self.y, self.curr_planet.x, self.curr_planet.y) > (self.r) + self.curr_planet.r then
    if tico.input.isDown("left") then
      self.motion = -math.rad(2)
      self:play_animation("run")
      self.sx = -1
      --[[local xx = self.curr_planet.x - (math.cos(self.angle+self.motion)*distance(self.curr_planet.x, self.curr_planet.y, self.x, self.y))
      local yy = self.curr_planet.y - (math.sin(self.angle+self.motion)*distance(self.curr_planet.x, self.curr_planet.y, self.x, self.y))
      --self.dx = self.dx + (math.cos(self.angle+self.motion)*self.gravity*dt)
      xx = xx - self.x
      yy = yy - self.y

      local angle = math.atan2(yy,xx)
      self.dx = self.dx + (math.cos(angle)*100*dt)
      self.dy = self.dy + (math.sin(angle)*100*dt)]]
      --self.dy = self.dy + (math.sin(self.angle+self.motion)*self.gravity*dt)
    elseif tico.input.isDown("right") then
      self.motion = math.rad(2)
      self:play_animation("run")
      self.sx = 1
    else
      self:play_animation("idle")
      self.motion = 0
    end

    --self.angle = self.angle + self.motion

    self.x = self.curr_planet.x - (math.cos(self.angle+self.motion)*distance(self.curr_planet.x, self.curr_planet.y, self.x, self.y))
    self.y = self.curr_planet.y - (math.sin(self.angle+self.motion)*distance(self.curr_planet.x, self.curr_planet.y, self.x, self.y))
    --self.x = self.curr_planet.x - (math.cos(self.angle+self.motion)*160)
    --self.y = self.curr_planet.y - (math.sin(self.angle+self.motion)*160)

    self.dx = self.dx + (math.cos(self.angle)*self.gravity*dt)
    self.dy = self.dy + (math.sin(self.angle)*self.gravity*dt)
    if tico.input.isDown("up") and self.jetpack_cooldown <= 0 then
        -- self.part:setEmissionRate(15)
        self.dx = self.dx - (math.cos(self.angle)*dt*self.fly_force*100*dt)
        self.dy = self.dy - (math.sin(self.angle)*dt*self.fly_force*100*dt)
    end
  else
    self.dx = 0
    self.dy = 0
    self.motion = 0
    if tico.input.isDown("left") then
        self.motion = -math.rad(2)
        self:play_animation("run")
        self.sx = -1
        --player.sx = -1
    elseif tico.input.isDown("right") then
        self.motion = math.rad(2)
        self:play_animation("run")
        self.sx = 1
        --player.sx = 1
    else
      self:play_animation("idle")
    end
    self.angle = self.angle + self.motion
    self.x = self.curr_planet.x - math.cos(self.angle)*(self.r+self.curr_planet.r)
    self.y = self.curr_planet.y - math.sin(self.angle)*(self.r+self.curr_planet.r)
    if tico.input.isDown("up") then
        self.dx = self.dx - (math.cos(self.angle)*dt*self.jump_force*100*dt)
        self.dy = self.dy - (math.sin(self.angle)*dt*self.jump_force*100*dt)
    end
  end

  if distance(self.x, self.y, self.curr_planet.x, self.curr_planet.y) > self.r+4+(self.curr_planet.r*3) then
      self.curr_planet = nil
      self.dx = -math.cos(self.angle)*dt*self.jump_force*100*dt
      self.dy = -math.sin(self.angle)*dt*self.jump_force*100*dt
      self.motion = 0
  end
end

function player:out_orbit_state(dt)
  --[[if tico.input.isDown("up") then
      self.dy = self.dy - (200 * dt)
  elseif tico.input.isDown("down") then
      self.dy = self.dy + (200 * dt)
  end

  if tico.input.isDown("left") then
      self.dx = self.dx - (200 * dt)
  elseif tico.input.isDown("right") then
      self.dx = self.dx + (200 * dt)
  end]]
  self:play_animation("idle")
  if tico.input.isDown("left") then
      self.angle = self.angle - math.rad(5)
  elseif tico.input.isDown("right") then
      self.angle = self.angle + math.rad(5)
  end

  if tico.input.isDown("up") then
    -- self.part:setEmissionRate(15)
    self.dx = self.dx - (math.cos(self.angle)*200*dt)
    self.dy = self.dy - (math.sin(self.angle)*200*dt)
  elseif tico.input.isDown("down") then
    self.dx = self.dx * 0.9
    self.dy = self.dy * 0.9
    -- self.part:setEmissionRate(2)
  end
end

function player:update(dt)
  -- self.part:setEmissionRate(0)
  self:update_anim(dt)
  for i,v in ipairs(planets) do
      if distance(self.x, self.y, v.x, v.y) < (self.r/2)+(v.r*3) and self.curr_planet ~= v then
          self.curr_planet = v
          self.dx = 0
          self.dy = 0
          self.jetpack_cooldown = 30
      end
  end

  if self.curr_planet ~= nil then
    self.state = self.in_orbit_state
  else
    self.state = self.out_orbit_state
  end

  if self.state ~= nil then
    self:state(dt)
  end

  self.x = self.x + (self.dx*dt)
  self.y = self.y + (self.dy*dt)
  -- self.part:setPosition(self.x, self.y)
  -- self.part:setDirection(self.angle-math.rad(180+love.math.random(-30,30)))
  -- self.part:update(dt)

  if tico.input.isDown("space") then
    --local m_ang = math.atan2(self.y+(love.mouse.getY())-camera.y, self.x+(love.mouse.getX())-camera.x)
    table.insert(carrots, Carrot:new({x=self.x, y=self.y,angle=self.angle+math.rad(180)}))
  end
  if self.jetpack_cooldown > 0 then
    self.jetpack_cooldown = self.jetpack_cooldown - 1
  end
end

function player:draw()
  --love.graphics.circle("line", self.x, self.y, self.r)
  -- love.graphics.draw(self.part, 0, 0)
  self.image:draw(self.animation[self.curr_anim][self.frame], self.x, self.y, self.angle - math.rad(90), self.sx, self.sy, 8, 8)
  -- love.graphics.draw(self.image, self.animation[self.curr_anim][self.frame], self.x, self.y, self.angle-math.rad(90),self.sx,self.sy, 8, 8)
  --love.graphics.print(self.curr_anim, self.x, self.y)
  --love.graphics.line(self.x, self.y, love.mouse.getX()+self.x-160, love.mouse.getY()+self.y-120)
end

return player
