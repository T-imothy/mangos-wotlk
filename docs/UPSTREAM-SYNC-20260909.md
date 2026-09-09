# Upstream synchronization — 2026-09-09

CMaNGOS authority: `2448954ee7611b2f580119f789bfd14116922250`.
Incoming upstream commits: 5.
Playerbots dependency: `7f45ddd8ae9fa8830f9231544ac2fae3126e92f5`.

c38787ca8f Unit: Name flag HITINFO_RAGE_GAIN and add use
e1376b0ff8 Unit: Fix crash due to rage gain code not being safeguarded for cleandamage
b7628aa67b Spell: Fix channels being interrupted by jumping when having SPELL_ATTR_EX5_ALLOW_ACTIONS_DURING_CHANNEL
ed6653a465 Fix 46607 channel being interrupted by jump
2448954ee7 Borean: Quest 11590

Existing custom safeguards and features are retained, including realm-wide WHO totals, faction filtering, spell-start lifetime protection, portable utilities, and the core loot-policy integration. No archived encounter experiments are restored. The encounter authority manifest now references the reviewed official revision.

Release dependency: deploy the matching Wrath database update 5873, changed ACID rows for creatures 25316 and 25474, spell-script bindings for 45625/45626 (remove obsolete 45630 binding), and the upstream spell 46607 AttributesEx5 fix together with this core. See the database repository docs/UPSTREAM-SYNC-20260909.md. Git synchronization does not apply these changes to a running database.
