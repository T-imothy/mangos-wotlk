-- Retain every obsolete row in persistent archive tables before removing active copies.
-- Only the known ICC C++ replacements and currently unreferenced loot IDs qualify.
-- Do not apply without the preceding warning-repairs migration.
CREATE TABLE IF NOT EXISTS mantech_archive_20260921_eventai LIKE creature_ai_scripts;
CREATE TABLE IF NOT EXISTS mantech_archive_20260921_reference LIKE reference_loot_template;
CREATE TEMPORARY TABLE mantech_unused_icc_refs (entry INT UNSIGNED PRIMARY KEY);
INSERT INTO mantech_unused_icc_refs SELECT DISTINCT entry FROM reference_loot_template
WHERE entry IN (45103,65001,65002,65004,65005,65006,65007,65008,65009,65010,65011,65012,65013,65014,65015,65016,65017,65020,65031,65032,65033,65034,65101,65102,65103,65104,65105,65106);
DELETE candidate FROM mantech_unused_icc_refs candidate JOIN creature_loot_template used ON used.mincountOrRef=-CAST(candidate.entry AS SIGNED);
DELETE candidate FROM mantech_unused_icc_refs candidate JOIN disenchant_loot_template used ON used.mincountOrRef=-CAST(candidate.entry AS SIGNED);
DELETE candidate FROM mantech_unused_icc_refs candidate JOIN fishing_loot_template used ON used.mincountOrRef=-CAST(candidate.entry AS SIGNED);
DELETE candidate FROM mantech_unused_icc_refs candidate JOIN gameobject_loot_template used ON used.mincountOrRef=-CAST(candidate.entry AS SIGNED);
DELETE candidate FROM mantech_unused_icc_refs candidate JOIN item_loot_template used ON used.mincountOrRef=-CAST(candidate.entry AS SIGNED);
DELETE candidate FROM mantech_unused_icc_refs candidate JOIN mail_loot_template used ON used.mincountOrRef=-CAST(candidate.entry AS SIGNED);
DELETE candidate FROM mantech_unused_icc_refs candidate JOIN milling_loot_template used ON used.mincountOrRef=-CAST(candidate.entry AS SIGNED);
DELETE candidate FROM mantech_unused_icc_refs candidate JOIN pickpocketing_loot_template used ON used.mincountOrRef=-CAST(candidate.entry AS SIGNED);
DELETE candidate FROM mantech_unused_icc_refs candidate JOIN prospecting_loot_template used ON used.mincountOrRef=-CAST(candidate.entry AS SIGNED);
DELETE candidate FROM mantech_unused_icc_refs candidate JOIN reference_loot_template used ON used.mincountOrRef=-CAST(candidate.entry AS SIGNED);
DELETE candidate FROM mantech_unused_icc_refs candidate JOIN skinning_loot_template used ON used.mincountOrRef=-CAST(candidate.entry AS SIGNED);
DELETE candidate FROM mantech_unused_icc_refs candidate JOIN spell_loot_template used ON used.mincountOrRef=-CAST(candidate.entry AS SIGNED);
START TRANSACTION;
INSERT IGNORE INTO mantech_archive_20260921_eventai SELECT ai.* FROM creature_ai_scripts ai JOIN creature_template c ON c.entry=ai.creature_id WHERE ((c.entry IN (37122,37123,37124,37125,37491,37493,37494,37495) AND c.ScriptName='npc_argent_captain') OR (c.entry=37126 AND c.ScriptName='boss_sister_svalna') OR (c.entry=37129 AND c.ScriptName='npc_crok_scourgebane')) AND c.AIName='';
DELETE ai FROM creature_ai_scripts ai JOIN creature_template c ON c.entry=ai.creature_id WHERE ((c.entry IN (37122,37123,37124,37125,37491,37493,37494,37495) AND c.ScriptName='npc_argent_captain') OR (c.entry=37126 AND c.ScriptName='boss_sister_svalna') OR (c.entry=37129 AND c.ScriptName='npc_crok_scourgebane')) AND c.AIName='';
INSERT IGNORE INTO mantech_archive_20260921_reference
SELECT loot.* FROM reference_loot_template loot JOIN mantech_unused_icc_refs candidate USING (entry);
DELETE loot FROM reference_loot_template loot JOIN mantech_unused_icc_refs candidate USING (entry);
COMMIT;
DROP TEMPORARY TABLE mantech_unused_icc_refs;
