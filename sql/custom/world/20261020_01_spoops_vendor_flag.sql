-- Spoops is a Hallow's End questgiver, not a vendor. The live template
-- advertised vendor inventory without any entries or vendor template.
-- Preserve gossip/quest flags and refuse to change a populated vendor.
UPDATE creature_template AS c
SET c.NpcFlags = c.NpcFlags & ~128
WHERE c.Entry = 15309 AND c.VendorTemplateId = 0
  AND NOT EXISTS (SELECT 1 FROM npc_vendor AS v WHERE v.entry = c.Entry);
