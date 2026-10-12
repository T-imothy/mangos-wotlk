-- ManTech wotlk reviewed upstream and long-run fixes, 2026-10-11.
-- Matching WORLD database only. Back up affected tables before applying.

-- Incremental source: ACID/acid_wotlk.sql
UPDATE `creature_ai_scripts` SET `event_type`='1',`event_chance`='35',`event_flags`='1',`event_param1`='10000',`event_param2`='30000',`event_param3`='30000',`event_param4`='55000',`action1_type`='53',`action1_param1`='21268',`comment`='Stormforged Taskmaster - (35% chance) START_RELAY_SCRIPT on OOC Timer' WHERE `id` <=> '2936903' AND (`event_type` <=> '5' OR `event_type` <=> '1') AND (`event_chance` <=> '30' OR `event_chance` <=> '35') AND (`event_flags` <=> '0' OR `event_flags` <=> '1') AND (`event_param1` <=> '0' OR `event_param1` <=> '10000') AND (`event_param2` <=> '0' OR `event_param2` <=> '30000') AND (`event_param3` <=> '1' OR `event_param3` <=> '30000') AND (`event_param4` <=> '0' OR `event_param4` <=> '55000') AND (`action1_type` <=> '1' OR `action1_type` <=> '53') AND (`action1_param1` <=> '30238' OR `action1_param1` <=> '21268') AND (`comment` <=> 'Stormforged Taskmaster - Chance to Say on Player Kill' OR `comment` <=> 'Stormforged Taskmaster - (35% chance) START_RELAY_SCRIPT on OOC Timer');
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`event_param5`,`event_param6`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) SELECT '2936904','29369','7','0','100','0','0','0','0','0','0','0','53','21269','0','0','0','0','0','0','0','0','0','0','Stormforged Taskmaster - START_RELAY_SCRIPT on Evade' WHERE NOT EXISTS (SELECT 1 FROM `creature_ai_scripts` WHERE `id` <=> '2936904');

-- Source: Updates/5878_StormPeaks_TheFrozenMine.sql
-- Storm Peaks: The Frozen Mine

