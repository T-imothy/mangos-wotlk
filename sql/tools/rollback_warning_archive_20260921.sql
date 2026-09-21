-- Restore archived definitions without overwriting any newer active rows.
INSERT IGNORE INTO creature_ai_scripts SELECT * FROM mantech_archive_20260921_eventai;
INSERT IGNORE INTO reference_loot_template SELECT * FROM mantech_archive_20260921_reference;
-- Retain archive tables as backups.
