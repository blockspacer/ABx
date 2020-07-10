include("/scripts/includes/attributes.lua")
-- Drop Stats ------------------------------------------------------------------
dropStats = {}
materialStats = {}
-- When this item drops it has a damage in this range scaled to the level of 
-- the player/drop area
dropStats["MinDamage"] = 19
dropStats["MaxDamage"] = 35
dropStats["Attributes"] = { ATTRIB_HAMMER_MASTERY }

materialStats[1] = { 100001, 250 }
materialStats[2] = { 100000, 500 }
materialStats[3] = { 0, 0 }
materialStats[4] = { 9999999, 1000 }
-- /Drop Stats -----------------------------------------------------------------