-- c.29384 Captive Mechagnome
DELETE FROM creature_addon WHERE guid IN (527747,527798,527799,527801,527803,527813,527814,527815,527816,527817,527818,527819,527820);
DELETE FROM creature_movement WHERE id IN (527747,527798,527799,527801,527803,527813,527814,527815,527816,527817,527818,527819,527820);
DELETE FROM game_event_creature WHERE guid IN (527747,527798,527799,527801,527803,527813,527814,527815,527816,527817,527818,527819,527820);
DELETE FROM game_event_creature_data WHERE guid IN (527747,527798,527799,527801,527803,527813,527814,527815,527816,527817,527818,527819,527820);
DELETE FROM creature_battleground WHERE guid IN (527747,527798,527799,527801,527803,527813,527814,527815,527816,527817,527818,527819,527820);
DELETE FROM creature_linking WHERE guid IN (527747,527798,527799,527801,527803,527813,527814,527815,527816,527817,527818,527819,527820);
DELETE FROM creature WHERE guid IN (527747,527798,527799,527801,527803,527813,527814,527815,527816,527817,527818,527819,527820);
INSERT INTO creature (guid, id, map, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecsmin, spawntimesecsmax, spawndist, MovementType) VALUES
(527747,29384,571,1,1,7726.9253,-9.089301,865.71344,1.535889744758605957,300,300,0,0),
(527798,29384,571,1,1,7736.012,-36.465385,869.1368,3.333578824996948242,300,300,0,0),
(527799,29384,571,1,1,7745.44,-55.28776,870.79114,2.617993831634521484,300,300,0,0),
(527801,29384,571,1,1,7717.46,-10.003038,866.54846,3.001966238021850585,300,300,0,0),
(527803,29384,571,1,1,7747.329,-47.022243,869.69885,0.994837641716003417,300,300,0,0),
(527813,29384,571,1,1,7836.1235,-68.48221,880.6543,6.108652114868164062,300,300,0,0),
(527814,29384,571,1,1,7827.557,-65.01367,882.1137,3.211405754089355468,300,300,0,0),
(527815,29384,571,1,1,7831.0347,-46.304253,882.2456,5.864306449890136718,300,300,0,0),
(527816,29384,571,1,1,7765.5947,3.372504,865.40204,0.907571196556091308,300,300,0,0),
(527817,29384,571,1,1,7743.0034,-25.287436,867.45264,0.226892799139022827,300,300,0,0),
(527818,29384,571,1,1,7768.4976,-16.321507,864.4009,4.97418832778930664,300,300,0,0),
(527819,29384,571,1,1,7779.954,-7.854275,863.28754,0.436332315206527709,300,300,0,0),
(527820,29384,571,1,1,7815.5645,-36.965496,879.7954,1.623156189918518066,300,300,0,0);
UPDATE creature SET position_x = 7833.2153, position_y = -56.419815, position_z = 881.8431, orientation = 0.541052043437957763, spawndist = 0, MovementType = 0 WHERE guid = 524683;
UPDATE creature SET position_x = 7823.1226, position_y = -55.006077, position_z = 882.2609, orientation = 4.834561824798583984, spawndist = 0, MovementType = 0 WHERE guid = 524684;
UPDATE creature SET position_x = 7813.856, position_y = -53.62424, position_z = 883.7043, orientation = 5.078907966613769531, spawndist = 0, MovementType = 0 WHERE guid = 524685;
UPDATE creature SET position_x = 7823.6665, position_y = -37.51964, position_z = 882.7526, orientation = 1.692969322204589843, spawndist = 0, MovementType = 0 WHERE guid = 524686;
UPDATE creature SET position_x = 7831.7163, position_y = -38.967556, position_z = 881.6061, orientation = 4.956735134124755859, spawndist = 0, MovementType = 0 WHERE guid = 524687;
UPDATE creature SET position_x = 7794.343, position_y = -48.1097, position_z = 881.72894, orientation = 3.577924966812133789, spawndist = 0, MovementType = 0 WHERE guid = 524688;
UPDATE creature SET position_x = 7794.047, position_y = -60.66504, position_z = 880.9378, orientation = 5.79449319839477539, spawndist = 0, MovementType = 0 WHERE guid = 524689;
UPDATE creature SET position_x = 7808.4854, position_y = -40.310764, position_z = 879.52576, orientation = 3.211405754089355468, spawndist = 0, MovementType = 0 WHERE guid = 524690;
UPDATE creature SET position_x = 7756.3345, position_y = -66.37359, position_z = 875.0351, orientation = 4.694935798645019531, spawndist = 0, MovementType = 0 WHERE guid = 524691;
UPDATE creature SET position_x = 7758.001, position_y = -58.602215, position_z = 874.88245, orientation = 0.959931075572967529, spawndist = 0, MovementType = 0 WHERE guid = 524692;
UPDATE creature SET position_x = 7793.433, position_y = -35.11556, position_z = 879.70123, orientation = 2.0245819091796875, spawndist = 0, MovementType = 0 WHERE guid = 524693;
UPDATE creature SET position_x = 7802.9395, position_y = -38.81966, position_z = 879.3645, orientation = 4.101523876190185546, spawndist = 0, MovementType = 0 WHERE guid = 524694;
UPDATE creature SET position_x = 7781.9297, position_y = -40.24425, position_z = 879.1712, orientation = 2.251474618911743164, spawndist = 0, MovementType = 0 WHERE guid = 524695;
UPDATE creature SET position_x = 7812.1797, position_y = -94.02344, position_z = 880.2368, orientation = 4.660028934478759765, spawndist = 0, MovementType = 0 WHERE guid = 524696;
UPDATE creature SET position_x = 7757.494, position_y = -4.936957, position_z = 865.0462, orientation = 3.96189737319946289, spawndist = 0, MovementType = 0 WHERE guid = 524697;
UPDATE creature SET position_x = 7828.539, position_y = -87.213, position_z = 882.2543, orientation = 0.104719758033752441, spawndist = 0, MovementType = 0 WHERE guid = 524698;
UPDATE creature SET position_x = 7739.954, position_y = 3.092556, position_z = 865.9239, orientation = 0.698131680488586425, spawndist = 0, MovementType = 0 WHERE guid = 524699;
UPDATE creature SET position_x = 7718.132, position_y = 1.124241, position_z = 867.4224, orientation = 2.181661605834960937, spawndist = 0, MovementType = 0 WHERE guid = 524700;
UPDATE creature SET position_x = 7725.975, position_y = -16.970161, position_z = 867.8891, orientation = 0.174532920122146606, spawndist = 0, MovementType = 0 WHERE guid = 524701;
DELETE FROM creature_addon WHERE guid IN(SELECT guid FROM creature WHERE id = 29384);
DELETE FROM creature_template_addon WHERE entry IN (29384);
INSERT INTO creature_template_addon (entry, mount, stand_state, sheath_state, pvp_flags, emote, moveflags, auras) VALUES
(29384,0,0,1,0,233,0,NULL);

