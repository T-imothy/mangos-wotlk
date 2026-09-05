-- ManTech Arch3: remove a stale spell observed in WotLK bot/character data.
-- Runtime learn/cast guards prevent it from being reintroduced.
DELETE FROM `character_spell` WHERE `spell` = 75460;
DELETE FROM `pet_spell` WHERE `spell` = 75460;

CREATE TABLE IF NOT EXISTS `mantech_migration` (
  `id` varchar(64) NOT NULL,
  `applied_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `details` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

INSERT INTO `mantech_migration` (`id`,`details`)
VALUES ('arch3-characters-wotlk-v1','Remove stale unavailable spell 75460')
ON DUPLICATE KEY UPDATE `applied_at`=CURRENT_TIMESTAMP, `details`=VALUES(`details`);
