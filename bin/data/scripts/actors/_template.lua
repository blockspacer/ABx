include("/scripts/includes/chat.lua")
include("/scripts/includes/consts.lua")

name = "Name"
level = 20
itemIndex = 5     -- Smith body model
sex = SEX_MALE     -- Male
creatureState = CREATURESTATE_IDLE
prof1Index = 1     -- Warrior
prof2Index = 0     -- None
behavior = "behaviour"

function onInit()
  return true
end

function onUpdate(timeElapsed)

end

function onClicked(creature)
end

-- self was selected by creature
function onSelected(creature)
end

-- creature: GameObject collides with self
function onCollide(creature)
end

function onArrived()
end

-- other: GamneObject
function onTrigger(other)
end

-- other: GamneObject
function onLeftArea(other)
end

function onDied()
end

function onStartUseSkill(skill)
end

function onEndUseSkill(skill)
end

-- This Actor is attacking the target
-- target: Actor
-- success: bool
function onAttack(target, success)
end

-- This Actor was attacked by source
-- source: Actor
-- type: DamageType
-- damage: int
-- success: bool
function onAttacked(source, _type, damage, success)
end

-- This Actor is going to be attacked by source. Happens before onAttacked.
-- source: Actor
-- success: bool
function onGettingAttacked(source, success)
end

-- This actor is using a skill on target
-- target: Actor
-- skill: Skill
-- success: bool
function onUseSkill(target, skill, success)
end

-- This Actor is targeted for a skill by source
-- source: Actor
-- skill: Skill
-- success: bool
function onSkillTargeted(source, skill, success)
end

-- This actor is going to be interrupted
function onInterruptingAttack(success)
end

-- This actor is going to be interrupted
function onInterruptingSkill(_type, skill, success)
end

-- This actor was interrupted
function onInterruptedAttack()
end

-- This actor was interrupted
function onInterruptedSkill(skill)
end

function onKnockedDown(_time)
end

function onHealed(hp)
end
