include("/scripts/includes/attributes.lua")
include("/scripts/includes/damage.lua")
-- Drop Stats ------------------------------------------------------------------
dropStats = {}
materialStats = {}
-- When this item drops it has a damage in this range scaled to the level of 
-- the player/drop area
dropStats["MinDamage"] = 6
dropStats["MaxDamage"] = 28
dropStats["Attributes"] = { ATTRIB_AXE_MASTERY }

materialStats[1] = { 100001, 50 }
materialStats[2] = { 100000, 100 }
materialStats[3] = { 100003, 10 }
materialStats[4] = { 9999999, 1000 }
-- /Drop Stats -----------------------------------------------------------------
