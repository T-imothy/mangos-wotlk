# Wrath LookingForGroup chat compatibility

Keep `Channel.RestrictedLfg = 0` in the deployed mangosd.conf. The distributed
template has the same value. Existing installations must update their actual
config; changing the template alone does not update it.

With the setting enabled, Channel::Join unconditionally refuses SEC_PLAYER
accounts on an LFG-flagged channel and emits CHAT_NOT_IN_LFG_NOTICE (0x21).
The installed 3.3.5 enUS patch-enUS-3.MPQ GlobalStrings.lua lacks that notice,
although the older locale archive defines it. ChatFrame.lua line 2802 tries
to format the missing global and produces the login nil-format Lua error.

The restriction predates this project's recent bot changes (the current
unconditional check dates to upstream commit 66d9c1363b2, March 2020).
Disabling it permits ordinary chat-channel membership. It does not force
bots into queues, enable matchmaking, or change dungeon eligibility.

This is a config-only release. Reload server configuration or restart the
world server, then retry channel join/relog. No client or addon edit is needed.
The executable remains the separately verified repair-vendor build 14d1372282bd.
The compiled fallback when the setting is absent is unchanged; retain the
explicit line in deployed configurations.
