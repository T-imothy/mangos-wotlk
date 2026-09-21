-- Rank 2 Improved Stings: third aura must target the caster, like adjacent ranks.
-- Guard the original erroneous value. Classic has no third effect and needs no change.
UPDATE spell_template SET EffectImplicitTargetA3=1
WHERE Id=19465 AND Effect3=6 AND EffectImplicitTargetA3=17;

-- Missing quest triggers. Preserve any deliberately installed alternative binding.
INSERT INTO scripted_areatrigger (entry,ScriptName)
SELECT 4292,'at_earthbinder_rayge' WHERE NOT EXISTS
(SELECT 1 FROM scripted_areatrigger WHERE entry=4292);
INSERT INTO scripted_areatrigger (entry,ScriptName)
SELECT 4542,'at_vindicator_vuuleen' WHERE NOT EXISTS
(SELECT 1 FROM scripted_areatrigger WHERE entry=4542);

-- Loader already clears this static bit; click availability comes from spellclick data.
UPDATE creature_template c SET NpcFlags=NpcFlags & ~16777216
WHERE entry IN (37945,38430) AND ScriptName='npc_valithria_portal'
AND EXISTS (SELECT 1 FROM npc_spellclick_spells s WHERE s.npc_entry=c.entry AND s.spell_id=70766);

-- This creature has quest-credit/relay EventAI and no replacement script.
UPDATE creature_template SET AIName='EventAI'
WHERE entry=25474 AND AIName='' AND ScriptName='';