-- c.29369 Stormforged Taskmaster
DELETE FROM creature_addon WHERE guid IN (527730,527821,527872);
DELETE FROM creature_movement WHERE id IN (527730,527821,527872);
DELETE FROM game_event_creature WHERE guid IN (527730,527821,527872);
DELETE FROM game_event_creature_data WHERE guid IN (527730,527821,527872);
DELETE FROM creature_battleground WHERE guid IN (527730,527821,527872);
DELETE FROM creature_linking WHERE guid IN (527730,527821,527872);
DELETE FROM creature WHERE guid IN (527730,527821,527872);
INSERT INTO creature (guid, id, map, spawnMask, phaseMask, position_x, position_y, position_z, orientation, spawntimesecsmin, spawntimesecsmax, spawndist, MovementType) VALUES
(527730,29369,571,1,1,7796.762,-50.188694,882.3017,0,300,300,0,4),
(527821,29369,571,1,1,7726.0645,0.671767,867.3513,0,300,300,0,4),
(527872,29369,571,1,1,7730.3213,-23.281912,867.55054,0,300,300,0,4);
UPDATE creature SET position_x = 7842.5425, position_y = -110.64627, position_z = 882.8648, orientation = 0, spawndist = 0, MovementType = 4 WHERE guid = 524563; -- linear
UPDATE creature SET position_x = 7824.0103, position_y = -133.21658, position_z = 883.1015, orientation = 0, spawndist = 0, MovementType = 4 WHERE guid = 524564; -- linear
UPDATE creature SET position_x = 7835.0044, position_y = -35.802517, position_z = 881.1093, orientation = 0, spawndist = 0, MovementType = 4 WHERE guid = 524565; -- linear
UPDATE creature SET position_x = 7832.377, position_y = -77.63553, position_z = 882.18726, orientation = 0, spawndist = 0, MovementType = 2 WHERE guid = 524566;
DELETE FROM creature_movement WHERE id IN (524563,524564,524565,524566,527730,527821,527872);
INSERT INTO creature_movement (`id`, `Point`, `PositionX`, `PositionY`, `PositionZ`, `Orientation`, `WaitTime`, `ScriptId`) VALUES
-- 524563
(524563,1,7842.5425,-110.64627,882.8648,100,500,0),
(524563,2,7839.189,-106.30838,882.96606,100,0,0),
(524563,3,7833.7085,-100.74078,881.4133,100,0,0),
(524563,4,7827.7676,-96.58257,881.1446,100,0,0),
(524563,5,7822.916,-90.537544,880.74774,100,0,0),
(524563,6,7822.8447,-83.79514,881.602,100,500,0),
-- 524564
(524564,1,7824.0103,-133.21658,883.1015,100,500,0),
(524564,2,7834.078,-130.53917,882.26056,100,0,0),
(524564,3,7842.921,-124.64529,880.8713,100,0,0),
(524564,4,7851.019,-120.68349,880.71344,100,0,0),
(524564,5,7856.9653,-115.75087,881.2493,100,500,0),
-- 524565
(524565,1,7835.0044,-35.802517,881.1093,100,500,0),
(524565,2,7831.4062,-36.4311,881.7886,100,0,0),
(524565,3,7825.858,-41.522785,882.7488,100,0,0),
(524565,4,7825.4517,-46.997395,881.8351,100,0,0),
(524565,5,7825.428,-52.692165,881.8121,100,0,0),
(524565,6,7829.174,-59.9388,881.9704,100,0,0),
(524565,7,7830.633,-64.98741,881.8168,100,500,0),
-- 524566
(524566,1,7832.377,-77.63553,882.18726,100,0,0),
(524566,2,7837.315,-75.350296,880.3026,100,0,0),
(524566,3,7827.683,-77.09928,882.81104,100,0,0),
(524566,4,7824.5723,-79.80317,882.5861,100,0,0),
-- 527730
(527730,1,7796.762,-50.188694,882.3017,100,500,0),
(527730,2,7802.53,-48.544163,883.21814,100,0,0),
(527730,3,7806.8228,-50.18902,883.62915,100,0,0),
(527730,4,7810.619,-51.102974,883.7081,100,0,0),
(527730,5,7814.9263,-51.181965,883.2064,100,0,0),
(527730,6,7818.904,-50.98427,882.66345,100,500,0),
-- 527821
(527821,1,7726.0645,0.671767,867.3513,100,500,0),
(527821,2,7732.2876,0.640082,866.2783,100,0,0),
(527821,3,7738.8896,-1.104058,866.25586,100,0,0),
(527821,4,7747.007,-1.096788,865.7106,100,0,0),
(527821,5,7755.479,-0.063151,865.96466,100,0,0),
(527821,6,7761.0825,1.824002,865.41266,100,0,0),
(527821,7,7766.3066,-2.848633,864.75476,100,0,0),
(527821,8,7771.362,-8.772439,864.01984,100,0,0),
(527821,9,7776.58,-11.874777,863.3382,100,500,0),
-- 527872
(527872,1,7730.3213,-23.281912,867.55054,100,500,0),
(527872,2,7736.8145,-27.022352,867.09174,100,0,0),
(527872,3,7741.8926,-35.71669,868.4073,100,0,0),
(527872,4,7742.489,-41.61561,868.16895,100,0,0),
(527872,5,7745.9194,-49.20703,869.6114,100,0,0),
(527872,6,7748.4785,-53.62652,870.92737,100,500,0);
DELETE FROM dbscripts_on_relay WHERE id IN (21268,21269);
INSERT INTO dbscripts_on_relay (id, delay, command, datalong, datalong2, datalong3, buddy_entry, search_radius, data_flags, dataint, dataint2, dataint3, dataint4, x, y, z, o, comments) VALUES
(21268,1,31,29384,10,0,0,0,0,0,0,0,0,0,0,0,0,'Part of Stormforged Taskmaster 29369 EAI: search for 29384'),
(21268,100,32,1,0,0,0,0,0x004,0,0,0,0,0,0,0,0,'Part of Stormforged Taskmaster 29369 EAI: pause wps'),
(21268,1000,36,0,0,0,29384,20,1,0,0,0,0,0,0,0,0,'Part of Stormforged Taskmaster 29369 EAI: face target'),
(21268,2500,1,25,0,0,0,0,0,0,0,0,0,0,0,0,0,'Part of Stormforged Taskmaster 29369 EAI: emote'),
(21268,2600,0,20392,0,0,0,0,0,0,0,0,0,0,0,0,0,'Part of Stormforged Taskmaster 29369 EAI: random say'),
(21268,5000,32,0,0,0,0,0,0x004,0,0,0,0,0,0,0,0,'Part of Stormforged Taskmaster 29369 EAI: unpause wps'),
(21269,1,32,0,0,0,0,0,0x004,0,0,0,0,0,0,0,0,'Part of Stormforged Taskmaster 29369 EAI: unpause wps');
DELETE FROM `dbscript_random_templates` WHERE `id`=20392;
INSERT INTO `dbscript_random_templates` (`id`, `type`, `target_id`, `chance`, `comments`) VALUES
(20392,0,30233,0,'Stormforged Taskmaster 29369 - Random Text 1'),
(20392,0,30234,0,'Stormforged Taskmaster 29369 - Random Text 2'),
(20392,0,30235,0,'Stormforged Taskmaster 29369 - Random Text 3'),
(20392,0,30236,0,'Stormforged Taskmaster 29369 - Random Text 4'),
(20392,0,30237,0,'Stormforged Taskmaster 29369 - Random Text 5'),
(20392,0,30238,0,'Stormforged Taskmaster 29369 - Random Text 6'),
(20392,0,30239,0,'Stormforged Taskmaster 29369 - Random Text 7'),
(20392,0,30240,0,'Stormforged Taskmaster 29369 - Random Text 8');


