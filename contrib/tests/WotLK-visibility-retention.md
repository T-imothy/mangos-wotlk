> Historical review notes. The integrated release preserves current Arch4 timing and omits the extra telemetry described below. See doc/RETENTION-TRAVEL-RELEASE-20260914.md.

# WotLK visibility-retention correction

Port of the TBC correction `efc8930798880853ecf660c7274118a0df173e5a` onto the reconciled WotLK baseline `703f12a9827bbfd6c8760b3dd5508fb59deeb2c7`.
Shared Playerbots remains unchanged at `407f4cd53ddcde686fed3dc3488fd798bfb53364`.

## Scope

- Complete existing out-of-range visibility reconciliation before the bot-only client-notification return.
- Remove orphaned forward/reverse visibility GUIDs when the corresponding object/player no longer exists on the map.
- Bound diagnostic-only removed-object history to 4096 unique entries per map; no gameplay object count or spawn behavior is capped.
- Add the same existing-setting-controlled `VISIBILITY_SUMMARY` and rate-limited `SLOW_OBJECT_SEND` telemetry as the TBC correction.
- Preserve Wrath's existing moving-transport exceptions, phase handling, vehicle-passenger removal, and aura-packet implementation. No TBC aura code is substituted.
- Inherit all reconciled ManTech baseline changes, including the previously approved movement-height guard. No shared Playerbot, configuration, or database changes are required.

The old bot early return is present in the deployed WotLK Arch 3 source `cfa553094e`; it bypasses visibility cleanup while bot camera visits still populate those records. Fixing this defect does not establish that every source of memory growth or server delay is resolved.

## Native regression checks

Run from an MSVC developer shell:

```
python -B contrib/tests/visibility_retention_regression.py --baseline
python -B contrib/tests/visibility_retention_regression.py
python -B contrib/tests/movement_height_regression.py
```

The baseline check must reproduce the old roaming-retention failure. The corrected checks compile the actual WotLK notifier, Player::RemoveAtClient, reverse-link cleanup loop, and bounded-history method against controlled interfaces, with playerbots enabled and disabled. Cases cover repeated roaming, missing objects, normal human notification, bot notification suppression, transport passengers, moving transports, overridden visibility, phase changes, Wrath vehicle-passenger recursive removal, aura notifications, orphan reverse links, and 100000 history entries.

These are focused native tests, not a full client/gameplay simulation.

## Runtime verification

Keep the existing 4000-bot population and production settings. Compare memory and processing-time trends over a similar runtime to the pre-fix run; a fresh restart by itself lowers memory.

Check normal visibility/despawning, bot follow and combat, dungeon entry/exit, stealth reveal, a boat/zeppelin trip, and a Wrath vehicle encounter. The patch uses existing visibility and vehicle-removal paths rather than replacing these systems.

The five-minute visibility summary reads collection sizes, not all GUID contents. Slow-send timing is controlled by the existing PerformanceLog thresholds and detail interval. Worker-wait time is included in build time, and recipient batches are not unique recipient counts. All diagnostics append to the existing Performance.log; no additional runtime log file is introduced.
