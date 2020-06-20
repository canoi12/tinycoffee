local Scene = require "scenes.scene"
local GameScene = Scene:extend("GameScene")

local Concord = require "libs.concord"

local KinematicSystem = require "systems.kinematicsystem"
local DrawSystem = require "systems.drawsystem"
local SpriteSystem = require "systems.spritesystem"
local PlayerInputSystem = require "systems.playerinputsystem"
local PhysicsSystem = require "systems.physicssystem"
local GravitySystem = require "systems.gravitysystem"
local PatrolSystem = require "systems.patrolsystem"
local GoblinSystem = require "systems.goblinsystem"
local SlimeSystem = require "systems.slimesystem"
local CameraSystem = require "systems.camerasystem"

local Player = require "entities.knight"
local Goblin = require "entities.goblin"
local Slime = require "entities.slime"

function GameScene:constructor()
  Scene.constructor(self)
  self.world:addSystems(KinematicSystem, DrawSystem, SpriteSystem, PlayerInputSystem)
  self.world:addSystems(PhysicsSystem, GravitySystem, GoblinSystem, SlimeSystem)
  self.world:addSystems(PatrolSystem, CameraSystem)

  self:add(Player)

  self:add(Concord.entity():assemble(Goblin, 16))
  self:add(Concord.entity():assemble(Slime, 128))
end

return GameScene