-- Native improved unholy presence definition.
INSERT INTO spell_template(Id,Category,Dispel,Mechanic,Attributes,AttributesEx,AttributesEx2,AttributesEx3,AttributesEx4,AttributesEx5,AttributesEx6,AttributesEx7,Stances,Stances2,StancesNot,StancesNot2,Targets,TargetCreatureType,RequiresSpellFocus,FacingCasterFlags,CasterAuraState,TargetAuraState,CasterAuraStateNot,TargetAuraStateNot,CasterAuraSpell,TargetAuraSpell,ExcludeCasterAuraSpell,ExcludeTargetAuraSpell,CastingTimeIndex,RecoveryTime,CategoryRecoveryTime,InterruptFlags,AuraInterruptFlags,ChannelInterruptFlags,ProcFlags,ProcChance,ProcCharges,MaxLevel,BaseLevel,SpellLevel,DurationIndex,PowerType,ManaCost,ManaCostPerlevel,ManaPerSecond,ManaPerSecondPerLevel,RangeIndex,Speed,ModalNextSpell,StackAmount,Totem1,Totem2,Reagent1,Reagent2,Reagent3,Reagent4,Reagent5,Reagent6,Reagent7,Reagent8,ReagentCount1,ReagentCount2,ReagentCount3,ReagentCount4,ReagentCount5,ReagentCount6,ReagentCount7,ReagentCount8,EquippedItemClass,EquippedItemSubClassMask,EquippedItemInventoryTypeMask,Effect1,Effect2,Effect3,EffectDieSides1,EffectDieSides2,EffectDieSides3,EffectRealPointsPerLevel1,EffectRealPointsPerLevel2,EffectRealPointsPerLevel3,EffectBasePoints1,EffectBasePoints2,EffectBasePoints3,EffectMechanic1,EffectMechanic2,EffectMechanic3,EffectImplicitTargetA1,EffectImplicitTargetA2,EffectImplicitTargetA3,EffectImplicitTargetB1,EffectImplicitTargetB2,EffectImplicitTargetB3,EffectRadiusIndex1,EffectRadiusIndex2,EffectRadiusIndex3,EffectApplyAuraName1,EffectApplyAuraName2,EffectApplyAuraName3,EffectAmplitude1,EffectAmplitude2,EffectAmplitude3,EffectMultipleValue1,EffectMultipleValue2,EffectMultipleValue3,EffectChainTarget1,EffectChainTarget2,EffectChainTarget3,EffectItemType1,EffectItemType2,EffectItemType3,EffectMiscValue1,EffectMiscValue2,EffectMiscValue3,EffectMiscValueB1,EffectMiscValueB2,EffectMiscValueB3,EffectTriggerSpell1,EffectTriggerSpell2,EffectTriggerSpell3,EffectPointsPerComboPoint1,EffectPointsPerComboPoint2,EffectPointsPerComboPoint3,EffectSpellClassMask1_1,EffectSpellClassMask1_2,EffectSpellClassMask1_3,EffectSpellClassMask2_1,EffectSpellClassMask2_2,EffectSpellClassMask2_3,EffectSpellClassMask3_1,EffectSpellClassMask3_2,EffectSpellClassMask3_3,SpellVisual,SpellVisual2,SpellIconID,ActiveIconID,SpellPriority,SpellName,SpellName2,SpellName3,SpellName4,SpellName5,SpellName6,SpellName7,SpellName8,SpellName9,SpellName10,SpellName11,SpellName12,SpellName13,SpellName14,SpellName15,SpellName16,Rank1,Rank2,Rank3,Rank4,Rank5,Rank6,Rank7,Rank8,Rank9,Rank10,Rank11,Rank12,Rank13,Rank14,Rank15,Rank16,ManaCostPercentage,StartRecoveryCategory,StartRecoveryTime,MaxTargetLevel,SpellFamilyName,SpellFamilyFlags,SpellFamilyFlags2,MaxAffectedTargets,DmgClass,PreventionType,StanceBarOrder,DmgMultiplier1,DmgMultiplier2,DmgMultiplier3,MinFactionId,MinReputation,RequiredAuraVision,TotemCategory1,TotemCategory2,AreaId,SchoolMask,RuneCostID,SpellMissileID,PowerDisplayId,EffectBonusCoefficient1,EffectBonusCoefficient2,EffectBonusCoefficient3,EffectBonusCoefficientFromAP1,EffectBonusCoefficientFromAP2,EffectBonusCoefficientFromAP3,SpellDescriptionVariableID,SpellDifficultyId,IsServerSide,AttributesServerside) SELECT '65095','0','0','0','2835677584','268436480','1','1114112','2097152','0','4096','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','101','0','0','55','55','21','0','0','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','0','6','0','0','1','1','1','0','0','0','-1','-1','-1','0','0','0','1','1','1','0','0','0','0','0','0','110','0','0','0','0','0','0','0','0','0','0','0','0','0','0','5','5','5','3','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','2633','122','0','Improved Unholy Presence','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','0','0','0','0','15','0','0','0','0','0','0','1','1','1','0','0','0','0','0','0','32','2318','0','0','0','0','0','0','0','0','0','0','0','0' WHERE NOT EXISTS (SELECT 1 FROM spell_template WHERE Id=65095);

