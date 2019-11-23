include("/scripts/includes/chat.lua")
include("/scripts/includes/consts.lua")
include("/scripts/includes/attributes.lua")

name = "Dorothea Samara"
level = 20
itemIndex = 12    -- Female Pedestrian 1 body model
sex = SEX_FEMALE
creatureState = CREATURESTATE_IDLE
prof1Index = PROFESSIONINDEX_MESMER
prof2Index = PROFESSIONINDEX_NONE
behavior = "dorothea_samara"

local startTick;

function onInit()
  startTick = Tick()
  self:SetSpeed(0.5)

  local skillBar = self:GetSkillBar()
  skillBar:AddSkill(61)
  skillBar:SetAttributeValue(ATTRIB_FASTCAST, 9)
  skillBar:SetAttributeValue(ATTRIB_INSPIRATION, 9)
  skillBar:SetAttributeValue(ATTRIB_DOMINATION, 12)

  return true
end

function onUpdate(timeElapsed)
  if (Tick() - startTick > 10000 and self:GetState() == CREATURESTATE_IDLE) then
    startTick = Tick()
  end
end

function onArrived()
--  self:SetState(CREATURESTATE_EMOTE_SIT)
end

function onClicked(creature)
  if (self:GetState() == CREATURESTATE_IDLE) then
--    self:FollowObject(creature)
  end
end

-- self was selected by creature
function onSelected(creature)
  if (self:GetState() ~= CREATURESTATE_IDLE) then
    self:Say(CHAT_CHANNEL_GENERAL, "Not now!")
  end
end

-- creature collides with self
function onCollide(creature)
end

function onDied()
end

function onResurrected()
end
