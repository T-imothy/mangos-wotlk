# ManTech ICC cumulative restoration

The September 15 integration included the saved `icc-master` commits but missed
later uncommitted work in the cumulative test tree. This follow-up restores that
work without replacing the current ManTech core or reverting newer upstream code.

## Source provenance

The recovered source is the surviving `core-icc-cumulative-v2` working tree from
the August 20 ICC workspace, which produced the August 28 cumulative V3 and
September 1 KW-review builds discussed in the **ICC rework** task. The original
working tree remains untouched; its diff and changed files were preserved before
integration. Its Git HEAD alone (`0639125ef`) does not describe its final contents.

Recovered behavior includes:

- Svalna/Crok/captain escort, active loading, resurrection, combat and wipe cleanup.
- Blood Prince invocation order and timing; Blood Queen and Rotface timing.
- Crimson Hall Tactician grouping and Empowered Blood paired auras.
- September 1 Putricide selectors, native Abomination class/energy data,
  puddle growth, phase actions, laboratory movement and cleanup.
- The matching Spire Frostwyrm StringId completion check.
- Vehicle cleanup when a dynamically grouped creature fails to load.

Marrowgar, Deathwhisper, Gunship, Saurfang, Festergut, Sindragosa, Valithria and
Lich King source already matched the surviving cumulative tree. Their previous
restoration and rewards remain present. Unrelated old quest/aura implementations
were not copied over newer upstream and ManTech changes.

## Gunship crash

The September 16 crash from baseline `da4274a7` aborts at the player-only assertion
in `MapManager::CreateMap`, called by `Transport::TeleportTransport`. Continent
partition selection returns zero for ICC map 631. Comparing that zero against
the raid's nonzero instance ID incorrectly requested a map transfer and passed
a transport to a method requiring a player for instance creation.

Same-map dungeon/raid transports now retain their instance ID. Partition routing
on maps 0/1 remains enabled. The `icc_transport_instances` test covers both paths
and phased map 609.

## Required paired data

Use `T-imothy/wotlk-db:mantech-wotlk` updates **5900 through 5920**, in order.
The last two files restore cumulative V3/review data and explicitly install ICC
core script/spell bindings. Compiling does not run these SQL files. For a world
that has not received the earlier ICC updates, applying only 5919/5920 is not enough.

The observed production world `mangos_wotlk` lacked the Frostwyrm, Gunship hull,
Svalna, frost-jet and Abomination bindings. This is separate from the transport
code crash. Baseline publication alone does not update that database.

## Verification limits

All 17 affected/related ICC and native translation units passed MSVC syntax/type
checks. Updates 5900–5920 executed twice successfully on repository world data
upgraded to the current core schema in a private MySQL 8.0.46 instance. That
instance was then shut down. These checks do not claim a new full in-game raid run.