-- Incremental source: sql/scriptdev2/spell.sql
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 68576,'spell_eject_all_passengers' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 68576);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 69674,'spell_rotface_mutated_infection' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 69674);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 69678,'spell_gunship_rocket_artillery' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 69678);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 69679,'spell_gunship_rocket_artillery_explosion' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 69679);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 69705,'spell_gunship_below_zero' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 69705);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 70227,'spell_icc_empowered_blood' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 70227);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 70304,'spell_icc_empowered_blood_controller' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 70304);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 70609,'spell_gunship_rocket_artillery' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 70609);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 70739,'spell_icc_geist_alarm' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 70739);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 70740,'spell_icc_geist_alarm' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 70740);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 71224,'spell_rotface_mutated_infection' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 71224);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 73022,'spell_rotface_mutated_infection' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 73022);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 73023,'spell_rotface_mutated_infection' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 73023);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 48263,'spell_frost_presence' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 48263);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 48265,'spell_unholy_presence' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 48265);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 48266,'spell_blood_presence' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 48266);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 50365,'spell_improved_blood_presence' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 50365);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 50371,'spell_improved_blood_presence' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 50371);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 50384,'spell_improved_frost_presence' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 50384);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 50385,'spell_improved_frost_presence' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 50385);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 50391,'spell_improved_unholy_presence' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 50391);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 50392,'spell_improved_unholy_presence' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 50392);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 73077,'spell_gunship_rocket_pack' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 73077);
INSERT INTO `spell_scripts` (`Id`,`ScriptName`) SELECT 72340,'spell_gunship_victory_teleport' WHERE NOT EXISTS (SELECT 1 FROM `spell_scripts` WHERE `Id` <=> 72340);

