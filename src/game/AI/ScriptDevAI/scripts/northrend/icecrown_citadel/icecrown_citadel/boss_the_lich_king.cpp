/* This file is part of the ScriptDev2 Project. See AUTHORS file for Copyright information
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: boss_the_lich_king
SD%Complete: 85%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"
#include "Movement/MoveSplineInit.h"
#include "Spells/Scripts/SpellScript.h"

enum
{
    SAY_INTRO_1                 = -1631158,
    SAY_INTRO_2                 = -1631159,
    SAY_INTRO_3                 = -1631160,
    SAY_INTRO_4                 = -1631161,
    SAY_INTRO_5                 = -1631162,
    SAY_AGGRO                   = -1631163,
    SAY_REMORSELESS_WINTER      = -1631164,
    SAY_SHATTER_ARENA           = -1631165,
    SAY_SUMMON_VALKYR           = -1631166,
    SAY_HARVEST_SOUL            = -1631167,
    SAY_FM_TERENAS_AID_1        = -1631168,
    SAY_FM_TERENAS_AID_2        = -1631169,
    SAY_FM_TERENAS_AID_3        = -1631170,
    SAY_FM_PLAYER_ESCAPE        = -1631171,
    SAY_FM_PLAYER_DEATH         = -1631172,
    SAY_SPECIAL_1               = -1631173,
    SAY_SPECIAL_2               = -1631174,
    SAY_LAST_PHASE              = -1631175,
    SAY_SLAY_1                  = -1631176,
    SAY_SLAY_2                  = -1631177,
    SAY_ENRAGE                  = -1631178,
    SAY_OUTRO_1                 = -1631179,
    SAY_OUTRO_2                 = -1631180,
    SAY_OUTRO_3                 = -1631181,
    SAY_OUTRO_4                 = -1631182,
    SAY_OUTRO_5                 = -1631183,
    SAY_OUTRO_6                 = -1631184,
    SAY_OUTRO_7                 = -1631185,
    SAY_OUTRO_8                 = -1631186,
    SAY_OUTRO_9                 = -1631187,
    SAY_OUTRO_10                = -1631188,
    SAY_OUTRO_11                = -1631189,
    SAY_OUTRO_12                = -1631190,
    SAY_OUTRO_13                = -1631191,
    SAY_OUTRO_14                = -1631192,
};

enum
{
    SPELL_BERSERK               = 47008,
    SPELL_SIT_EMOTE_NO_SHEATH   = 73220,
    SPELL_PLAGUE_AVOIDANCE      = 72846,

    // Intro
    SPELL_ICE_LOCK              = 71614,

    // Outro
    SPELL_FURY_OF_FROSTMOURNE   = 72350,
    SPELL_FURY_OF_FROSTMOURNE2  = 72351, // cannot resurect aura
    SPELL_RAISE_DEAD            = 71769,
    SPELL_THROW_FROSTMOURNE     = 73017, // 1
    SPELL_BROKEN_FROSTMOURNE    = 72398, // 2
    SPELL_SUMMON_FROSTMOURNE    = 72407, // 3 summon npc which casts 4 and 5 and LK enters this npc as vehicle
    SPELL_FROSTMOURNE_DESPAWN   = 72726, // 4
    SPELL_FROSTMOURNE_SPIRITS   = 72405, // 5
    SPELL_SOUL_BARRAGE          = 72305, // strangulation and sounds
    SPELL_PLAY_MOVIE            = 73159,

    // Tirion
    SPELL_LIGHTS_BLESSING       = 71797, // after 5secs smashes Ice Lock

    // Terenas Menethil
    SPELL_MASS_RESURRECTION     = 72429, // dummy
    SPELL_MASS_RESURRECTION2    = 72423, // actual res

    // Phase One
    SPELL_NECROTIC_PLAGUE       = 70337,
    SPELL_NECROTIC_PLAGUE_STACK = 70338,
    SPELL_INFEST                = 70541,
    SPELL_SUMMON_GHOULS         = 70358,
    SPELL_SUMMON_HORROR         = 70372,
    SPELL_SHADOW_TRAP           = 73539,

    // Phase transition
    SPELL_REMORSELESS_WINTER_1  = 68981, // rooting caster and with Activate Object effect
    SPELL_REMORSELESS_WINTER_2  = 72259, // rooting caster and with Send Script Event (23507) effect
    SPELL_QUAKE                 = 72262,
    SPELL_PAIN_AND_SUFFERING    = 72133,
    SPELL_RAGING_SPIRIT         = 69200,
    SPELL_SUMMON_RAGING_SPIRIT  = 69201,
    SPELL_SUMMON_ICE_SPHERE     = 69103,
    SPELL_ICE_SPHERE            = 69104, // missile and summon effect

    // Phase Two
    SPELL_SUMMON_VALKYR         = 69037,
    SPELL_SUMMON_VALKYRS        = 74361, // 25man
    SPELL_SOUL_REAPER           = 69409,
    SPELL_DEFILE                = 72762,

    // Phase Three
    SPELL_VILE_SPIRITS          = 70498,
    SPELL_HARVEST_SOUL          = 68980, // stun aura and periodic damage, triggers summoning of vehicle
    SPELL_HARVEST_SOUL_TP_FM_N  = 72546, // teleports to Frostmourne Room and applies 60sec dummy aura (normal)
    SPELL_HARVEST_SOUL_TP_FM_H  = 73655, // teleports to Frostmourne Room and applies 60sec DoT aura (heroic)
    SPELL_HARVEST_SOUL_CLONE    = 71372,
    SPELL_HARVEST_SOULS         = 74296,
    SPELL_HARVESTED_SOUL_1      = 73028,
    SPELL_HARVESTED_SOUL_2      = 74321,
    SPELL_HARVESTED_SOUL_3      = 74322,
    SPELL_HARVESTED_SOUL_4      = 74323,

    SPELL_FROSTMOURNE_TP_VISUAL = 73078,

    // Shambling Horror
    SPELL_FRENZY                = 28747,
    SPELL_ENRAGE                = 72143,
    SPELL_SHOCKWAVE             = 72149,

    // Shadow Trap
    SPELL_SHADOW_TRAP_VISUAL    = 73530,
    SPELL_SHADOW_TRAP_AURA      = 73525,

    // Raging Spirit
    SPELL_SOUL_SHRIEK           = 69242,
    SPELL_RAGING_SPIRIT_VISUAL  = 69198, // clone effect (clone of player)

    // Ice Sphere
    SPELL_ICE_SPHERE_VISUAL     = 69090,
    SPELL_ICE_BURST_AURA        = 69109,
    SPELL_ICE_BURST             = 69108,
    SPELL_ICE_PULSE             = 69091,

    // Val'kyr Shadowguard
    SPELL_LIFE_SIPHON           = 73783,
    SPELL_VALKYR_CHARGE         = 74399,
    SPELL_VALKYR_TARGET_SEARCH  = 69030,
    SPELL_VALKYR_CARRY          = 74445,
    SPELL_HARVEST_SOUL_VEHICLE  = 68985,
    SPELL_EJECT_PASSENGERS      = 68576,
    SPELL_WINGS_OF_THE_DAMNED   = 74352,

    // Defile
    SPELL_DEFILE_AURA           = 72743,
    SPELL_DEFILE_GROW           = 72756,

    // Vile Spirit and Wicked Spirit
    SPELL_SPIRIT_BURST_AURA     = 70502,
    SPELL_VILE_SPIRIT_MOVE      = 70501,

    // Spirit Warden
    SPELL_DARK_HUNGER           = 69383,
    SPELL_DESTROY_SOUL          = 74086,
    SPELL_SOUL_RIP              = 69397, // 3500, each next one x2 (maybe based on HP of target?)

    // Terenas in Frostmourne
    SPELL_RESTORE_SOUL          = 72595,
    SPELL_RESTORE_SOUL_HEROIC   = 73650,
    SPELL_LIGHTS_FAVOR          = 69382,
    SPELL_SUMMON_SPIRIT_BOMBS_1 = 73581, // heroic only, summons Spirit Bomb every 1 sec
    SPELL_SUMMON_SPIRIT_BOMBS_2 = 74299, // 2 secs interval
    SPELL_SUMMON_SPIRIT_BOMB    = 74300, // aura doesnt work somehow, so we will use manual summoning

    // Spirit Bomb
    SPELL_SPIRIT_BOMB_VISUAL    = 73572,
    SPELL_EXPLOSION             = 73804,

    // NPCs
    NPC_SHADOW_TRAP             = 39137,
    NPC_FROSTMOURNE             = 38584,
    NPC_ICE_SPHERE              = 36633,
    NPC_RAGING_SPIRIT           = 36701,
    NPC_DEFILE                  = 38757,
    NPC_SPIRIT_WARDEN           = 36824,
    NPC_TERENAS_FM_NORMAL       = 36823,
    NPC_TERENAS_FM_HEROIC       = 39217,
    NPC_WICKED_SPIRIT           = 39190,
    NPC_SPIRIT_BOMB             = 39189,
    NPC_DRUDGE_GHOUL            = 37695,
    NPC_SHAMBLING_HORROR        = 37698,
    NPC_VALKYR_SHADOWGUARD      = 36609,
    NPC_VILE_SPIRIT             = 37799,

    // 3.3.5 creature-template factions used by the retail encounter.  Some
    // spell-summoned creatures do not inherit their template faction on this
    // core, so restore the original per-creature value at the summon boundary.
    FACTION_SCOURGE_MONSTER     = 21,
    FACTION_HOSTILE_MONSTER     = 14,
};

enum Phase
{
    PHASE_INTRO                 = 0,    // intro
    PHASE_ONE                   = 1,    // phase one
    PHASE_RUNNING_WINTER_ONE    = 2,    // running to center of platform to cast Remorseless Winter
    PHASE_TRANSITION_ONE        = 3,    // Remorseless Winter aura and casting spells, summoning orbs and spirits
    PHASE_QUAKE_ONE             = 4,    // casting Quake
    PHASE_TWO                   = 5,    // phase two with val'kyrs and some more spells
    PHASE_RUNNING_WINTER_TWO    = 6,    // running to center of platform to cast Remorseless Winter again
    PHASE_TRANSITION_TWO        = 7,    // second Remorseless Winter phase
    PHASE_QUAKE_TWO             = 8,    // second Quake casting
    PHASE_THREE                 = 9,    // phase three, casting Soul Harvest (Frostmourne phase)
    PHASE_IN_FROSTMOURNE        = 10,   // phase three, waiting untill whole raid leaves Frostmourne
    PHASE_CUTSCENE              = 11,   // phase when LK kills raid, Terenas comes etc.
    PHASE_DEATH_AWAITS          = 12,   // strangulating Lich King, raid group finishing him
};

enum MovePoint
{
    POINT_CENTER_LAND           = 1,
    POINT_CENTER_LAND_TIRION    = 2,
    POINT_TELEPORTER_TIRION     = 3,
    POINT_VALKYR_THROW          = 4,
    POINT_VALKYR_CENTER         = 5,
    POINT_TP_TO_FM              = 6, // point where strangulate vehicle moves, after reaching player is teleported into frostmourne
    POINT_SPIRIT_BOMB           = 7, // Spirit Bomb moving down
};

enum MiscData
{
    LIGHT_ID_SNOWSTORM             = 2490,
    LIGHT_ID_SOULSTORM             = 2508,

    MUSIC_ID_FROZEN_THRONE         = 17457,
    MUSIC_ID_SPECIAL               = 17458,    // Summon Shambling Horror, Remorseless Winter, Quake, Summon Val'kyr Periodic, Harvest Soul, Vile Spirits
    MUSIC_ID_FURY_OF_FROSTMOURNE   = 17459,
    MUSIC_ID_FINAL                 = 17460,    // Raise Dead, Light's Blessing

    SOUND_ID_PAIN                  = 17360,    // separate sound, not attached to any text

    EQUIP_ID_ASHBRINGER_GLOWING    = 50442,
    EQUIP_ID_BROKEN_FROSTMOURNE    = 50840,

    MOVIE_ID_FALL_OF_THE_LICH_KING = 16,

    AREA_ID_THE_FROZEN_THRONE      = 4859,
};

enum TirionData
{
    GOSSIP_ACTION_START_LICH_KING = GOSSIP_ACTION_INFO_DEF + 1,
    POINT_TIRION_INTRO = 20,
    POINT_TIRION_CHARGE = 21,
};

static const float fLichKingPosition[11][3] =
{
    {458.59f, -2122.71f, 1040.86f},    // 0 Lich King Intro
    {503.16f, -2124.52f, 1040.86f},    // 1 Center of the platform
    {500.16f, -2124.52f, 1040.86f},    // 2 Tirion strikes Lich King
    {481.70f, -2124.64f, 1040.86f},    // 3 Tirion 2
    {498.00f, -2201.57f, 1046.09f},    // 4 Valkyrs?
    {517.48f, -2124.91f, 1040.86f},    // 5 Tirion?
    {529.85f, -2124.71f, 1040.86f},    // 6 Lich king final, o=3.1146
    {520.31f, -2124.71f, 1040.86f},    // 7 Frostmourne
    {453.80f, -2088.20f, 1040.86f},    // 8 Val'kyr drop point right to Frozen Throne
    {457.03f, -2155.08f, 1040.86f},    // 9 Val'kyr drop point left to Frozen Throne
    {494.31f, -2523.08f, 1249.87f},    // 10 center of platform inside Frostmourne
};

namespace
{
void LaunchLichKingIntroWalk(Creature* lichKing)
{
    Movement::PointsArray path;
    path.push_back(Vector3(lichKing->GetPositionX(), lichKing->GetPositionY(), lichKing->GetPositionZ()));
    path.push_back(Vector3(432.0851f, -2123.673f, 1064.6582f));
    path.push_back(Vector3(457.8351f, -2123.423f, 1041.1582f));
    path.push_back(Vector3(465.0730f, -2123.470f, 1040.8569f));

    Movement::MoveSplineInit movement(*lichKing);
    movement.MovebyPath(path);
    movement.SetWalk(true);
    movement.SetVelocity(3.5f);
    movement.SetFacing(0.0f);
    movement.Launch();
}
}

struct boss_the_lich_king_iccAI : public ScriptedAI
{
    boss_the_lich_king_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiGhoulsTimer;
    uint32 m_uiHorrorTimer;
    uint32 m_uiNecroticPlagueTimer;
    uint32 m_uiInfestTimer;
    uint32 m_uiShadowTrapTimer;
    uint32 m_uiPainSufferingTimer;
    uint32 m_uiRagingSpiritTimer;
    uint32 m_uiIceSphereTimer;
    uint32 m_uiValkyrTimer;
    uint32 m_uiDefileTimer;
    uint32 m_uiSoulReaperTimer;
    uint32 m_uiHarvestSoulTimer;
    uint32 m_uiFrostmournePhaseTimer;
    uint32 m_uiVileSpiritsTimer;
    uint32 m_uiEndingTimer;
    uint8 m_uiEndingStep;
    uint8 m_uiIntroStep;
    ObjectGuid m_frostmourneTargetGuid;
    GuidList m_frostmourneSummons;
    GuidList m_encounterSummons;

    void Reset() override
    {
        m_uiPhase               = PHASE_INTRO;

        m_uiBerserkTimer        = 15 * MINUTE * IN_MILLISECONDS;
        m_uiGhoulsTimer         = 10000;
        m_uiHorrorTimer         = 15000;
        m_uiInfestTimer         = 5000;
        m_uiNecroticPlagueTimer = urand(30000, 31000);
        m_uiShadowTrapTimer     = 15500;
        m_uiPainSufferingTimer  = 6000;
        m_uiRagingSpiritTimer   = 20000;
        m_uiIceSphereTimer      = 6000;
        m_uiValkyrTimer         = 10000;
        m_uiDefileTimer         = 30000;
        m_uiSoulReaperTimer     = 25000;
        m_uiHarvestSoulTimer    = 5000;
        m_uiVileSpiritsTimer    = 20000;
        m_uiEndingTimer         = 0;
        m_uiEndingStep          = 0;
        m_uiIntroStep           = 0;
        m_frostmourneTargetGuid.Clear();
        m_frostmourneSummons.clear();
        m_encounterSummons.clear();

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);

        if (m_pInstance)
            m_pInstance->SetLichKingPlatformDamaged(false);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LICH_KING, IN_PROGRESS);

        SetCombatMovement(true);
        DoScriptText(SAY_AGGRO, m_creature);
        m_uiPhase = PHASE_ONE;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit*, Unit*, uint32) override
    {
        if (eventType != AI_EVENT_CUSTOM_A || m_uiPhase != PHASE_INTRO || m_uiIntroStep)
            return;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LICH_KING, IN_PROGRESS);
        m_uiIntroStep = 1;
        m_uiPhaseTimer = 11000;
        DoScriptText(SAY_INTRO_1, m_creature);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        LaunchLichKingIntroWalk(m_creature);
    }

    void KilledUnit(Unit* pWho) override
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellEntry const*) override
    {
        // Do not allow a large hit (including the GM damage command used by
        // testers) to skip one of the two retail transitions or the scripted
        // ten-percent finale.  Normal raid damage is unchanged until it
        // reaches the threshold belonging to the current encounter phase.
        uint32 floorPct = 0;
        switch (m_uiPhase)
        {
            case PHASE_ONE:
            case PHASE_RUNNING_WINTER_ONE:
            case PHASE_TRANSITION_ONE:
            case PHASE_QUAKE_ONE:
                floorPct = 70;
                break;
            case PHASE_TWO:
            case PHASE_RUNNING_WINTER_TWO:
            case PHASE_TRANSITION_TWO:
            case PHASE_QUAKE_TWO:
                floorPct = 40;
                break;
            case PHASE_THREE:
            case PHASE_IN_FROSTMOURNE:
                floorPct = 10;
                break;
            case PHASE_INTRO:
            case PHASE_CUTSCENE:
                damage = 0;
                return;
            default:
                return;
        }

        uint32 const healthFloor = std::max<uint32>(1, m_creature->GetMaxHealth() * floorPct / 100);
        if (m_creature->GetHealth() <= healthFloor)
            damage = 0;
        else if (damage >= m_creature->GetHealth() - healthFloor)
            damage = m_creature->GetHealth() - healthFloor;
    }

    void JustSummoned(Creature* summon) override
    {
        switch (summon->GetEntry())
        {
            case NPC_DRUDGE_GHOUL:
            case NPC_SHAMBLING_HORROR:
                // These are combat summons, never RP actors. Use the same
                // hostile encounter faction as the remaining Lich King adds
                // so a missing/variant DB faction cannot leave them friendly.
                summon->SetFactionTemporary(FACTION_HOSTILE_MONSTER, TEMPFACTION_NONE);
                summon->SetInCombatWithZone();
                summon->AI()->AttackClosestEnemy();
                if (Unit* victim = summon->GetVictim())
                    summon->GetMotionMaster()->MoveChase(victim);
                m_encounterSummons.push_back(summon->GetObjectGuid());
                break;
            case NPC_RAGING_SPIRIT:
            case NPC_VALKYR_SHADOWGUARD:
                summon->SetFactionTemporary(FACTION_HOSTILE_MONSTER, TEMPFACTION_NONE);
                summon->SetInCombatWithZone();
                summon->AI()->AttackClosestEnemy();
                if (Unit* victim = summon->GetVictim())
                    summon->GetMotionMaster()->MoveChase(victim);
                m_encounterSummons.push_back(summon->GetObjectGuid());
                break;
            case NPC_VILE_SPIRIT:
                summon->SetFactionTemporary(FACTION_HOSTILE_MONSTER, TEMPFACTION_NONE);
                summon->SetInCombatWithZone();
                // Its own AI preserves the retail hover delay before it picks
                // a target.  Starting an attack here skips that mechanic.
                m_encounterSummons.push_back(summon->GetObjectGuid());
                break;
            case NPC_ICE_SPHERE:
            case NPC_DEFILE:
            case NPC_SHADOW_TRAP:
                summon->AI()->SetReactState(REACT_PASSIVE);
                m_encounterSummons.push_back(summon->GetObjectGuid());
                break;
            case NPC_TERENAS_FM_NORMAL:
            case NPC_TERENAS_FM_HEROIC:
            case NPC_SPIRIT_WARDEN:
            case NPC_SPIRIT_BOMB:
                m_frostmourneSummons.push_back(summon->GetObjectGuid());
                break;
        }
    }

    void CleanupEncounterSummons()
    {
        for (GuidList::const_iterator itr = m_encounterSummons.begin(); itr != m_encounterSummons.end(); ++itr)
            if (Creature* summon = m_creature->GetMap()->GetCreature(*itr))
                summon->ForcedDespawn();
        m_encounterSummons.clear();
    }

    void CleanupFrostmourneRoom()
    {
        for (GuidList::const_iterator itr = m_frostmourneSummons.begin(); itr != m_frostmourneSummons.end(); ++itr)
            if (Creature* summon = m_creature->GetMap()->GetCreature(*itr))
                summon->ForcedDespawn(1000);
        m_frostmourneSummons.clear();
        m_frostmourneTargetGuid.Clear();
    }

    void StartNormalFrostmourne(Player* target)
    {
        if (!target)
            return;

        m_frostmourneTargetGuid = target->GetObjectGuid();
        target->CastSpell(target, SPELL_HARVEST_SOUL_TP_FM_N, TRIGGERED_OLD_TRIGGERED);
        Creature* terenas = m_creature->SummonCreature(NPC_TERENAS_FM_NORMAL, 495.55f, -2517.01f, 1250.0f,
            4.6993f, TEMPSPAWN_TIMED_OR_DEAD_DESPAWN, 65000);
        Creature* warden = m_creature->SummonCreature(NPC_SPIRIT_WARDEN, 495.34f, -2529.98f, 1250.0f,
            1.5592f, TEMPSPAWN_TIMED_OR_DEAD_DESPAWN, 65000);
        if (terenas && warden)
        {
            terenas->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, target, terenas);
            warden->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, terenas, warden);
            terenas->SetInCombatWith(warden);
            warden->SetInCombatWith(terenas);
            terenas->AI()->AttackStart(warden);
            warden->AI()->AttackStart(terenas);
        }
    }

    void StartHeroicFrostmourne()
    {
        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
            if (Player* player = playerRef.getSource())
                if (player->IsAlive() && !player->IsGameMaster())
                    player->CastSpell(player, SPELL_HARVEST_SOUL_TP_FM_H, TRIGGERED_OLD_TRIGGERED);

        Creature* terenas = m_creature->SummonCreature(NPC_TERENAS_FM_HEROIC, 495.71f, -2523.76f, 1250.0f,
            0.0f, TEMPSPAWN_TIMED_DESPAWN, 50000);
        if (terenas)
            terenas->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, terenas);
    }

    void FinishNormalFrostmourne(bool success)
    {
        if (Player* target = m_creature->GetMap()->GetPlayer(m_frostmourneTargetGuid))
        {
            if (success)
                target->CastSpell(target, SPELL_RESTORE_SOUL, TRIGGERED_OLD_TRIGGERED);
            else
                target->CastSpell(target, SPELL_DESTROY_SOUL, TRIGGERED_OLD_TRIGGERED);
        }
        CleanupFrostmourneRoom();
    }

    void ReturnHeroicFrostmournePlayers()
    {
        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
            if (Player* player = playerRef.getSource())
                if (player->IsAlive() && !player->IsGameMaster())
                    player->CastSpell(player, SPELL_RESTORE_SOUL_HEROIC, TRIGGERED_OLD_TRIGGERED);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        CleanupEncounterSummons();
        CleanupFrostmourneRoom();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LICH_KING, DONE);

        DoScriptText(SAY_OUTRO_14, m_creature);

        // Spell 73159 requires a Player unit target for the registered
        // PlayMovie spell script. An untargeted cast does not reliably build
        // one on CMaNGOS, so cast the native encounter spell once per player.
        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
            if (Player* player = playerRef.getSource())
                m_creature->CastSpell(player, SPELL_PLAY_MOVIE, TRIGGERED_OLD_TRIGGERED);
    }

    void JustReachedHome() override
    {
        CleanupEncounterSummons();
        CleanupFrostmourneRoom();
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_LICH_KING, FAIL);
            if (Creature* tirion = m_pInstance->GetSingleCreatureFromStorage(NPC_TIRION_FORDRING))
            {
                tirion->RemoveAurasDueToSpell(SPELL_ICE_LOCK);
                tirion->CombatStop(true);
                tirion->GetMotionMaster()->MoveTargetedHome();
            }
        }
    }

    void MovementInform(uint32 uiMovementType, uint32 uiData) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        switch (uiData)
        {
            case POINT_CENTER_LAND:
            {
                if (m_uiPhase == PHASE_RUNNING_WINTER_ONE)
                {
                    DoScriptText(SAY_REMORSELESS_WINTER, m_creature);

                    DoCastSpellIfCan(m_creature, SPELL_REMORSELESS_WINTER_1);

                    m_uiPhase = PHASE_TRANSITION_ONE;
                    m_uiPhaseTimer          = 62000;

                    m_uiPainSufferingTimer = 3500;
                    m_uiRagingSpiritTimer = 4000;
                    m_uiIceSphereTimer = 8000;
                }
                else if (m_uiPhase == PHASE_RUNNING_WINTER_TWO)
                {
                    DoScriptText(SAY_REMORSELESS_WINTER, m_creature);

                    DoCastSpellIfCan(m_creature, SPELL_REMORSELESS_WINTER_2);

                    m_uiPhase = PHASE_TRANSITION_TWO;
                    m_uiPhaseTimer          = 62000;

                    m_uiPainSufferingTimer = 3500;
                    m_uiRagingSpiritTimer = 4000;
                    m_uiIceSphereTimer = 8000;
                }
                else if (m_uiPhase == PHASE_DEATH_AWAITS)
                {
                    DoCastSpellIfCan(m_creature, SPELL_RAISE_DEAD);
                }

                break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiPhase != PHASE_INTRO && m_uiPhase != PHASE_DEATH_AWAITS)
        {
            // Transition, quake and Frostmourne timers must continue without
            // a current melee victim. Retail deliberately makes the boss
            // stationary/unattackable during parts of these states, and a
            // solo player or threat reset can otherwise freeze the encounter
            // forever before the next phase begins.
            bool const victimRequired = m_uiPhase == PHASE_ONE ||
                m_uiPhase == PHASE_TWO || m_uiPhase == PHASE_THREE;
            if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            {
                if (victimRequired)
                {
                    m_creature->SetInCombatWithZone();
                    m_creature->AI()->AttackClosestEnemy();
                    if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
                        return;
                }
            }

            // Berserk
            if (m_uiBerserkTimer)
            {
                if (m_uiBerserkTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                    {
                        DoScriptText(SAY_ENRAGE, m_creature);
                        m_uiBerserkTimer = 0;
                    }
                }
                else
                    m_uiBerserkTimer -= uiDiff;
            }
        }

        switch (m_uiPhase)
        {
            case PHASE_INTRO:
            {
                if (!m_uiIntroStep)
                    return;

                if (m_uiPhaseTimer > uiDiff)
                {
                    m_uiPhaseTimer -= uiDiff;
                    return;
                }

                switch (++m_uiIntroStep)
                {
                    case 2:
                        if (Creature* tirion = m_pInstance ? m_pInstance->GetSingleCreatureFromStorage(NPC_TIRION_FORDRING) : nullptr)
                            DoScriptText(SAY_INTRO_2, tirion);
                        m_uiPhaseTimer = 9000;
                        break;
                    case 3:
                        DoScriptText(SAY_INTRO_3, m_creature);
                        m_uiPhaseTimer = 25000;
                        break;
                    case 4:
                        if (Creature* tirion = m_pInstance ? m_pInstance->GetSingleCreatureFromStorage(NPC_TIRION_FORDRING) : nullptr)
                        {
                            DoScriptText(SAY_INTRO_4, tirion);
                            tirion->SetWalk(false);
                            tirion->GetMotionMaster()->MovePoint(POINT_TIRION_CHARGE, 482.902f, -2124.479f, 1040.857f, FORCED_MOVEMENT_RUN);
                        }
                        m_uiPhaseTimer = 6000;
                        break;
                    case 5:
                        DoScriptText(SAY_INTRO_5, m_creature);
                        if (Creature* tirion = m_pInstance ? m_pInstance->GetSingleCreatureFromStorage(NPC_TIRION_FORDRING) : nullptr)
                            DoCastSpellIfCan(tirion, SPELL_ICE_LOCK, CAST_TRIGGERED);
                        m_uiPhaseTimer = 5000;
                        break;
                    default:
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                        // Leave the intro state before selecting a victim so
                        // an empty/solo threat list cannot replay this branch
                        // every server tick.
                        m_uiPhase = PHASE_ONE;
                        SetCombatMovement(true);
                        m_creature->SetInCombatWithZone();
                        m_creature->AI()->AttackClosestEnemy();
                        if (Unit* victim = m_creature->GetVictim())
                            m_creature->GetMotionMaster()->MoveChase(victim);
                        break;
                }
                return;
            }
            case PHASE_ONE:
            {
                // check HP
                if (m_creature->GetHealthPercent() <= 70.0f)
                {
                    // phase transition
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(POINT_CENTER_LAND, fLichKingPosition[1][0], fLichKingPosition[1][1], fLichKingPosition[1][2]);
                    m_uiPhase = PHASE_RUNNING_WINTER_ONE;
                    return;
                }

                // Necrotic Plague
                if (m_uiNecroticPlagueTimer < uiDiff)
                {
                    // shouldn't be targeting players who already have Necrotic Plague on them
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_NECROTIC_PLAGUE, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_NECROTIC_PLAGUE) == CAST_OK)
                            m_uiNecroticPlagueTimer = 30000;
                    }
                }
                else
                    m_uiNecroticPlagueTimer -= uiDiff;

                // Infest
                if (m_uiInfestTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_INFEST) == CAST_OK)
                        m_uiInfestTimer = urand(20000, 25000);
                }
                else
                    m_uiInfestTimer -= uiDiff;

                // Summon Ghouls
                if (m_uiGhoulsTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_GHOULS) == CAST_OK)
                        m_uiGhoulsTimer = 32000;
                }
                else
                    m_uiGhoulsTimer -= uiDiff;

                // Summon Shambling Horror
                if (m_uiHorrorTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_HORROR) == CAST_OK)
                        m_uiHorrorTimer = 60000;
                }
                else
                    m_uiHorrorTimer -= uiDiff;

                // Shadow Trap (heroic)
                if (m_pInstance && m_pInstance->IsHeroicDifficulty())
                {
                    if (m_uiShadowTrapTimer < uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_SHADOW_TRAP, SELECT_FLAG_PLAYER))
                        {
                            if (DoCastSpellIfCan(pTarget, SPELL_SHADOW_TRAP) == CAST_OK)
                                m_uiShadowTrapTimer = 15000;
                        }
                    }
                    else
                        m_uiShadowTrapTimer -= uiDiff;
                }

                DoMeleeAttackIfReady();

                break;
            }
            case PHASE_RUNNING_WINTER_ONE:
            case PHASE_RUNNING_WINTER_TWO:
            {
                // wait for waypoint arrival
                break;
            }
            case PHASE_TRANSITION_ONE:
            case PHASE_TRANSITION_TWO:
            {
                // phase end timer
                if (m_uiPhaseTimer < uiDiff)
                {
                    // Remorseless Winter is still channeling when the
                    // transition timer expires.  Quake cannot begin until
                    // that channel is explicitly ended (retail/TC/AC
                    // transition ordering).
                    m_creature->InterruptNonMeleeSpells(false);
                    m_creature->clearUnitState(UNIT_STAT_CHANNELING);
                    if (DoCastSpellIfCan(m_creature, SPELL_QUAKE) == CAST_OK)
                    {
                        DoScriptText(SAY_SHATTER_ARENA, m_creature);
                        m_uiPhase = (m_uiPhase == PHASE_TRANSITION_ONE ? PHASE_QUAKE_ONE : PHASE_QUAKE_TWO);
                        m_uiPhaseTimer = 6500;
                    }
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                // Pain and Suffering
                if (m_uiPainSufferingTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_PAIN_AND_SUFFERING, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_PAIN_AND_SUFFERING) == CAST_OK)
                            m_uiPainSufferingTimer = urand(1500, 3000);
                    }
                }
                else
                    m_uiPainSufferingTimer -= uiDiff;

                // Summon Ice Sphere
                if (m_uiIceSphereTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_ICE_SPHERE) == CAST_OK)
                        m_uiIceSphereTimer = 6000;
                }
                else
                    m_uiIceSphereTimer -= uiDiff;

                // Summon Raging Spirit
                if (m_uiRagingSpiritTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_RAGING_SPIRIT, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_RAGING_SPIRIT, CAST_TRIGGERED) == CAST_OK)
                            m_uiRagingSpiritTimer = (m_uiPhase == PHASE_TRANSITION_ONE ? 20000 : 15000);
                    }
                }
                else
                    m_uiRagingSpiritTimer -= uiDiff;

                break;
            }
            case PHASE_QUAKE_ONE:
            case PHASE_QUAKE_TWO:
            {
                // Casting Quake spell - phase end timer
                if (m_uiPhaseTimer < uiDiff)
                {
                    if (m_pInstance)
                        m_pInstance->SetLichKingPlatformDamaged(true);

                    bool const firstTransition = m_uiPhase == PHASE_QUAKE_ONE;
                    m_uiPhase = firstTransition ? PHASE_TWO : PHASE_THREE;
                    if (firstTransition)
                    {
                        m_uiInfestTimer = 14000;
                        m_uiValkyrTimer = 20000;
                        m_uiSoulReaperTimer = 40000;
                        m_uiDefileTimer = 38000;
                    }
                    else
                    {
                        m_uiSoulReaperTimer = 40000;
                        m_uiDefileTimer = 38000;
                        m_uiVileSpiritsTimer = 20000;
                        m_uiHarvestSoulTimer = 14000;
                    }
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->SetInCombatWithZone();
                    m_creature->AI()->AttackClosestEnemy();
                    if (m_creature->GetVictim())
                        m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                break;
            }
            case PHASE_TWO:
            {
                // check HP
                if (m_creature->GetHealthPercent() <= 40.0f)
                {
                    // phase transition
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(POINT_CENTER_LAND, fLichKingPosition[1][0], fLichKingPosition[1][1], fLichKingPosition[1][2]);
                    m_uiPhaseTimer = 60000;
                    m_uiPhase = PHASE_RUNNING_WINTER_TWO;
                }

                // Soul Reaper
                if (m_uiSoulReaperTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SOUL_REAPER) == CAST_OK)
                        m_uiSoulReaperTimer = 30000;
                }
                else
                    m_uiSoulReaperTimer -= uiDiff;

                // Infest
                if (m_uiInfestTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_INFEST) == CAST_OK)
                        m_uiInfestTimer = urand(20000, 25000);
                }
                else
                    m_uiInfestTimer -= uiDiff;

                // Defile
                if (m_uiDefileTimer < uiDiff)
                {
                    // shouldn't be targeting players in vehicles
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_DEFILE, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DEFILE) == CAST_OK)
                            m_uiDefileTimer = 32500;
                    }
                }
                else
                    m_uiDefileTimer -= uiDiff;

                // Summon Val'kyr
                if (m_uiValkyrTimer < uiDiff)
                {
                    uint32 valkyrSpell = m_pInstance && m_pInstance->Is25ManDifficulty() ? SPELL_SUMMON_VALKYRS : SPELL_SUMMON_VALKYR;
                    if (DoCastSpellIfCan(m_creature, valkyrSpell) == CAST_OK)
                    {
                        DoScriptText(SAY_SUMMON_VALKYR, m_creature);
                        m_uiValkyrTimer = 45000;
                    }
                }
                else
                    m_uiValkyrTimer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            }
            case PHASE_THREE:
            {
                // check HP
                if (m_creature->GetHealthPercent() <= 10.0f)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_FURY_OF_FROSTMOURNE) == CAST_OK)
                    {
                        DoScriptText(SAY_LAST_PHASE, m_creature);
                        m_uiPhase = PHASE_DEATH_AWAITS;

                        m_creature->AttackStop();
                        SetCombatMovement(false);
                        m_creature->GetMotionMaster()->Clear(false, true);
                        m_creature->GetMotionMaster()->MoveIdle();
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                        m_uiEndingStep = 1;
                        m_uiEndingTimer = 8000;

                        return;
                    }
                }

                // Soul Reaper
                if (m_uiSoulReaperTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SOUL_REAPER) == CAST_OK)
                        m_uiSoulReaperTimer = 30000;
                }
                else
                    m_uiSoulReaperTimer -= uiDiff;

                // Defile
                if (m_uiDefileTimer < uiDiff)
                {
                    // shouldn't be targeting players in vehicles
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_DEFILE, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DEFILE) == CAST_OK)
                            m_uiDefileTimer = 32500;
                    }
                }
                else
                    m_uiDefileTimer -= uiDiff;

                // Harvest Soul
                if (m_uiHarvestSoulTimer < uiDiff)
                {
                    Unit* pTarget = nullptr;
                    bool m_bIsHeroic = m_pInstance && m_pInstance->IsHeroicDifficulty();
                    if (m_bIsHeroic)
                        pTarget = m_creature;
                    else
                        pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_HARVEST_SOUL, SELECT_FLAG_PLAYER);

                    if (pTarget)
                    {
                        if (DoCastSpellIfCan(pTarget, m_bIsHeroic ? SPELL_HARVEST_SOULS : SPELL_HARVEST_SOUL) == CAST_OK)
                        {
                            DoScriptText(SAY_HARVEST_SOUL, m_creature);
                            m_uiHarvestSoulTimer = m_bIsHeroic ? urand(100000, 110000) : 75000;

                            if (m_bIsHeroic)
                            {
                                StartHeroicFrostmourne();
                                m_uiPhase = PHASE_IN_FROSTMOURNE;
                                SetCombatMovement(false);
                                m_creature->StopMoving();
                                m_uiFrostmournePhaseTimer = 47000;
                                m_uiDefileTimer = 1000;
                            }
                            else
                                StartNormalFrostmourne(static_cast<Player*>(pTarget));
                        }
                    }
                }
                else
                    m_uiHarvestSoulTimer -= uiDiff;

                // Vile Spirits
                if (m_uiVileSpiritsTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_VILE_SPIRITS) == CAST_OK)
                        m_uiVileSpiritsTimer = 30000;
                }
                else
                    m_uiVileSpiritsTimer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            }
            case PHASE_IN_FROSTMOURNE:
            {
                // check if players are alive before entering evade mode?
                // wait until they leave Frostmourne
                if (m_uiFrostmournePhaseTimer < uiDiff)
                {
                    ReturnHeroicFrostmournePlayers();
                    CleanupFrostmourneRoom();
                    m_uiPhase = PHASE_THREE;
                    if (m_creature->GetVictim())
                        m_creature->GetMotionMaster()->MoveChase(m_creature->GetVictim());
                }
                else
                    m_uiFrostmournePhaseTimer -= uiDiff;

                break;
            }
            case PHASE_DEATH_AWAITS:
            {
                if (!m_uiEndingTimer || m_uiEndingTimer > uiDiff)
                {
                    if (m_uiEndingTimer)
                        m_uiEndingTimer -= uiDiff;
                    break;
                }

                switch (m_uiEndingStep++)
                {
                    case 1:
                        DoScriptText(SAY_OUTRO_1, m_creature);
                        if (Creature* tirion = m_pInstance ? m_pInstance->GetSingleCreatureFromStorage(NPC_TIRION_FORDRING) : nullptr)
                            tirion->CastSpell(tirion, SPELL_LIGHTS_BLESSING, TRIGGERED_NONE);
                        m_uiEndingTimer = 10000;
                        break;
                    case 2:
                        DoScriptText(SAY_OUTRO_2, m_creature);
                        DoCastSpellIfCan(m_creature, SPELL_THROW_FROSTMOURNE, CAST_TRIGGERED);
                        m_uiEndingTimer = 8000;
                        break;
                    case 3:
                        DoCastSpellIfCan(m_creature, SPELL_BROKEN_FROSTMOURNE, CAST_TRIGGERED);
                        DoCastSpellIfCan(m_creature, SPELL_FROSTMOURNE_SPIRITS, CAST_TRIGGERED);
                        m_uiEndingTimer = 10000;
                        break;
                    case 4:
                        if (Creature* terenas = m_creature->SummonCreature(NPC_TENERAS_MENETHIL,
                                fLichKingPosition[7][0], fLichKingPosition[7][1], fLichKingPosition[7][2], 0.0f,
                                TEMPSPAWN_TIMED_DESPAWN, 60000))
                            terenas->CastSpell(terenas, SPELL_MASS_RESURRECTION2, TRIGGERED_NONE);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
                        m_creature->SetHealth(m_creature->GetMaxHealth() / 10);
                        m_creature->SetInCombatWithZone();
                        m_uiEndingTimer = 0;
                        break;
                }
                break;
            }
        }
    }
};

struct PlayMovie : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (Unit* target = spell->GetUnitTarget())
            if (target->IsPlayer())
                static_cast<Player*>(target)->SendMovieStart(MOVIE_ID_FALL_OF_THE_LICH_KING);
    }
};

struct npc_terenas_frostmourne_iccAI : public ScriptedAI
{
    npc_terenas_frostmourne_iccAI(Creature* creature) : ScriptedAI(creature), m_bombTimer(3000),
        m_failureTimer(60000), m_heroic(false), m_failed(false) { }

    uint32 m_bombTimer;
    uint32 m_failureTimer;
    bool m_heroic;
    bool m_failed;

    void ReceiveAIEvent(AIEventType type, Unit*, Unit*, uint32) override
    {
        if (type == AI_EVENT_CUSTOM_A)
        {
            m_heroic = false;
            DoCastSpellIfCan(m_creature, SPELL_LIGHTS_FAVOR, CAST_TRIGGERED);
        }
        else if (type == AI_EVENT_CUSTOM_B)
        {
            m_heroic = true;
        }
    }

    void FailNormalRoom()
    {
        if (m_failed)
            return;
        m_failed = true;
        if (instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(m_creature->GetInstanceData()))
            if (Creature* lichKing = instance->GetSingleCreatureFromStorage(NPC_LICH_KING))
                if (boss_the_lich_king_iccAI* ai = dynamic_cast<boss_the_lich_king_iccAI*>(lichKing->AI()))
                    ai->FinishNormalFrostmourne(false);
    }

    void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellEntry const*) override
    {
        if (damage >= m_creature->GetHealth())
        {
            damage = m_creature->GetHealth() > 1 ? m_creature->GetHealth() - 1 : 0;
            if (!m_heroic)
                FailNormalRoom();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (m_heroic)
        {
            if (m_bombTimer <= diff)
            {
                float angle = frand(0.0f, 2.0f * M_PI_F);
                float distance = frand(5.0f, 35.0f);
                m_creature->SummonCreature(NPC_SPIRIT_BOMB,
                    495.7f + std::cos(angle) * distance, -2523.8f + std::sin(angle) * distance, 1250.0f,
                    0.0f, TEMPSPAWN_TIMED_DESPAWN, 15000);
                m_bombTimer = urand(1500, 2500);
            }
            else
                m_bombTimer -= diff;
            return;
        }

        if (m_failureTimer <= diff)
        {
            FailNormalRoom();
            m_failureTimer = 0;
            return;
        }
        m_failureTimer -= diff;

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
        DoMeleeAttackIfReady();
    }
};

struct npc_spirit_warden_iccAI : public ScriptedAI
{
    npc_spirit_warden_iccAI(Creature* creature) : ScriptedAI(creature), m_uiSoulRipTimer(12000) { }

    void ReceiveAIEvent(AIEventType type, Unit* sender, Unit*, uint32) override
    {
        if (type == AI_EVENT_CUSTOM_A && sender)
            AttackStart(sender);
        DoCastSpellIfCan(m_creature, SPELL_DARK_HUNGER, CAST_TRIGGERED);
    }

    void JustDied(Unit*) override
    {
        if (instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(m_creature->GetInstanceData()))
            if (Creature* lichKing = instance->GetSingleCreatureFromStorage(NPC_LICH_KING))
                if (boss_the_lich_king_iccAI* ai = dynamic_cast<boss_the_lich_king_iccAI*>(lichKing->AI()))
                    ai->FinishNormalFrostmourne(true);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
        if (m_uiSoulRipTimer <= diff)
        {
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SOUL_RIP);
            m_uiSoulRipTimer = urand(23000, 27000);
        }
        else
            m_uiSoulRipTimer -= diff;
        DoMeleeAttackIfReady();
    }

private:
    uint32 m_uiSoulRipTimer;
};

struct npc_spirit_bomb_iccAI : public Scripted_NoMovementAI
{
    npc_spirit_bomb_iccAI(Creature* creature) : Scripted_NoMovementAI(creature), m_explodeTimer(5000) { }
    uint32 m_explodeTimer;
    void UpdateAI(uint32 diff) override
    {
        if (m_explodeTimer <= diff)
        {
            DoCastSpellIfCan(m_creature, SPELL_EXPLOSION, CAST_TRIGGERED);
            m_creature->ForcedDespawn(1000);
            m_explodeTimer = 0;
        }
        else
            m_explodeTimer -= diff;
    }
};

struct npc_tirion_fordring_tftAI : public ScriptedAI
{
    npc_tirion_fordring_tftAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
    }

    instance_icecrown_citadel* m_instance;

    void Reset() override
    {
        if (!m_instance || m_instance->GetData(TYPE_LICH_KING) != DONE)
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void MovementInform(uint32 type, uint32 point) override
    {
        if (type != POINT_MOTION_TYPE || point != POINT_TIRION_INTRO || !m_instance)
            return;

        if (Creature* lichKing = m_instance->GetSingleCreatureFromStorage(NPC_LICH_KING))
            lichKing->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, lichKing);
    }
};

bool GossipHello_npc_tirion_fordring_tft(Player* player, Creature* creature)
{
    instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
    if (instance && instance->GetData(TYPE_LICH_KING) != DONE && !instance->IsEncounterInProgress())
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "We are ready. Let us end this.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_START_LICH_KING);
    player->SEND_GOSSIP_MENU(creature->GetEntry(), creature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_tirion_fordring_tft(Player* player, Creature* creature, uint32, uint32 action)
{
    if (action != GOSSIP_ACTION_START_LICH_KING)
        return false;

    player->CLOSE_GOSSIP_MENU();
    instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
    if (!instance || instance->IsEncounterInProgress() || instance->GetData(TYPE_LICH_KING) == DONE)
        return false;

    // Claim the intro at selection, before Tirion starts moving. This makes
    // the RP single-owner and prevents a second click/reset from overlapping
    // the first dialogue sequence.
    instance->SetData(TYPE_LICH_KING, IN_PROGRESS);
    creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    creature->SetWalk(true);
    creature->GetMotionMaster()->MovePoint(POINT_TIRION_INTRO, 489.297f, -2124.840f, 1040.857f);
    return true;
}

struct npc_shambling_horror_iccAI : public ScriptedAI
{
    npc_shambling_horror_iccAI(Creature* creature) : ScriptedAI(creature) { Reset(); }
    uint32 shockwaveTimer;
    uint32 enrageTimer;
    bool frenzied;

    void Reset() override
    {
        shockwaveTimer = urand(20000, 25000);
        enrageTimer = urand(11000, 14000);
        frenzied = false;
    }

    void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellEntry const*) override
    {
        Difficulty difficulty = m_creature->GetMap()->GetDifficulty();
        bool heroic = difficulty == RAID_DIFFICULTY_10MAN_HEROIC || difficulty == RAID_DIFFICULTY_25MAN_HEROIC;
        if (!frenzied && heroic && m_creature->GetHealthPercent() <= 20.0f)
        {
            frenzied = true;
            DoCastSpellIfCan(m_creature, SPELL_FRENZY, CAST_TRIGGERED);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
        if (shockwaveTimer <= diff) { if (DoCastSpellIfCan(m_creature, SPELL_SHOCKWAVE) == CAST_OK) shockwaveTimer = urand(20000, 25000); }
        else shockwaveTimer -= diff;
        if (enrageTimer <= diff) { if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK) enrageTimer = urand(20000, 25000); }
        else enrageTimer -= diff;
        DoMeleeAttackIfReady();
    }
};

struct npc_raging_spirit_iccAI : public ScriptedAI
{
    npc_raging_spirit_iccAI(Creature* creature) : ScriptedAI(creature) { Reset(); }
    uint32 shriekTimer;
    void Reset() override
    {
        shriekTimer = urand(12000, 15000);
        DoCastSpellIfCan(m_creature, SPELL_PLAGUE_AVOIDANCE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_RAGING_SPIRIT_VISUAL, CAST_TRIGGERED);
    }
    void UpdateAI(uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim()) return;
        if (shriekTimer <= diff) { if (DoCastSpellIfCan(m_creature, SPELL_SOUL_SHRIEK) == CAST_OK) shriekTimer = urand(12000, 15000); }
        else shriekTimer -= diff;
        DoMeleeAttackIfReady();
    }
};

struct npc_defile_iccAI : public ScriptedAI
{
    npc_defile_iccAI(Creature* creature) : ScriptedAI(creature) { Reset(); }
    void Reset() override
    {
        SetCombatMovement(false);
        m_creature->SetCanEnterCombat(false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        DoCastSpellIfCan(m_creature, SPELL_DEFILE_AURA, CAST_TRIGGERED);
    }
};

struct npc_vile_spirit_iccAI : public ScriptedAI
{
    npc_vile_spirit_iccAI(Creature* creature) : ScriptedAI(creature) { Reset(); }
    uint32 activateTimer;
    bool active;
    void Reset() override
    {
        activateTimer = 15000;
        active = false;
        m_creature->SetLevitate(true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        DoCastSpellIfCan(m_creature, SPELL_SPIRIT_BURST_AURA, CAST_TRIGGERED);
    }
    void UpdateAI(uint32 diff) override
    {
        if (!active)
        {
            if (activateTimer > diff) { activateTimer -= diff; return; }
            active = true;
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER))
                m_creature->GetMotionMaster()->MoveChase(target);
        }
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            m_creature->SetInCombatWithZone();
    }
};

struct npc_shadow_trap_iccAI : public ScriptedAI
{
    npc_shadow_trap_iccAI(Creature* creature) : ScriptedAI(creature) { Reset(); }
    uint32 armTimer;
    void Reset() override
    {
        armTimer = 5000;
        SetCombatMovement(false);
        m_creature->SetCanEnterCombat(false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
        DoCastSpellIfCan(m_creature, SPELL_SHADOW_TRAP_VISUAL, CAST_TRIGGERED);
    }
    void UpdateAI(uint32 diff) override
    {
        if (!armTimer) return;
        if (armTimer > diff) { armTimer -= diff; return; }
        armTimer = 0;
        DoCastSpellIfCan(m_creature, SPELL_SHADOW_TRAP_AURA, CAST_TRIGGERED);
    }
};

struct npc_ice_sphere_iccAI : public ScriptedAI
{
    npc_ice_sphere_iccAI(Creature* creature) : ScriptedAI(creature) { Reset(); }
    uint32 acquireTimer;
    void Reset() override
    {
        acquireTimer = 1000;
        SetCombatMovement(false);
        m_creature->SetLevitate(true);
        DoCastSpellIfCan(m_creature, SPELL_ICE_SPHERE_VISUAL, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_ICE_BURST_AURA, CAST_TRIGGERED);
    }
    void UpdateAI(uint32 diff) override
    {
        if (!acquireTimer) return;
        if (acquireTimer > diff) { acquireTimer -= diff; return; }
        acquireTimer = 0;
        m_creature->SetInCombatWithZone();
        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER))
        {
            DoCastSpellIfCan(target, SPELL_ICE_PULSE, CAST_TRIGGERED);
            m_creature->GetMotionMaster()->MoveChase(target);
        }
    }
};

struct npc_valkyr_shadowguard_iccAI : public ScriptedAI
{
    npc_valkyr_shadowguard_iccAI(Creature* creature) : ScriptedAI(creature) { Reset(); }
    uint32 grabTimer;
    bool carrying;
    void Reset() override
    {
        grabTimer = 2500;
        carrying = false;
        SetCombatMovement(false);
        m_creature->SetLevitate(true);
        DoCastSpellIfCan(m_creature, SPELL_WINGS_OF_THE_DAMNED, CAST_TRIGGERED);
    }
    void AttackStart(Unit*) override { }
    void MovementInform(uint32 type, uint32 point) override
    {
        if (type == POINT_MOTION_TYPE && point == POINT_VALKYR_THROW)
        {
            DoCastSpellIfCan(m_creature, SPELL_EJECT_PASSENGERS, CAST_TRIGGERED);
            m_creature->ForcedDespawn(1000);
        }
    }
    void UpdateAI(uint32 diff) override
    {
        if (carrying) return;
        if (grabTimer > diff) { grabTimer -= diff; return; }
        grabTimer = 2000;
        m_creature->SetInCombatWithZone();
        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, uint32(0), SELECT_FLAG_PLAYER))
        {
            carrying = true;
            DoCastSpellIfCan(target, SPELL_VALKYR_CARRY, CAST_TRIGGERED);
            uint8 drop = urand(8, 9);
            m_creature->GetMotionMaster()->MovePoint(POINT_VALKYR_THROW, fLichKingPosition[drop][0], fLichKingPosition[drop][1], fLichKingPosition[drop][2]);
        }
    }
};

void AddSC_boss_the_lich_king()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_the_lich_king_icc";
    pNewScript->GetAI = &GetNewAIInstance<boss_the_lich_king_iccAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tirion_fordring_tft";
    pNewScript->GetAI = &GetNewAIInstance<npc_tirion_fordring_tftAI>;
    pNewScript->pGossipHello = &GossipHello_npc_tirion_fordring_tft;
    pNewScript->pGossipSelect = &GossipSelect_npc_tirion_fordring_tft;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shambling_horror_icc";
    pNewScript->GetAI = &GetNewAIInstance<npc_shambling_horror_iccAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_raging_spirit_icc";
    pNewScript->GetAI = &GetNewAIInstance<npc_raging_spirit_iccAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_defile_icc";
    pNewScript->GetAI = &GetNewAIInstance<npc_defile_iccAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_vile_spirit_icc";
    pNewScript->GetAI = &GetNewAIInstance<npc_vile_spirit_iccAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shadow_trap_icc";
    pNewScript->GetAI = &GetNewAIInstance<npc_shadow_trap_iccAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ice_sphere_icc";
    pNewScript->GetAI = &GetNewAIInstance<npc_ice_sphere_iccAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_valkyr_shadowguard_icc";
    pNewScript->GetAI = &GetNewAIInstance<npc_valkyr_shadowguard_iccAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_terenas_frostmourne_icc";
    pNewScript->GetAI = &GetNewAIInstance<npc_terenas_frostmourne_iccAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spirit_warden_icc";
    pNewScript->GetAI = &GetNewAIInstance<npc_spirit_warden_iccAI>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spirit_bomb_icc";
    pNewScript->GetAI = &GetNewAIInstance<npc_spirit_bomb_iccAI>;
    pNewScript->RegisterSelf();

    RegisterSpellScript<PlayMovie>("spell_play_movie");
}
