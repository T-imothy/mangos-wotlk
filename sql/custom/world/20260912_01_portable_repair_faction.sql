-- Only the repair bot summoned by custom item 65001 receives the player's faction.
-- Native engineering casts, templates and the working Wrath mailbox are unchanged.
INSERT INTO spell_scripts (Id, ScriptName)
SELECT 44389, 'spell_mantech_portable_repair'
WHERE NOT EXISTS (SELECT 1 FROM spell_scripts WHERE Id=44389);
