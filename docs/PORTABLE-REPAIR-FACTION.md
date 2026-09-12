# Portable repair hammer faction

Spell script spell_mantech_portable_repair changes custom creature 65001 to faction 12 (Alliance) or 29 (Horde) only when summoned by custom item 65001. Native engineering casts and creature templates are untouched. The native summon duration, cooldown and vendor/repair services remain unchanged. Wrath's working MOLL-E mailbox is unchanged.

Apply sql/custom/world/20260912_01_portable_repair_faction.sql with this binary and restart the world. No client/addon patch is needed.

The faction callback checks `PortableRepairVendor::CREATURE_ENTRY`. The native summon entry (24780) has already been remapped by `PortableRepairVendor::ResolveSummonEntry` before that callback. Regression tests compile this production resolver and pass its result to the actual spell script.