-- Item 19874's passive is equipped once, never re-applied on each hit.
UPDATE item_template SET spelltrigger_1=1 WHERE entry=19874 AND spellid_1=25669 AND spelltrigger_1=2;


-- Scene steps use native relay guards; parent cleanup must still run.
UPDATE dbscripts_on_relay SET priority=1 WHERE id=14 AND delay=2000 AND command=26 AND buddy_entry=4952 AND priority=0;
DELETE FROM dbscripts_on_relay WHERE id=14 AND delay=2000 AND command=48 AND priority=0;
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('14','2000','0','48','512','0','0','0','0','4','0','0','0','0','0','0','0','0','0','0','0','ManTech training - remove own NPC immunity immediately before attack');
UPDATE dbscripts_on_relay SET datalong2=0 WHERE id=14 AND delay=35000 AND command=35 AND datalong=5 AND data_flags=4;
DELETE FROM dbscripts_on_relay WHERE id=1900014;
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1900014','0','0','31','4952','5','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','ManTech training - optional dummy cleanup guard');
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1900014','0','1','35','5','0','0','4952','5','1','0','0','0','0','0','0','0','0','0','0','0','ManTech training - reset only this dummy, never neighboring guards');
DELETE FROM dbscripts_on_relay WHERE id=14 AND delay=35000 AND command=45 AND datalong=1900014;
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('14','35000','1','45','1900014','0','0','0','0','4','0','0','0','0','0','0','0','0','0','0','0','ManTech training - optional paired dummy cleanup');
UPDATE dbscripts_on_relay SET delay=0 WHERE id IN(18027,18031) AND delay=1 AND priority=1 AND command=35;
DELETE FROM dbscripts_on_relay WHERE id=1908330;
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908330','0','0','31','0','5','0','28011','5','2048','0','0','0','0','0','0','0','0','0','0','0','ManTech Sunfury - optional actor guard for one scene step');
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908330','0','1','1','0','0','0','28011','5','2048','0','0','0','0','0','0','0','0','0','0','0','Netherstorm - StringID - EmoteStateNone');
UPDATE dbscripts_on_relay SET command=45,datalong=1908330,datalong2=0,datalong3=0,buddy_entry=0,search_radius=0,data_flags=0,dataint=0,dataint2=0,dataint3=0,dataint4=0,x=0,y=0,z=0,o=0,speed=0,condition_id=0 WHERE id=18033 AND delay=0 AND priority=0 AND command=1 AND buddy_entry=28011;
DELETE FROM dbscripts_on_relay WHERE id=1908331;
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908331','0','0','31','0','5','0','28011','5','2048','0','0','0','0','0','0','0','0','0','0','0','ManTech Sunfury - optional actor guard for one scene step');
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908331','0','1','36','0','0','0','28011','5','2051','0','0','0','0','0','0','0','0','0','0','0','Netherstorm - StringID - facing to Sunfury Warp-Master');
UPDATE dbscripts_on_relay SET command=45,datalong=1908331,datalong2=0,datalong3=0,buddy_entry=0,search_radius=0,data_flags=0,dataint=0,dataint2=0,dataint3=0,dataint4=0,x=0,y=0,z=0,o=0,speed=0,condition_id=0 WHERE id=18033 AND delay=1000 AND priority=0 AND command=36 AND buddy_entry=28011;
DELETE FROM dbscripts_on_relay WHERE id=1908332;
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908332','0','0','31','0','5','0','28011','5','2048','0','0','0','0','0','0','0','0','0','0','0','ManTech Sunfury - optional actor guard for one scene step');
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908332','0','1','36','0','0','0','28011','5','2049','0','0','0','0','0','0','0','0','0','0','0','Netherstorm - Sunfury Warp-Master - face StringID');
UPDATE dbscripts_on_relay SET command=45,datalong=1908332,datalong2=0,datalong3=0,buddy_entry=0,search_radius=0,data_flags=0,dataint=0,dataint2=0,dataint3=0,dataint4=0,x=0,y=0,z=0,o=0,speed=0,condition_id=0 WHERE id=18033 AND delay=1000 AND priority=1 AND command=36 AND buddy_entry=28011;
DELETE FROM dbscripts_on_relay WHERE id=1908333;
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908333','0','0','31','0','5','0','28011','5','2048','0','0','0','0','0','0','0','0','0','0','0','ManTech Sunfury - optional actor guard for one scene step');
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908333','0','1','1','6','0','0','28011','5','2048','0','0','0','0','0','0','0','0','0','0','0','Netherstorm - StringID - Emote OneShotQuestion');
UPDATE dbscripts_on_relay SET command=45,datalong=1908333,datalong2=0,datalong3=0,buddy_entry=0,search_radius=0,data_flags=0,dataint=0,dataint2=0,dataint3=0,dataint4=0,x=0,y=0,z=0,o=0,speed=0,condition_id=0 WHERE id=18033 AND delay=1000 AND priority=2 AND command=1 AND buddy_entry=28011;
DELETE FROM dbscripts_on_relay WHERE id=1908334;
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908334','0','0','31','0','5','0','28011','5','2048','0','0','0','0','0','0','0','0','0','0','0','ManTech Sunfury - optional actor guard for one scene step');
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908334','0','1','1','274','0','0','28011','5','2048','0','0','0','0','0','0','0','0','0','0','0','Netherstorm - StringID - Emote OneShotNo');
UPDATE dbscripts_on_relay SET command=45,datalong=1908334,datalong2=0,datalong3=0,buddy_entry=0,search_radius=0,data_flags=0,dataint=0,dataint2=0,dataint3=0,dataint4=0,x=0,y=0,z=0,o=0,speed=0,condition_id=0 WHERE id=18033 AND delay=4000 AND priority=0 AND command=1 AND buddy_entry=28011;
DELETE FROM dbscripts_on_relay WHERE id=1908335;
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908335','0','0','31','0','5','0','28011','5','2048','0','0','0','0','0','0','0','0','0','0','0','ManTech Sunfury - optional actor guard for one scene step');
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908335','0','1','36','1','0','0','28011','5','2048','0','0','0','0','0','0','0','0','0','0','0','Netherstorm - StringID - Reset Facing');
UPDATE dbscripts_on_relay SET command=45,datalong=1908335,datalong2=0,datalong3=0,buddy_entry=0,search_radius=0,data_flags=0,dataint=0,dataint2=0,dataint3=0,dataint4=0,x=0,y=0,z=0,o=0,speed=0,condition_id=0 WHERE id=18033 AND delay=7000 AND priority=0 AND command=36 AND buddy_entry=28011;
DELETE FROM dbscripts_on_relay WHERE id=1908336;
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908336','0','0','31','0','5','0','28011','5','2048','0','0','0','0','0','0','0','0','0','0','0','ManTech Sunfury - optional actor guard for one scene step');
INSERT INTO dbscripts_on_relay(`id`,`delay`,`priority`,`command`,`datalong`,`datalong2`,`datalong3`,`buddy_entry`,`search_radius`,`data_flags`,`dataint`,`dataint2`,`dataint3`,`dataint4`,`datafloat`,`x`,`y`,`z`,`o`,`speed`,`condition_id`,`comments`) VALUES('1908336','0','1','35','6','0','0','28011','5','2049','0','0','0','0','0','0','0','0','0','0','0','Netherstorm - Sunfury Warp-Master - SendAIEventB to StringID');
UPDATE dbscripts_on_relay SET command=45,datalong=1908336,datalong2=0,datalong3=0,buddy_entry=0,search_radius=0,data_flags=0,dataint=0,dataint2=0,dataint3=0,dataint4=0,x=0,y=0,z=0,o=0,speed=0,condition_id=0 WHERE id=18033 AND delay=9000 AND priority=2 AND command=35 AND buddy_entry=28011;
