include("/scripts/includes/consts.lua")

name = "Poison Dart Launcher"
level = 20
itemIndex = 10001
creatureState = CREATURESTATE_IDLE

local lastDart

function onInit()
  self:SetBoundingSize({1.5, 2.3, 1.5})
  self:SetUndestroyable(true)
  self:SetSelectable(false)
  lastDart = Tick()
  return true
end

function onUpdate(timeElapsed)
  local tick = Tick()
  if (tick - lastDart > 2000) then
    -- Must be destroyable and selectable
    local actor = self:GetClosestEnemy(false, false)
    if (actor ~= nil) then
      if (self:IsInRange(RANGE_SPIRIT, actor)) then
        self:ShootAt("e945dcab-b383-4247-9da5-0a01c4bd4bfd", actor)
        lastDart = tick
      end
    end
  end
end
