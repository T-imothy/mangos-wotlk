# Portable repair hammer: Wrath cached item spell compatibility

Live world DB verified read-only: item 65001 uses spell 28020, on-use trigger,
1800000 ms cooldown, and spell_mantech_portable_repair_carrier binding. Creature
65001 is present with repair/vendor flags. No DB changes are needed.

Wrath CMSG_USE_ITEM carries a client-supplied spell ID. CastItemUseSpell skips
entries that do not match it, so a cache still advertising the old spell 44389
can produce no cast at all. This is a verified code path; the reporting user's
actual packet/cache has not yet been captured, so other causes remain possible.

HandleUseItemOpcode now translates only item 65001 / old spell 44389 to 28020
when its authoritative first spell is the new on-use carrier. This occurs after
bag/slot/GUID validation and retains all normal item, spell and cooldown checks.
It also sends the authoritative item query response to refresh the client.
Other items and arbitrary spell IDs are untouched. Classic/TBC send an item
spell index and do not need this compatibility translation.

Source/API review and CMake configuration only; no binary compiled. Test a
stale-cache hammer, a fresh-cache hammer, repeat use during cooldown, relog with
cooldown, and native engineering repair items on Wrath. A fresh-cache failure
would require further tracing rather than attributing it to this path.

Immediate diagnostic workaround: fully exit the Wrath client, rename its Cache
folder, relaunch and try the hammer when it is off cooldown. This needs no server
restart. Permanent compatibility fix requires a Wrath rebuild/deploy/restart.
