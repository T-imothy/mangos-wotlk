# Preserve pet follow recovery after a failed path

The existing FollowMovementGenerator::Move fallback relocates a non-player
follower near its owner when pathfinding/line-of-sight recovery is permitted.
It previously left an unfinished movement spline running. CreatureRelocation
does not stop that spline, so the next Unit::UpdateSplineMovement can overwrite
the recovered position with coordinates from the old path.

Stop the old spline and clear FOLLOW_MOVE before the existing relocation.
This keeps its destination/eligibility checks, normal paths, player teleport
handling, reaction modes and pet ownership unchanged. No new teleport or
distance threshold is introduced.

The regression compiles native FollowMovementGenerator::Move,
Unit::InterruptMoving and Unit::UpdateSplineMovement against boundary fakes
for pathfinding, relocation and spline timing/positions. NOPATH, SHORTCUT and
LOS recovery must remain at the recovered position after the next tick.
Denied recovery, normal follow and player teleport are also checked. The old
code fails all three recovery scenarios; the corrected code passes.

Run tests/mantech/test_follow_recovery.py in a Visual Studio developer shell.
This isolates a real recovery defect; it is not a complete live reproduction
of the reported hunter incident or a claim that every disappearance is fixed.
The captured pet stack changed from named-murloc chase to Follow(owner), which
motivated reviewing this path. Off-screen map-update behavior remains a
separate lead if the live issue persists. No client, database or config change.
