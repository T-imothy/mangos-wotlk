# ManTech Wrath ICC baseline integration — 2026-09-15

The complete saved ICC integration line is restored to `mantech-wotlk` alongside
the current ManTech core changes. This is a source integration, not evidence
that a newly built executable or database update has been deployed.

## Provenance

- Starting core baseline: `c758f2ff7140c421f870606eb8a0454a67177a62`.
- Saved core `icc-master`: `f0f1f2ad46c95dc472bec9f7e1751474821373c3`.
- Saved DB `icc-master`: `13d9e8605d774fd3c64f320c4bf17f7f40b67fa5`.
- Later Professor Putricide core completion: `64dbbc9d004c47cfbb6f00fd00547cd9a2c54539`.
- Matching database work is consolidated on `T-imothy/wotlk-db:mantech-wotlk`.
  Its integration record lists the later encounter-data commits.

Individual PR branch names were removed during repository cleanup. Their exact
commit histories remain recoverable from the verified pre-cleanup bundles. No
extra remote integration or PR branch is required to build this baseline.

## Merge decisions

The saved combined branch predates several changes already accepted upstream.
The merge keeps the baseline's current Festergut implementation, difficulty
variants and spell handlers. It also keeps the upstream CombatAI implementations
for Spire Frostwyrms, the Putricide trap and pipe Fleshreapers, including their
StringId-based selection and corrected insect despawn duration.

The other restored work includes the raid encounters, gunship and nested
transport support, raid progression, achievements, elevator/rocket-pack
behavior, Geist Alarm, and Lich King/Putricide re-entry aura cleanup. The instance
merge preserves both encounter cleanup paths and uses the newer StringId-based
Light's Hammer gate. One duplicate valve switch case introduced by combining
the two histories was removed. The elevator implementation explicitly includes
the native transport type it uses.

## Database pairing and release

Use the paired Wrath DB baseline. The restored encounter migrations are numbered
`5900`–`5918`, after the existing `5896_restore_cmangos_encounters.sql` migration.
The older ICC filenames reused upstream migration numbers and preceded that
restoration; leaving them there could allow later SQL to undo restored state.
The updated `Updates/Instances/631_icecrown_citadel.sql` is part of the same set.

A future release must include both the new executable and its matching ICC
database/script bindings (`sql/scriptdev2/spell.sql` and `scriptdev2.sql`).
Follow the normal reviewed database-update procedure; this document does not
authorize running SQL against a live realm.

## Validation performed

- MSVC C++20 syntax/type checks passed for all 23 affected translation units
  and the preserved Festergut/header combination included in that total.
- All 12 primary boss/encounter source files match the latest saved individual
  encounter PR versions after Git text normalization.
- All 63 added spell bindings resolve to registered native scripts; no duplicate
  `(spell ID, script name)` pairs were introduced.
- ScriptName assignments in all 19 restored DB migrations resolve to registered
  scripts. Transaction delimiters, merge markers and whitespace checks passed.

These checks are not a full linked build, database execution, or gameplay test.
Raid behavior and a real database upgrade were not exercised during this merge.
