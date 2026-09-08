-- Targeted production log repairs; idempotent and expansion-specific.
-- Repair the obsolete trigger name without replacing other bindings.
UPDATE scripted_areatrigger SET ScriptName='at_southwind_tower'
WHERE entry=3146 AND ScriptName='at_hive_tower';

-- A moved single model must retain a nonzero selection probability.
UPDATE creature_template SET DisplayIdProbability1=100
WHERE Entry=17304 AND DisplayId1=16973 AND DisplayId2=0 AND DisplayId3=0 AND DisplayId4=0 AND DisplayIdProbability1=0;
-- Preserve stock, restock, cost, conditions and ordering; remove only exact duplicates.
DELETE nv FROM npc_vendor nv JOIN creature_template ct ON ct.Entry=nv.entry
JOIN npc_vendor_template vt ON vt.entry=ct.VendorTemplateId AND vt.item=nv.item
AND vt.ExtendedCost=nv.ExtendedCost AND vt.maxcount=nv.maxcount AND vt.incrtime=nv.incrtime
AND vt.condition_id=nv.condition_id AND vt.slot=nv.slot
WHERE ct.Entry IN(26089,26090,26091,26092,27489,27478,23710,24495,10856,2842,19045,23896,24510,24468);
-- Preserve the existing spell and cadence; mark the EventAI caster's main attack.
UPDATE creature_ai_scripts SET action1_param3=action1_param3|256
WHERE id=3140302 AND creature_id=31403 AND action1_type=11 AND action1_param1=34446;
UPDATE creature_spell_list SET Flags=Flags|2
WHERE Id=429301 AND Position=4 AND SpellId=9613 AND Flags=0;
