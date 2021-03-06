include("/scripts/includes/consts.lua")

name = ""
level = 20
itemIndex = 11
creatureState = CREATURESTATE_IDLE
interactionRange = RANGE_ADJECENT

function onInit()
  -- Player collides with BB. Make it a bit larget than the default BB.
  self:SetBoundingSize({1.0, 2.0, 2.0})
  self:SetUndestroyable(true)
  -- Even when the collision mask is zero, it still calls the trigger even
  self:SetCollisionMask(0)
  -- Will call onTrigger() when it collides
  self:SetTrigger(true)
  return true
end

local function teleport(creature)
  local player = creature:AsPlayer()
  if (player ~= nil) then
    local party = player:GetParty()
    if (party ~= nil) then
      party:ChangeInstance(self:GetVarString("destination"))
    end
  end
end

function onTrigger(creature)
  teleport(creature)
end

function onInteract(creature)
  teleport(creature)
end
