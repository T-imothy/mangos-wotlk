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
SDName: instance_icecrown_citadel
SD%Complete: 20%
SDComment: Just basic stuff
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Maps/SpawnManager.h"
#include "icecrown_citadel.h"
#include "Entities/Transports.h"

enum
{
    // Marrowgar
    SAY_MARROWGAR_INTRO             = -1631001,

    // Deathwhisper
    SAY_DEATHWHISPER_SPEECH_1       = -1631011,
    SAY_DEATHWHISPER_SPEECH_2       = -1631012,
    SAY_DEATHWHISPER_SPEECH_3       = -1631013,
    SAY_DEATHWHISPER_SPEECH_4       = -1631014,
    SAY_DEATHWHISPER_SPEECH_5       = -1631015,
    SAY_DEATHWHISPER_SPEECH_6       = -1631016,
    SAY_DEATHWHISPER_SPEECH_7       = -1631017,

    // Gunship dialogue
    MUSIC_ID_GUNSHIP                = 17289,

    SAY_GUNSHIP_START_ALLY_1        = -1631035,
    SAY_GUNSHIP_START_ALLY_2        = -1631036,
    SAY_GUNSHIP_START_ALLY_3        = -1631037,
    SAY_GUNSHIP_START_ALLY_4        = -1631038,
    SAY_GUNSHIP_START_ALLY_5        = -1631039,
    SAY_GUNSHIP_START_ALLY_6        = -1631040,
    SAY_GUNSHIP_START_ALLY_7        = -1631041,
    SAY_GUNSHIP_START_ALLY_8        = -1631042,
    SAY_GUNSHIP_HORDE_SOLDIERS      = -1631043,
    SAY_GUNSHIP_HORDE_MAGE          = -1631044,
    SAY_GUNSHIP_HORDE_ATTACK_1      = -1631045,
    SAY_GUNSHIP_HORDE_ATTACK_2      = -1631046,
    SAY_GUNSHIP_ALLY_WIN            = -1631047,

    SAY_GUNSHIP_START_HORDE_1       = -1631048,
    SAY_GUNSHIP_START_HORDE_2       = -1631049,
    SAY_GUNSHIP_START_HORDE_3       = -1631050,
    SAY_GUNSHIP_START_HORDE_4       = -1631051,
    SAY_GUNSHIP_START_HORDE_5       = -1631052,
    SAY_GUNSHIP_START_HORDE_6       = -1631053,
    SAY_GUNSHIP_START_HORDE_7       = -1631054,
    SAY_GUNSHIP_ALLY_SOLDIERS       = -1631055,
    SAY_GUNSHIP_ALLY_MAGE           = -1631056,
    SAY_GUNSHIP_ALLY_ATTACK_1       = -1631057,
    SAY_GUNSHIP_ALLY_ATTACK_2       = -1631058,
    SAY_GUNSHIP_HORDE_WIN           = -1631059,

    SAY_MURADIN_AGGRO               = -1631060,
    SAY_SAURFANG_AGGRO              = -1631061,

    // Festergut
    SAY_STINKY_DIES                 = -1631081,
    // Rotface
    SAY_PRECIOUS_DIES               = -1631070,

    // Gunship related spells
    SPELL_AWARD_REPUTATION          = 73843,
    SPELL_GUNSHIP_ACHIEVEMENT       = 72959,
    SPELL_TELEPORT_PLAYERS_VICTORY  = 72340,
    SPELL_TELEPORT_PLAYERS_RESET_A  = 70446,
    SPELL_TELEPORT_PLAYERS_RESET_H  = 71284,
    SPELL_CHECK_FOR_PLAYERS         = 70332,                // check for aura 70120 or 70121 on player; if not found cast 67335
    SPELL_COLDFLAME_JETS            = 70460,
    SPELL_WEB_BEAM_2                = 69986,

    POINT_GAUNTLET_LAND             = 1,
};

namespace
{
Transport* GetGunshipTransport(Map* map, uint32 entry)
{
    for (Transport* transport : map->GetTransports())
        if (transport->GetEntry() == entry)
            return transport;

    return nullptr;
}

void StartGunshipTransport(Map* map, uint32 entry)
{
    if (Transport* transport = GetGunshipTransport(map, entry))
        transport->StartMovementNow();
}
}

static const DialogueEntry aCitadelDialogue[] =
{
    // Deathwhisper dialogue
    {SAY_DEATHWHISPER_SPEECH_1,  NPC_LADY_DEATHWHISPER,  12000},
    {SAY_DEATHWHISPER_SPEECH_2,  NPC_LADY_DEATHWHISPER,  11000},
    {SAY_DEATHWHISPER_SPEECH_3,  NPC_LADY_DEATHWHISPER,  10000},
    {SAY_DEATHWHISPER_SPEECH_4,  NPC_LADY_DEATHWHISPER,  9000},
    {SAY_DEATHWHISPER_SPEECH_5,  NPC_LADY_DEATHWHISPER,  10000},
    {SAY_DEATHWHISPER_SPEECH_6,  NPC_LADY_DEATHWHISPER,  10000},
    {SAY_DEATHWHISPER_SPEECH_7,  NPC_LADY_DEATHWHISPER,  0},

    // Gunship dialogue - alliance
    {SAY_GUNSHIP_START_ALLY_1,  NPC_GUNSHIP_MURADIN,  6000},
    {SAY_GUNSHIP_START_ALLY_2,  NPC_GUNSHIP_MURADIN,  20000},
    {SAY_GUNSHIP_START_ALLY_3,  NPC_GUNSHIP_MURADIN,  5000},
    {SAY_GUNSHIP_START_ALLY_4,  NPC_GUNSHIP_MURADIN,  6000},
    {SAY_GUNSHIP_START_ALLY_5,  NPC_GUNSHIP_MURADIN,  8000},
    {SAY_GUNSHIP_START_ALLY_6,  NPC_GUNSHIP_MURADIN,  5000},
    {SAY_GUNSHIP_START_ALLY_7,  NPC_GUNSHIP_SAURFANG, 6000},
    {SAY_GUNSHIP_START_ALLY_8,  NPC_GUNSHIP_MURADIN,  0},           // start encounter

    // Gunship dialogue - horde
    {SAY_GUNSHIP_START_HORDE_1, NPC_GUNSHIP_SAURFANG, 10000},
    {SAY_GUNSHIP_START_HORDE_2, NPC_GUNSHIP_SAURFANG, 15000},
    {SAY_GUNSHIP_START_HORDE_3, NPC_GUNSHIP_SAURFANG, 20000},
    {SAY_GUNSHIP_START_HORDE_4, NPC_GUNSHIP_SAURFANG, 6000},
    {SAY_GUNSHIP_START_HORDE_5, NPC_GUNSHIP_SAURFANG, 6000},
    {SAY_GUNSHIP_START_HORDE_6, NPC_GUNSHIP_MURADIN,  6000},
    {SAY_GUNSHIP_START_HORDE_7, NPC_GUNSHIP_SAURFANG, 0},           // start encounter

    {0, 0, 0},
};

instance_icecrown_citadel::instance_icecrown_citadel(Map* pMap) : ScriptedInstance(pMap), DialogueHelper(aCitadelDialogue),
    m_uiTeam(0),
    m_uiPutricideValveTimer(0),
    m_uiGunshipResetTimer(0),
    m_uiGunshipVictoryTeleportTimer(0),
    m_uiColdflameJetsState(NOT_STARTED),
    m_uiSindragosaGauntletState(NOT_STARTED),
    m_uiLightsHammerDamnedKills(0),
    m_bHasMarrowgarIntroYelled(false),
    m_bHasDeathwhisperIntroYelled(false),
    m_bHasRimefangLanded(false),
    m_bHasSpinestalkerLanded(false),
    m_bGunshipReloadPending(false)
{
    Initialize();
}

void instance_icecrown_citadel::Initialize()
{
    InitializeDialogueHelper(this);
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    m_uiGunshipResetTimer = 0;
    m_uiGunshipVictoryTeleportTimer = 0;
    m_uiColdflameJetsState = NOT_STARTED;
    m_uiSindragosaGauntletState = NOT_STARTED;
    m_bGunshipReloadPending = false;
    m_uiLightsHammerDamnedKills = 0;
    m_sLightsHammerDamnedGuids.clear();
    m_sRimefangTrashGuids.clear();
    m_sSpinestalkerTrashGuids.clear();

    for (bool& i : m_abAchievCriteria)
        i = false;
}

bool instance_icecrown_citadel::IsEncounterInProgress() const
{
    for (uint32 i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            return true;
    }

    return m_uiSindragosaGauntletState == IN_PROGRESS;
}

void instance_icecrown_citadel::DoHandleCitadelAreaTrigger(uint32 uiTriggerId, Player* pPlayer)
{
    if (uiTriggerId == AT_MARROWGAR_INTRO && !m_bHasMarrowgarIntroYelled)
    {
        if (Creature* pMarrowgar = GetSingleCreatureFromStorage(NPC_LORD_MARROWGAR))
        {
            DoScriptText(SAY_MARROWGAR_INTRO, pMarrowgar);
            m_bHasMarrowgarIntroYelled = true;
        }
    }
    else if (uiTriggerId == AT_DEATHWHISPER_INTRO && !m_bHasDeathwhisperIntroYelled)
    {
        StartNextDialogueText(SAY_DEATHWHISPER_SPEECH_1);
        m_bHasDeathwhisperIntroYelled = true;
    }
    else if (uiTriggerId == AT_SINDRAGOSA_PLATFORM)
    {
        if (Creature* pSindragosa = GetSingleCreatureFromStorage(NPC_SINDRAGOSA))
        {
            if (pSindragosa->IsAlive() && !pSindragosa->IsInCombat())
                pSindragosa->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pPlayer, pSindragosa);
        }
        else
        {
            // This trigger is only a safety net. The retail progression is
            // driven by clearing each frostwyrm's whelp group.
            if (m_sRimefangTrashGuids.empty())
                StartSindragosaFrostwyrm(NPC_RIMEFANG, pPlayer);
            if (m_sSpinestalkerTrashGuids.empty())
                StartSindragosaFrostwyrm(NPC_SPINESTALKER, pPlayer);
        }
    }
    else if (uiTriggerId == AT_SINDRAGOSA_GAUNTLET)
    {
        // Progression to this room is controlled by Valithria's exit door.
        // Once a player legitimately reaches the retail area trigger, start
        // the gauntlet exactly as the reference implementations do. A second
        // Valithria state gate here left the room permanently empty whenever
        // the completed encounter and the trigger loaded in different grids.
        if (m_uiSindragosaGauntletState != NOT_STARTED)
            return;

        if (Creature* controller = GetSingleCreatureFromStorage(NPC_SINDRAGOSA_GAUNTLET))
            controller->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pPlayer, controller);
    }
    else if (uiTriggerId == AT_SAURFANG_PORTAL)
    {
        if (GetData(TYPE_DEATHBRINGER_SAURFANG) != DONE)
            return;

        float const destinationX = 4126.35f;
        float const destinationY = 2769.23f;
        float const destinationZ = 350.963f;
        float const destinationO = 0.0f;

        if (m_uiColdflameJetsState == NOT_STARTED)
        {
            // Preload the destination grid before collecting its trap
            // creatures. This is the same localized relocation sequence used
            // by the reference implementations; the player's real teleport
            // still happens only after the trap schedule is initialized.
            float originalX, originalY, originalZ, originalO;
            pPlayer->GetPosition(originalX, originalY, originalZ);
            originalO = pPlayer->GetOrientation();
            pPlayer->GetMap()->PlayerRelocation(pPlayer, destinationX, destinationY, destinationZ, destinationO);

            m_uiColdflameJetsState = IN_PROGRESS;
            CreatureList traps;
            GetCreatureListWithEntryInGrid(traps, pPlayer, NPC_FROST_FREEZE_TRAP, 120.0f);
            traps.sort([pPlayer](Creature* left, Creature* right)
            {
                return pPlayer->GetDistance(left) < pPlayer->GetDistance(right);
            });

            bool activateSoon = false;
            for (Creature* trap : traps)
            {
                trap->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, trap, trap, activateSoon ? 1000 : 11000);
                activateSoon = !activateSoon;
            }

            pPlayer->GetMap()->PlayerRelocation(pPlayer, originalX, originalY, originalZ, originalO);
        }

        pPlayer->TeleportTo(instance->GetId(), destinationX, destinationY, destinationZ, destinationO);
    }
    else if (uiTriggerId == AT_SHUTDOWN_FROST_JETS)
        SetData(DATA_COLDFLAME_JETS, DONE);
}

// Upper Spire's retail gauntlet alternates two trap rows. Each trap receives
// either a 1-second or 11-second initial offset and repeats every 22 seconds.
struct npc_frost_freeze_trapAI : public ScriptedAI
{
    npc_frost_freeze_trapAI(Creature* creature) : ScriptedAI(creature),
        m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData())),
        m_activationTimer(0)
    {
        SetCombatMovement(false);
        m_creature->SetCanEnterCombat(false);
    }

    instance_icecrown_citadel* m_instance;
    uint32 m_activationTimer;

    void Reset() override
    {
        m_activationTimer = 0;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && (miscValue == 1000 || miscValue == 11000))
            m_activationTimer = miscValue;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_activationTimer)
            return;

        if (m_activationTimer > diff)
        {
            m_activationTimer -= diff;
            return;
        }

        m_activationTimer = 0;
        if (m_instance && m_instance->GetData(DATA_COLDFLAME_JETS) == IN_PROGRESS)
        {
            DoCastSpellIfCan(m_creature, SPELL_COLDFLAME_JETS);
            m_activationTimer = 22000;
        }
    }
};

UnitAI* GetAI_npc_frost_freeze_trap(Creature* creature)
{
    return new npc_frost_freeze_trapAI(creature);
}

// Frostwing's post-Valithria gauntlet is a scripted, three-wave retail event;
// the creatures do not exist as permanent DB spawns.  The controller owns
// only its temporary summons, so reset cleanup is bounded and cannot leak
// creatures or timers into later attempts.
struct npc_sindragosa_gauntlet_controllerAI : public ScriptedAI
{
    npc_sindragosa_gauntlet_controllerAI(Creature* creature) : ScriptedAI(creature),
        m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData())),
        m_active(false), m_phase(0), m_checkTimer(0), m_broodlingTimer(0), m_broodlingsLeft(0)
    {
        SetCombatMovement(false);
        m_creature->SetCanEnterCombat(false);
        // Sindragosa's Ward is a server-side event controller.  Its creature
        // model is never presented to players on retail.
        m_creature->SetVisibility(VISIBILITY_OFF);
        Reset();
    }

    instance_icecrown_citadel* m_instance;
    GuidList m_summons;
    bool m_active;
    uint8 m_phase;
    uint32 m_checkTimer;
    uint32 m_broodlingTimer;
    uint8 m_broodlingsLeft;

    void DespawnSummons()
    {
        GuidList summons = m_summons;
        m_summons.clear();
        for (ObjectGuid const& guid : summons)
            if (Creature* summon = m_creature->GetMap()->GetCreature(guid))
                summon->ForcedDespawn();
    }

    Creature* Summon(uint32 entry, float x, float y, float z, float o)
    {
        return m_creature->SummonCreature(entry, x, y, z, o,
            TEMPSPAWN_CORPSE_TIMED_DESPAWN, 10 * MINUTE * IN_MILLISECONDS);
    }

    void SummonSpiders()
    {
        Summon(NPC_NERUBAR_CHAMPION,  4207.30f, 2532.00f, 256.0f, 4.253f);
        Summon(NPC_NERUBAR_WEBWEAVER, 4228.79f, 2510.36f, 256.0f, 3.577f);
        Summon(NPC_NERUBAR_CHAMPION,  4228.34f, 2458.20f, 256.0f, 2.642f);
        Summon(NPC_NERUBAR_WEBWEAVER, 4207.54f, 2437.18f, 256.0f, 2.073f);
        Summon(NPC_NERUBAR_CHAMPION,  4156.20f, 2436.80f, 256.0f, 1.083f);
        Summon(NPC_NERUBAR_WEBWEAVER, 4133.50f, 2459.28f, 256.0f, 0.483f);
        Summon(NPC_NERUBAR_CHAMPION,  4134.28f, 2509.71f, 256.0f, 5.788f);
        Summon(NPC_NERUBAR_WEBWEAVER, 4156.29f, 2532.19f, 256.0f, 5.187f);
    }

    void SummonFrostwardens()
    {
        for (uint8 i = 0; i < 3; ++i)
        {
            uint32 entry = i == 1 ? NPC_FROSTWARDEN_SORCERESS : NPC_FROSTWARDEN_WARRIOR;
            Summon(entry, 4173.94f + i * 7.0f, 2409.15f, 211.033f, 1.56f);
            Summon(entry, 4173.94f + i * 7.0f, 2556.71f, 211.033f, 4.712f);
        }
    }

    void MoveSpidersDown()
    {
        for (ObjectGuid const& guid : m_summons)
        {
            Creature* spider = m_creature->GetMap()->GetCreature(guid);
            if (!spider || !spider->IsAlive() || spider->GetPositionZ() <= 220.0f)
                continue;

            spider->CastSpell(spider, SPELL_WEB_BEAM_2, TRIGGERED_OLD_TRIGGERED);
            spider->GetMotionMaster()->MovePoint(POINT_GAUNTLET_LAND,
                Position(spider->GetPositionX(), spider->GetPositionY(), 213.03f, spider->GetOrientation()),
                FORCED_MOVEMENT_FLIGHT, 12.0f, false, ObjectGuid(), 0, AnimTier::Hover);
        }
    }

    void StartBroodlings()
    {
        m_broodlingsLeft = 30;
        m_broodlingTimer = 10000;
    }

    void Reset() override
    {
        DespawnSummons();
        m_active = false;
        m_phase = 0;
        m_checkTimer = 0;
        m_broodlingTimer = 0;
        m_broodlingsLeft = 0;

        if (m_instance && m_instance->GetData(DATA_SINDRAGOSA_GAUNTLET) != DONE)
        {
            m_instance->SetData(DATA_SINDRAGOSA_GAUNTLET, NOT_STARTED);
            SummonSpiders();
        }
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType != AI_EVENT_CUSTOM_A || m_active || !m_instance ||
                m_instance->GetData(DATA_SINDRAGOSA_GAUNTLET) != NOT_STARTED)
            return;

        m_active = true;
        m_phase = 1;
        m_checkTimer = 1000;
        m_instance->SetData(DATA_SINDRAGOSA_GAUNTLET, IN_PROGRESS);
        StartBroodlings();
        MoveSpidersDown();
    }

    void JustSummoned(Creature* summon) override
    {
        m_summons.push_back(summon->GetObjectGuid());
        if (summon->GetPositionZ() > 220.0f)
        {
            summon->SetLevitate(true);
            summon->SetAnimTier(AnimTier::Hover);
            summon->SetWalk(true);
        }
        else if (m_active)
        {
            summon->SetInCombatWithZone();
            summon->AI()->AttackClosestEnemy();
        }
    }

    void SummonedMovementInform(Creature* summon, uint32 motionType, uint32 pointId) override
    {
        if (motionType != POINT_MOTION_TYPE || pointId != POINT_GAUNTLET_LAND)
            return;

        summon->SetLevitate(false);
        summon->SetAnimTier(AnimTier::Ground);
        summon->SetWalk(false);
        summon->SetInCombatWithZone();
        summon->AI()->AttackClosestEnemy();
    }

    bool HasLivingMainWaveCreature() const
    {
        for (ObjectGuid const& guid : m_summons)
            if (Creature* summon = m_creature->GetMap()->GetCreature(guid))
                if (summon->IsAlive() && summon->GetEntry() != NPC_NERUBAR_BROODLING)
                    return true;
        return false;
    }

    void SummonedCreatureJustDied(Creature* summon) override
    {
        m_summons.remove(summon->GetObjectGuid());
        if (!m_active || summon->GetEntry() == NPC_NERUBAR_BROODLING || HasLivingMainWaveCreature())
            return;

        if (m_phase == 1)
        {
            m_phase = 2;
            SummonFrostwardens();
            StartBroodlings();
        }
        else if (m_phase == 2)
        {
            m_phase = 3;
            SummonSpiders();
            StartBroodlings();
            MoveSpidersDown();
        }
        else
        {
            m_active = false;
            m_broodlingTimer = 0;
            m_broodlingsLeft = 0;
            m_instance->SetData(DATA_SINDRAGOSA_GAUNTLET, DONE);
        }
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        m_summons.remove(summon->GetObjectGuid());
    }

    void SummonBroodling()
    {
        float distance = frand(18.0f, 39.0f);
        float angle = frand(0.0f, 2.0f * M_PI_F);
        float x = m_creature->GetPositionX() + std::cos(angle) * distance;
        float y = m_creature->GetPositionY() + std::sin(angle) * distance;
        if (Creature* broodling = Summon(NPC_NERUBAR_BROODLING, x, y, 250.0f,
                MapManager::NormalizeOrientation(angle - M_PI_F)))
        {
            broodling->CastSpell(broodling, SPELL_WEB_BEAM_2, TRIGGERED_OLD_TRIGGERED);
            broodling->GetMotionMaster()->MovePoint(POINT_GAUNTLET_LAND,
                Position(x, y, 213.03f, broodling->GetOrientation()), FORCED_MOVEMENT_FLIGHT,
                12.0f, false, ObjectGuid(), 0, AnimTier::Hover);
        }
    }

    bool HasNearbyPlayer() const
    {
        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
        {
            Player* player = playerRef.getSource();
            if (player && player->IsAlive() && m_creature->IsWithinDistInMap(player, 100.0f))
                return true;
        }
        return false;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_active)
        {
            // Area trigger 5623 is the primary retail start signal.  Keep a
            // tightly bounded proximity fallback at that same location so a
            // late-loaded controller/grid cannot leave the room permanently
            // empty after legitimate Valithria progression.
            if (!m_instance || m_instance->GetData(DATA_SINDRAGOSA_GAUNTLET) != NOT_STARTED)
                return;

            if (m_checkTimer > diff)
            {
                m_checkTimer -= diff;
                return;
            }

            m_checkTimer = 1000;
            for (auto& playerRef : m_creature->GetMap()->GetPlayers())
            {
                Player* player = playerRef.getSource();
                if (!player || !player->IsAlive() || player->IsGameMaster())
                    continue;

                float const dx = player->GetPositionX() - 4181.28f;
                float const dy = player->GetPositionY() - 2483.65f;
                if (dx * dx + dy * dy <= 21.21f * 21.21f)
                {
                    ReceiveAIEvent(AI_EVENT_CUSTOM_A, player, m_creature, 0);
                    break;
                }
            }
            return;
        }

        if (m_checkTimer <= diff)
        {
            m_checkTimer = 1000;
            if (!HasNearbyPlayer())
            {
                Reset();
                return;
            }
        }
        else
            m_checkTimer -= diff;

        if (!m_broodlingsLeft)
            return;

        if (m_broodlingTimer <= diff)
        {
            SummonBroodling();
            --m_broodlingsLeft;
            m_broodlingTimer = m_broodlingsLeft ? 350 : 0;
        }
        else
            m_broodlingTimer -= diff;
    }
};

UnitAI* GetAI_npc_sindragosa_gauntlet_controller(Creature* creature)
{
    return new npc_sindragosa_gauntlet_controllerAI(creature);
}

void instance_icecrown_citadel::StartSindragosaFrostwyrm(uint32 entry, Player* player)
{
    bool& landed = entry == NPC_RIMEFANG ? m_bHasRimefangLanded : m_bHasSpinestalkerLanded;
    if (landed)
        return;

    if (!player)
        player = GetPlayerInMap(true, false);

    if (player)
    {
        if (Creature* frostwyrm = GetSingleCreatureFromStorage(entry))
        {
            frostwyrm->AI()->AttackStart(player);
            landed = true;
        }
    }
}

void instance_icecrown_citadel::OpenSindragosaShortcut()
{
    DoUseOpenableObject(GO_SINDRAGOSA_SHORTCUT_ENTRANCE, true);
    DoUseOpenableObject(GO_SINDRAGOSA_SHORTCUT_EXIT, true);
}

void instance_icecrown_citadel::OnPlayerEnter(Player* pPlayer)
{
    if (!m_uiTeam)                      // very first player to enter
    {
        m_uiTeam = pPlayer->GetTeam();

        ProcessEventNpcs(pPlayer);
    }

    // Rocket packs are encounter tools with no charges. They persist through
    // a wipe for the next attempt, but must be removed once Gunship is DONE,
    // including from a player who logged out before the victory transport
    // stopped.
    if (m_auiEncounter[TYPE_GUNSHIP_BATTLE] == DONE)
        pPlayer->DestroyItemCount(ITEM_GOBLIN_ROCKET_PACK,
            pPlayer->GetItemCount(ITEM_GOBLIN_ROCKET_PACK), true);

    // Static creature respawn timers are saved per instance. If the world
    // server stopped after the starter archmages died, their normal seven-day
    // timer could otherwise survive even though Valithria reset correctly.
    if (m_auiEncounter[TYPE_VALITHRIA] != DONE &&
        m_auiEncounter[TYPE_VALITHRIA] != IN_PROGRESS)
        RespawnValithriaStarterPack();
}

void instance_icecrown_citadel::RespawnValithriaStarterPack()
{
    if (SpawnGroup* group = instance->GetSpawnManager().GetSpawnGroup(SPAWN_GROUP_VALITHRIA_STARTERS))
        group->Spawn(true, true);
}

void instance_icecrown_citadel::OnPlayerLeave(Player* pPlayer)
{
    if (!pPlayer)
        return;

    // The Goblin Rocket Pack is an ICC Gunship encounter tool. Retail keeps
    // it through a wipe so the raid can immediately make another attempt, but
    // it must not leave map 631 with the player. DestroyItemCount covers both
    // equipped and bagged copies and is also safe when the count is zero.
    pPlayer->DestroyItemCount(ITEM_GOBLIN_ROCKET_PACK,
        pPlayer->GetItemCount(ITEM_GOBLIN_ROCKET_PACK), true);
}

void instance_icecrown_citadel::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_LORD_MARROWGAR:
        case NPC_LADY_DEATHWHISPER:
        case NPC_DEATHBRINGER_SAURFANG:
        case NPC_FESTERGUT:
        case NPC_ROTFACE:
        case NPC_PROFESSOR_PUTRICIDE:
        case NPC_TALDARAM:
        case NPC_VALANAR:
        case NPC_KELESETH:
        case NPC_LANATHEL_INTRO:
        case NPC_SINDRAGOSA:
        case NPC_LICH_KING:
        case NPC_TIRION_FORDRING:
        case NPC_TIRION_LIGHTS_HAMMER:
        case NPC_RIMEFANG:
        case NPC_SPINESTALKER:
        case NPC_CAPTAIN_ARNATH:
        case NPC_CAPTAIN_BRANDON:
        case NPC_CAPTAIN_GRONDEL:
        case NPC_CAPTAIN_RUPERT:
        case NPC_SISTER_SVALNA:
        case NPC_CROK_SCOURGEBANE:
        case NPC_VALITHRIA_COMBAT_TRIGGER:
        case NPC_SINDRAGOSA_GAUNTLET:
        case NPC_BLOOD_ORB_CONTROL:
        case NPC_PUTRICIDES_TRAP:
        case NPC_GAS_STALKER:
        case NPC_OOZE_TENTACLE_STALKER:
        case NPC_SLIMY_TENTACLE_STALKER:
        case NPC_GUNSHIP_SAURFANG:
        case NPC_GUNSHIP_MURADIN:
        case NPC_SKYBREAKER:
        case NPC_ORGRIMS_HAMMER:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_VALITHRIA:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            // A fresh/recovered Valithria spawn must be at 50 percent.  DONE
            // is the only state where the saved creature represents the
            // successfully healed dragon.
            pCreature->SetHealth(m_auiEncounter[TYPE_VALITHRIA] == DONE ?
                pCreature->GetMaxHealth() : pCreature->GetMaxHealth() / 2);
            break;
        case NPC_THE_DAMNED:
            // Only the two Damned immediately in front of Light's Hammer start
            // the prologue. Record them at spawn time so later patrol movement
            // cannot make another pack satisfy the positional check.
            if (pCreature->GetPositionX() > -142.0f && pCreature->GetPositionX() < -138.0f &&
                    pCreature->GetPositionY() > 2204.0f && pCreature->GetPositionY() < 2219.0f)
                m_sLightsHammerDamnedGuids.insert(pCreature->GetObjectGuid());
            break;
        case NPC_SPIRE_FROSTWYRM:
            if (pCreature->IsTemporarySummon())
                m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_DEATHWHISPER_SPAWN_STALKER:
            m_lDeathwhisperStalkersGuids.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_CULT_ADHERENT:
        case NPC_CULT_FANATIC:
        case NPC_REANIMATED_FANATIC:
        case NPC_REANIMATED_ADHERENT:
            m_lDeathwhisperCultistsGuids.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_DARFALLEN_NOBLE:
        case NPC_DARKFALLEN_ARCHMAGE:
        case NPC_DARKFALLEN_BLOOD_KNIGHT:
        case NPC_DARKFALLEN_ADVISOR:
            if (pCreature->GetPositionZ() < 352.0f)
                m_sDarkfallenCreaturesLowerGuids.insert(pCreature->GetObjectGuid());
            else if (pCreature->GetPositionZ() < 400.0f)
            {
                if (pCreature->GetPositionY() < 2800.0f)
                    m_sDarkfallenCreaturesRightGuids.insert(pCreature->GetObjectGuid());
                else
                    m_sDarkfallenCreaturesLeftGuids.insert(pCreature->GetObjectGuid());
            }
            return;
        case NPC_PUDDLE_STALKER:
            // select Puddle Stalkers only from Rotface encounter, upper plan
            if (pCreature->GetPositionX() > 4350.0f && pCreature->GetPositionZ() > 365.0f)
                m_lRotfaceUpperStalkersGuids.push_back(pCreature->GetObjectGuid());
            return;
        case NPC_MAD_SCIENTIST_STALKER:
            if (pCreature->GetPositionX() < 4350.0f)
                m_leftScientistStalkerGuid = pCreature->GetObjectGuid();
            else
                m_rightScientistStalkerGuid = pCreature->GetObjectGuid();
            return;
        case NPC_FROSTWING_WHELP:
        {
            float x, y, z;
            pCreature->GetRespawnCoord(x, y, z);
            if (y < 2484.35f)
                m_sRimefangTrashGuids.insert(pCreature->GetObjectGuid());
            else
                m_sSpinestalkerTrashGuids.insert(pCreature->GetObjectGuid());
            return;
        }
    }
}

void instance_icecrown_citadel::OnCreatureRespawn(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        // following have passive behavior movement
        case NPC_COLDFLAME:
        case NPC_DEATHWHISPER_SPAWN_STALKER:
        case NPC_FROST_FREEZE_TRAP:
        case NPC_SKYBREAKER:
        case NPC_ORGRIMS_HAMMER:
            pCreature->AI()->SetReactState(REACT_PASSIVE);
            pCreature->SetCanEnterCombat(false);
            break;
    }
}

void instance_icecrown_citadel::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_ICEWALL_1:
        case GO_ICEWALL_2:
        case GO_ORATORY_DOOR:
            if (m_auiEncounter[TYPE_MARROWGAR] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_DEATHWHISPER_ELEVATOR:
            break;
        case GO_SAURFANG_DOOR:
            if (m_auiEncounter[TYPE_DEATHBRINGER_SAURFANG] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ALLIANCE_TELEPORTER:
            m_lFactionTeleporterGuids[TEAM_INDEX_ALLIANCE].push_back(pGo->GetObjectGuid());
            return;
        case GO_HORDE_TELEPORTER:
            m_lFactionTeleporterGuids[TEAM_INDEX_HORDE].push_back(pGo->GetObjectGuid());
            return;
        case GO_SCIENTIST_DOOR:
            if (m_auiEncounter[TYPE_PLAGUE_WING_ENTRANCE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_CRIMSON_HALL_DOOR:
            if (m_auiEncounter[TYPE_BLOOD_WING_ENTRANCE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_GREEN_DRAGON_ENTRANCE:
            if (m_auiEncounter[TYPE_FROST_WING_ENTRANCE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ORANGE_TUBE:
            if (m_auiEncounter[TYPE_FESTERGUT] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_GREEN_TUBE:
            if (m_auiEncounter[TYPE_ROTFACE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ORANGE_VALVE:
            if (m_auiEncounter[TYPE_FESTERGUT] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_GREEN_VALVE:
            if (m_auiEncounter[TYPE_ROTFACE] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_SCIENTIST_DOOR_GREEN:
            // If both Festergut and Rotface are DONE, set as ACTIVE_ALTERNATIVE
            if (m_auiEncounter[TYPE_FESTERGUT] == DONE && m_auiEncounter[TYPE_ROTFACE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            else if (m_auiEncounter[TYPE_ROTFACE] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_SCIENTIST_DOOR_ORANGE:
            // If both Festergut and Rotface are DONE, set as ACTIVE_ALTERNATIVE
            if (m_auiEncounter[TYPE_FESTERGUT] == DONE && m_auiEncounter[TYPE_ROTFACE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            else if (m_auiEncounter[TYPE_FESTERGUT] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_SCIENTIST_DOOR_COLLISION:
            if (m_auiEncounter[TYPE_FESTERGUT] == DONE && m_auiEncounter[TYPE_ROTFACE] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_COUNCIL_DOOR_1:
        case GO_COUNCIL_DOOR_2:
            if (m_auiEncounter[TYPE_BLOOD_PRINCE_COUNCIL] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_GREEN_DRAGON_EXIT:
            if (m_auiEncounter[TYPE_VALITHRIA] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SINDRAGOSA_ENTRANCE:
            // This is the passage at the end of the Frostwing gauntlet.  It
            // unlocks with that event, not merely with Valithria's death.
            if (m_uiSindragosaGauntletState == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SINDRAGOSA_SHORTCUT_ENTRANCE:
        case GO_SINDRAGOSA_SHORTCUT_EXIT:
            if (m_auiEncounter[TYPE_SINDRAGOSA] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SAURFANG_CACHE:
        case GO_SAURFANG_CACHE_25:
        case GO_SAURFANG_CACHE_10_H:
        case GO_SAURFANG_CACHE_25_H:
            m_goEntryGuidStore[GO_SAURFANG_CACHE] = pGo->GetObjectGuid();
            // The cache can enter the loaded grid after Saurfang is already
            // marked DONE. Unlock it here too, otherwise the valid loot chest
            // remains hidden or non-interactable until another state change.
            if (m_auiEncounter[TYPE_DEATHBRINGER_SAURFANG] == DONE)
            {
                DoRespawnGameObject(pGo->GetObjectGuid(), 60 * MINUTE);
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }
            return;
        case GO_GUNSHIP_ARMORY_A:
        case GO_GUNSHIP_ARMORY_A_25:
        case GO_GUNSHIP_ARMORY_A_10H:
        case GO_GUNSHIP_ARMORY_A_25H:
            m_goEntryGuidStore[GO_GUNSHIP_ARMORY_A] = pGo->GetObjectGuid();
            return;
        case GO_GUNSHIP_ARMORY_H:
        case GO_GUNSHIP_ARMORY_H_25:
        case GO_GUNSHIP_ARMORY_H_10H:
        case GO_GUNSHIP_ARMORY_H_25H:
            m_goEntryGuidStore[GO_GUNSHIP_ARMORY_H] = pGo->GetObjectGuid();
            return;
        case GO_DREAMWALKER_CACHE:
        case GO_DREAMWALKER_CACHE_25:
        case GO_DREAMWALKER_CACHE_10_H:
        case GO_DREAMWALKER_CACHE_25_H:
            m_goEntryGuidStore[GO_DREAMWALKER_CACHE] = pGo->GetObjectGuid();
            // Valithria's controller summons the cache a few seconds after the
            // encounter is marked DONE. SetData therefore cannot unlock an
            // object that does not exist yet; mirror the Saurfang cache's
            // late-load handling so the newly created reward is usable.
            if (m_auiEncounter[TYPE_VALITHRIA] == DONE)
            {
                DoRespawnGameObject(pGo->GetObjectGuid(), 60 * MINUTE);
                pGo->RemoveFlag(GAMEOBJECT_FLAGS,
                    GO_FLAG_LOCKED | GO_FLAG_INTERACT_COND | GO_FLAG_NO_INTERACT);
                pGo->SetLootState(GO_READY);
                pGo->SetGoState(GO_STATE_READY);
            }
            return;
        case GO_MARROWGAR_DOOR:
            // Combat boundary: open on a fresh/reset instance and lock only
            // while Marrowgar is actively in combat.
            if (m_auiEncounter[TYPE_MARROWGAR] != IN_PROGRESS)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ICESHARD_1:
        case GO_ICESHARD_2:
        case GO_ICESHARD_3:
        case GO_ICESHARD_4:
        case GO_FROSTY_WIND:
        case GO_FROSTY_EDGE:
        case GO_SNOW_EDGE:
        case GO_ARTHAS_PLATFORM:
        case GO_ARTHAS_PRECIPICE:
        case GO_BLOODPRINCE_DOOR:
        case GO_VALITHRIA_DOOR_1:
        case GO_VALITHRIA_DOOR_2:
        case GO_VALITHRIA_DOOR_3:
        case GO_VALITHRIA_DOOR_4:
        case GO_ICECROWN_GRATE:
        case GO_ORANGE_PLAGUE:
        case GO_GREEN_PLAGUE:
            break;
        case GO_DRINK_ME:
            // The abomination table is available only while Putricide is in
            // the phases which use it. It must not remain clickable before a
            // pull, after a wipe/death, or after the phase-three transition.
            DoToggleGameObjectFlags(pGo->GetObjectGuid(), GO_FLAG_NO_INTERACT,
                m_auiEncounter[TYPE_PROFESSOR_PUTRICIDE] != IN_PROGRESS);
            break;
        case GO_PLAGUE_SIGIL:
            if (m_auiEncounter[TYPE_PROFESSOR_PUTRICIDE] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_FROSTWING_SIGIL:
            if (m_auiEncounter[TYPE_SINDRAGOSA] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_BLOODWING_SIGIL:
            if (m_auiEncounter[TYPE_QUEEN_LANATHEL] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_TRANSPORTER_FROZEN_THRONE:
            if (m_auiEncounter[TYPE_PROFESSOR_PUTRICIDE] == DONE && m_auiEncounter[TYPE_QUEEN_LANATHEL] == DONE && m_auiEncounter[TYPE_SINDRAGOSA] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_TRANSPORTER_UPPER_SPIRE:
            if (m_auiEncounter[TYPE_DEATHBRINGER_SAURFANG] == DONE)
            {
                pGo->SetGoState(GO_STATE_ACTIVE);
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }
            break;
        case GO_TRANSPORTER_LIGHTS_HAMMER:
        case GO_TRANSPORTER_ORATORY_DAMNED:
            if (m_auiEncounter[TYPE_MARROWGAR] == DONE)
            {
                pGo->SetGoState(GO_STATE_ACTIVE);
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }
            break;
        case GO_TRANSPORTER_RAMPART_SKULLS:
            if (m_auiEncounter[TYPE_LADY_DEATHWHISPER] == DONE)
            {
                pGo->SetGoState(GO_STATE_ACTIVE);
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }
            break;
        case GO_TRANSPORTER_DEATHBRINGER:
            if (m_auiEncounter[TYPE_GUNSHIP_BATTLE] == DONE)
            {
                pGo->SetGoState(GO_STATE_ACTIVE);
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }
            break;
        case GO_TRANSPORTER_SINDRAGOSA:
            if (m_auiEncounter[TYPE_VALITHRIA] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_icecrown_citadel::OnObjectSpawn(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_DEATHWHISPER_ELEVATOR:
            if (m_auiEncounter[TYPE_LADY_DEATHWHISPER] == DONE)
                pGo->SetGoState(GO_STATE_READY);
            break;
    }
}

void instance_icecrown_citadel::OnCreatureEnterCombat(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_DARFALLEN_NOBLE:
        case NPC_DARKFALLEN_ARCHMAGE:
        case NPC_DARKFALLEN_BLOOD_KNIGHT:
        case NPC_DARKFALLEN_ADVISOR:
            // ToDo: cast SPELL_SIPHON_ESSENCE on combat
            return;
    }
}

void instance_icecrown_citadel::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_THE_DAMNED:
            // The old EventAI chain depended on one Damned being placed in an
            // undocumented phase. That phase was never reliably set, leaving
            // Tirion's 156-second prologue stuck. Own the two-kill gate in the
            // instance and keep the existing CMaNGOS movement/dialogue script.
            if (m_sLightsHammerDamnedGuids.erase(pCreature->GetObjectGuid()) &&
                    ++m_uiLightsHammerDamnedKills == 2)
            {
                if (Creature* pTirion = GetSingleCreatureFromStorage(NPC_TIRION_LIGHTS_HAMMER))
                {
                    // Start the existing 156-second movement/dialogue script
                    // directly. Relaying this through Tirion's EventAI made the
                    // prologue depend on his template AI assignment and could
                    // silently leave the event idle even after both gate mobs
                    // were killed.
                    pTirion->StopMoving();
                    pTirion->GetMotionMaster()->Clear(false, true);
                    pTirion->GetMotionMaster()->MoveWaypoint();
                }
            }
            break;
        case NPC_STINKY:
            if (Creature* pFestergut = GetSingleCreatureFromStorage(NPC_FESTERGUT))
            {
                if (pFestergut->IsAlive())
                    DoScriptText(SAY_STINKY_DIES, pFestergut);
            }
            break;
        case NPC_PRECIOUS:
            if (Creature* pRotface = GetSingleCreatureFromStorage(NPC_ROTFACE))
            {
                if (pRotface->IsAlive())
                    DoScriptText(SAY_PRECIOUS_DIES, pRotface);
            }
            break;
        case NPC_CULT_ADHERENT:
        case NPC_CULT_FANATIC:
        case NPC_EMPOWERED_ADHERENT:
        case NPC_DEFORMED_FANATIC:
        case NPC_REANIMATED_FANATIC:
        case NPC_REANIMATED_ADHERENT:
            m_lDeathwhisperCultistsGuids.remove(pCreature->GetObjectGuid());
            return;
        case NPC_DARFALLEN_NOBLE:
        case NPC_DARKFALLEN_ARCHMAGE:
        case NPC_DARKFALLEN_BLOOD_KNIGHT:
        case NPC_DARKFALLEN_ADVISOR:
            // lower pack
            if (m_sDarkfallenCreaturesLowerGuids.find(pCreature->GetObjectGuid()) != m_sDarkfallenCreaturesLowerGuids.end())
            {
                m_sDarkfallenCreaturesLowerGuids.erase(pCreature->GetObjectGuid());

                if (m_sDarkfallenCreaturesLowerGuids.empty())
                {
                    if (GetData(TYPE_BLOOD_WING_ENTRANCE) != DONE)
                        SetData(TYPE_BLOOD_WING_ENTRANCE, DONE);

                    if (GameObject* pOrb = GetClosestGameObjectWithEntry(pCreature, GO_EMPOWERING_BLOOD_ORB, 30.0f))
                        DoToggleGameObjectFlags(pOrb->GetObjectGuid(), GO_FLAG_NO_INTERACT, false);
                }
            }
            // left pack
            else if (m_sDarkfallenCreaturesLeftGuids.find(pCreature->GetObjectGuid()) != m_sDarkfallenCreaturesLeftGuids.end())
            {
                m_sDarkfallenCreaturesLeftGuids.erase(pCreature->GetObjectGuid());

                if (m_sDarkfallenCreaturesLeftGuids.empty())
                {
                    if (GameObject* pOrb = GetClosestGameObjectWithEntry(pCreature, GO_EMPOWERING_BLOOD_ORB, 30.0f))
                        DoToggleGameObjectFlags(pOrb->GetObjectGuid(), GO_FLAG_NO_INTERACT, false);
                }
            }
            // right pack
            else if (m_sDarkfallenCreaturesRightGuids.find(pCreature->GetObjectGuid()) != m_sDarkfallenCreaturesRightGuids.end())
            {
                m_sDarkfallenCreaturesRightGuids.erase(pCreature->GetObjectGuid());

                if (m_sDarkfallenCreaturesRightGuids.empty())
                {
                    if (GameObject* pOrb = GetClosestGameObjectWithEntry(pCreature, GO_EMPOWERING_BLOOD_ORB, 30.0f))
                        DoToggleGameObjectFlags(pOrb->GetObjectGuid(), GO_FLAG_NO_INTERACT, false);
                }
            }
            break;
        case NPC_SPIRE_FROSTWYRM:
            // The faction-filtered permanent wyrm on the opposite ramp is a
            // separate trash spawn. Only the area-triggered arrival summon
            // completes this saved event.
            if (pCreature->IsTemporarySummon())
                SetData(TYPE_SPIRE_FROSTWYRM, DONE);
            break;
        case NPC_SISTER_SVALNA:
            SetData(TYPE_FROST_WING_ENTRANCE, DONE);
            break;
        case NPC_FROSTWING_WHELP:
            if (m_sRimefangTrashGuids.erase(pCreature->GetObjectGuid()) && m_sRimefangTrashGuids.empty())
                StartSindragosaFrostwyrm(NPC_RIMEFANG);
            else if (m_sSpinestalkerTrashGuids.erase(pCreature->GetObjectGuid()) && m_sSpinestalkerTrashGuids.empty())
                StartSindragosaFrostwyrm(NPC_SPINESTALKER);
            break;
        case NPC_SKYBREAKER:
            SetData(TYPE_GUNSHIP_BATTLE, m_uiTeam == HORDE ? DONE : FAIL);
            break;
        case NPC_ORGRIMS_HAMMER:
            SetData(TYPE_GUNSHIP_BATTLE, m_uiTeam == ALLIANCE ? DONE : FAIL);
            break;
    }
}

void instance_icecrown_citadel::SetData(uint32 uiType, uint32 uiData)
{
    if (uiType == DATA_COLDFLAME_JETS)
    {
        m_uiColdflameJetsState = uiData;

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;
            std::ostringstream saveStream;
            for (uint32 i = 0; i < MAX_ENCOUNTER; ++i)
                saveStream << m_auiEncounter[i] << " ";
            saveStream << m_uiColdflameJetsState << " " << m_uiSindragosaGauntletState;
            m_strInstData = saveStream.str();
            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
        return;
    }

    if (uiType == DATA_SINDRAGOSA_GAUNTLET)
    {
        m_uiSindragosaGauntletState = uiData;

        // Keep the passage to Sindragosa closed until every gauntlet wave is
        // complete.  The similarly named shortcut objects connect different
        // floors and belong to Sindragosa's completed state, not this event.
        DoUseOpenableObject(GO_SINDRAGOSA_ENTRANCE, uiData == DONE);

        if (uiData == DONE)
        {
            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                       << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                       << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
                       << m_auiEncounter[9] << " " << m_auiEncounter[10] << " " << m_auiEncounter[11] << " "
                       << m_auiEncounter[12] << " " << m_auiEncounter[13] << " " << m_auiEncounter[14] << " "
                       << m_auiEncounter[15] << " " << m_uiColdflameJetsState << " " << m_uiSindragosaGauntletState;
            m_strInstData = saveStream.str();
            SaveToDB();
        }
        return;
    }

    switch (uiType)
    {
        case TYPE_MARROWGAR:
            m_auiEncounter[uiType] = uiData;
            DoUseOpenableObject(GO_MARROWGAR_DOOR, uiData != IN_PROGRESS);
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_ICEWALL_1);
                DoUseDoorOrButton(GO_ICEWALL_2);
                DoUseDoorOrButton(GO_ORATORY_DOOR);

                // enable teleporters
                DoToggleGameObjectFlags(GO_TRANSPORTER_LIGHTS_HAMMER, GO_FLAG_NO_INTERACT, false);
                DoToggleGameObjectFlags(GO_TRANSPORTER_ORATORY_DAMNED, GO_FLAG_NO_INTERACT, false);
                if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_ORATORY_DAMNED))
                    pTransporter->SetGoState(GO_STATE_ACTIVE);
                if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_LIGHTS_HAMMER))
                    pTransporter->SetGoState(GO_STATE_ACTIVE);
            }
            else if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_BONED, true);
            break;
        case TYPE_LADY_DEATHWHISPER:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_ORATORY_DOOR);
            if (uiData == DONE)
            {
                if (GameObject* pElevator = GetSingleGameObjectFromStorage(GO_DEATHWHISPER_ELEVATOR))
                    pElevator->SetGoState(GO_STATE_READY);

                // enable teleporter
                DoToggleGameObjectFlags(GO_TRANSPORTER_RAMPART_SKULLS, GO_FLAG_NO_INTERACT, false);
                if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_RAMPART_SKULLS))
                    pTransporter->SetGoState(GO_STATE_ACTIVE);

                // Check for achievement
                if (m_lDeathwhisperCultistsGuids.size() < 5)
                    break;

                // check if the entries of the remaining cultists is greater than 5
                std::set<uint32> lCultistsEntries;

                for (const auto& guid : m_lDeathwhisperCultistsGuids)
                {
                    if (Creature* pTemp = instance->GetCreature(guid))
                        lCultistsEntries.insert(pTemp->GetEntry());
                }

                // The set automatically excludes duplicates
                if (lCultistsEntries.size() >= 5)
                {
                    if (Creature* pDeathwhisper = GetSingleCreatureFromStorage(NPC_LADY_DEATHWHISPER))
                        pDeathwhisper->CastSpell(pDeathwhisper, SPELL_FULL_HOUSE_ACHIEV_CHECK, TRIGGERED_OLD_TRIGGERED);
                }
            }
            else if (uiData == IN_PROGRESS)
                m_lDeathwhisperCultistsGuids.clear();
            break;
        case TYPE_GUNSHIP_BATTLE:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                // Release controlled cannon riders before moving them away
                // from the transport.  Teleporting during the same update
                // can race the vehicle exit packet and leave the client
                // bound to a destroyed cannon, so the final relocation is
                // deferred to Update().
                for (auto& playerRef : instance->GetPlayers())
                    if (Player* player = playerRef.getSource())
                        if (player->IsBoarded())
                            player->ExitVehicle();
                m_uiGunshipVictoryTeleportTimer = 1500;

                // Spawn and enable the difficulty-specific armory. All four
                // variants are stored under their faction's base entry.
                uint32 armoryEntry = m_uiTeam == ALLIANCE ? GO_GUNSHIP_ARMORY_A : GO_GUNSHIP_ARMORY_H;
                DoRespawnGameObject(armoryEntry, 60 * MINUTE);
                DoToggleGameObjectFlags(armoryEntry, GO_FLAG_NO_INTERACT, false);

                // enable teleporter
                DoToggleGameObjectFlags(GO_TRANSPORTER_DEATHBRINGER, GO_FLAG_NO_INTERACT, false);
                if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_DEATHBRINGER))
                    pTransporter->SetGoState(GO_STATE_ACTIVE);

                // remove frames and cast spells
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_SKYBREAKER))
                {
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pShip->GetObjectGuid());

                    // cast spells on opposite team
                    if (m_uiTeam == HORDE)
                    {
                        pShip->CastSpell(pShip, SPELL_AWARD_REPUTATION, TRIGGERED_OLD_TRIGGERED);
                        pShip->CastSpell(pShip, SPELL_GUNSHIP_ACHIEVEMENT, TRIGGERED_OLD_TRIGGERED);
                    }
                }
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_ORGRIMS_HAMMER))
                {
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pShip->GetObjectGuid());

                    // cast spells on opposite team
                    if (m_uiTeam == ALLIANCE)
                    {
                        pShip->CastSpell(pShip, SPELL_AWARD_REPUTATION, TRIGGERED_OLD_TRIGGERED);
                        pShip->CastSpell(pShip, SPELL_GUNSHIP_ACHIEVEMENT, TRIGGERED_OLD_TRIGGERED);
                    }
                }

                // stop music
                if (Creature* pSource = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_GUNSHIP_MURADIN : NPC_GUNSHIP_SAURFANG))
                {
                    DoScriptText(m_uiTeam == ALLIANCE ? SAY_GUNSHIP_ALLY_WIN : SAY_GUNSHIP_HORDE_WIN, pSource);
                    pSource->PlayMusic(0);
                }

                if (Creature* pEnemyCaptain = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_GUNSHIP_SAURFANG : NPC_GUNSHIP_MURADIN))
                    pEnemyCaptain->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, pEnemyCaptain, pEnemyCaptain);

                // move the actual gunships to next position
                StartGunshipTransport(instance, m_uiTeam == ALLIANCE ? GO_ORGRIMS_HAMMER_A : GO_ORGRIMS_HAMMER_H);
                StartGunshipTransport(instance, m_uiTeam == ALLIANCE ? GO_THE_SKYBREAKER_A : GO_THE_SKYBREAKER_H);

                // The faction leader and Saurfang-event NPCs must be created
                // after the delayed victory relocation below.  Creating them
                // while the player is still on the departing transport can
                // leave the continuation scene missing at Deathbringer's Rise.
            }
            else if (uiData == SPECIAL)
            {
                // move the ships in combat position
                if (m_uiTeam == ALLIANCE)
                {
                    StartGunshipTransport(instance, GO_THE_SKYBREAKER_A);

                    StartNextDialogueText(SAY_GUNSHIP_START_ALLY_1);
                }
                else if (m_uiTeam == HORDE)
                {
                    StartGunshipTransport(instance, GO_ORGRIMS_HAMMER_H);

                    StartNextDialogueText(SAY_GUNSHIP_START_HORDE_1);
                }
            }
            else if (uiData == IN_PROGRESS)
            {
                // start encounters
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_SKYBREAKER))
                {
                    pShip->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pShip, pShip);
                    SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, pShip->GetObjectGuid());

                    pShip->SetHealth(pShip->GetMaxHealth());

                    // check for players during Alliance encounter
                    if (m_uiTeam == ALLIANCE)
                        pShip->CastSpell(pShip, SPELL_CHECK_FOR_PLAYERS, TRIGGERED_OLD_TRIGGERED);
                }
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_ORGRIMS_HAMMER))
                {
                    pShip->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pShip, pShip);
                    SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, pShip->GetObjectGuid());

                    pShip->SetHealth(pShip->GetMaxHealth());

                    // check for players during Horde encounter
                    if (m_uiTeam == HORDE)
                        pShip->CastSpell(pShip, SPELL_CHECK_FOR_PLAYERS, TRIGGERED_OLD_TRIGGERED);
                }

                // play music
                if (Creature* pSource = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_GUNSHIP_MURADIN : NPC_GUNSHIP_SAURFANG))
                    pSource->PlayMusic(MUSIC_ID_GUNSHIP);

                // The enemy captain owns the ranged crews, freeze mage and
                // timed boarding waves for either faction.
                if (Creature* pEnemyCaptain = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_GUNSHIP_SAURFANG : NPC_GUNSHIP_MURADIN))
                    pEnemyCaptain->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, pEnemyCaptain, pEnemyCaptain);
            }
            else if (uiData == FAIL)
            {
                // remove frames
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_SKYBREAKER))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pShip->GetObjectGuid());
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_ORGRIMS_HAMMER))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pShip->GetObjectGuid());

                // stop music
                if (Creature* pSource = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_GUNSHIP_MURADIN : NPC_GUNSHIP_SAURFANG))
                {
                    pSource->PlayMusic(0);
                    pSource->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }

                // Return survivors to the dock and make both ship-health
                // units ready for a clean repeat pull.
                if (Creature* pPlayerShip = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_SKYBREAKER : NPC_ORGRIMS_HAMMER))
                    pPlayerShip->CastSpell(pPlayerShip,
                        m_uiTeam == ALLIANCE ? SPELL_TELEPORT_PLAYERS_RESET_A : SPELL_TELEPORT_PLAYERS_RESET_H,
                        TRIGGERED_OLD_TRIGGERED);
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_SKYBREAKER))
                    pShip->SetHealth(pShip->GetMaxHealth());
                if (Creature* pShip = GetSingleCreatureFromStorage(NPC_ORGRIMS_HAMMER))
                    pShip->SetHealth(pShip->GetMaxHealth());

                if (Creature* pEnemyCaptain = GetSingleCreatureFromStorage(m_uiTeam == ALLIANCE ? NPC_GUNSHIP_SAURFANG : NPC_GUNSHIP_MURADIN))
                    pEnemyCaptain->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, pEnemyCaptain, pEnemyCaptain);

                // Give reset teleports time to land, then remove and recreate
                // the opposing transport so its static passengers and route
                // begin from a pristine state on the next pull.
                m_uiGunshipResetTimer = 8000;
                m_bGunshipReloadPending = false;
            }
            break;
        case TYPE_DEATHBRINGER_SAURFANG:
            m_auiEncounter[uiType] = uiData;
            // The Upper Spire door is the encounter boundary.  It is opened
            // for the intro, locked while Saurfang is in combat, and reopened
            // on a wipe or completed kill.
            DoUseOpenableObject(GO_SAURFANG_DOOR, uiData != IN_PROGRESS);
            if (uiData == DONE)
            {
                // Persistently unlock the route into the Upper Spire.  Avoid a
                // toggle here because the faction outro may already have used
                // the door before the encounter state is saved.
                DoUseOpenableObject(GO_SAURFANG_DOOR, true);
                DoToggleGameObjectFlags(GO_TRANSPORTER_UPPER_SPIRE, GO_FLAG_NO_INTERACT, false);
                if (GameObject* transporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_UPPER_SPIRE))
                    transporter->SetGoState(GO_STATE_ACTIVE);

                DoRespawnGameObject(GO_SAURFANG_CACHE, 60 * MINUTE);
                DoToggleGameObjectFlags(GO_SAURFANG_CACHE, GO_FLAG_NO_INTERACT, false);

                // spawn the Saurfang's ship for alliance only
                if (m_uiTeam == ALLIANCE)
                {
                    if (TransportTemplate* const zeppelinHorde = sTransportMgr.GetTransportTemplate(GO_ZEPPELIN_HORDE))
                        Transport::LoadTransport(*zeppelinHorde, instance, true);
                }
            }
            else if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_MADE_A_MESS, true);
            break;
        case TYPE_FESTERGUT:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_ORANGE_PLAGUE);
            if (uiData == DONE)
                DoToggleGameObjectFlags(GO_ORANGE_VALVE, GO_FLAG_NO_INTERACT, false);
            break;
        case TYPE_ROTFACE:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_GREEN_PLAGUE);
            if (uiData == DONE)
                DoToggleGameObjectFlags(GO_GREEN_VALVE, GO_FLAG_NO_INTERACT, false);
            else if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_DANCES_OOZES, true);
            break;
        case TYPE_PROFESSOR_PUTRICIDE:
            m_auiEncounter[uiType] = uiData;
            // This door is opened by the completed trap gauntlet and becomes
            // Putricide's combat boundary afterwards.  Assign its desired
            // state instead of toggling it, because repeated state delivery
            // otherwise closes an already-open progression door.
            DoUseOpenableObject(GO_SCIENTIST_DOOR,
                uiData != IN_PROGRESS && m_auiEncounter[TYPE_PLAGUE_WING_ENTRANCE] == DONE);
            if (uiData == DONE)
            {
                // deactivate the sigil and enable the teleporter if possible
                DoUseDoorOrButton(GO_PLAGUE_SIGIL);
                if (GetData(TYPE_QUEEN_LANATHEL) == DONE && GetData(TYPE_SINDRAGOSA) == DONE)
                {
                    if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_FROZEN_THRONE))
                        pTransporter->SetGoState(GO_STATE_ACTIVE);
                }
            }
            else if (uiData == IN_PROGRESS)
            {
                DoToggleGameObjectFlags(GO_DRINK_ME, GO_FLAG_NO_INTERACT, false);
                SetSpecialAchievementCriteria(TYPE_ACHIEV_NAUSEA, true);
            }
            else
                DoToggleGameObjectFlags(GO_DRINK_ME, GO_FLAG_NO_INTERACT, true);
            break;
        case TYPE_BLOOD_PRINCE_COUNCIL:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_CRIMSON_HALL_DOOR);
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_COUNCIL_DOOR_1);
                DoUseDoorOrButton(GO_COUNCIL_DOOR_2);
            }
            if (uiData == DONE || uiData == FAIL)
            {
                // remove encounter frames
                if (Creature* pPrince = GetSingleCreatureFromStorage(NPC_VALANAR))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pPrince->GetObjectGuid());
                if (Creature* pPrince = GetSingleCreatureFromStorage(NPC_KELESETH))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pPrince->GetObjectGuid());
                if (Creature* pPrince = GetSingleCreatureFromStorage(NPC_TALDARAM))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pPrince->GetObjectGuid());
            }
            else if (uiData == IN_PROGRESS)
            {
                // add encounter frames
                if (Creature* pPrince = GetSingleCreatureFromStorage(NPC_VALANAR))
                    SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, pPrince->GetObjectGuid());
                if (Creature* pPrince = GetSingleCreatureFromStorage(NPC_KELESETH))
                    SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, pPrince->GetObjectGuid());
                if (Creature* pPrince = GetSingleCreatureFromStorage(NPC_TALDARAM))
                    SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, pPrince->GetObjectGuid());
            }
            break;
        case TYPE_QUEEN_LANATHEL:
            m_auiEncounter[uiType] = uiData;
            DoUseDoorOrButton(GO_BLOODPRINCE_DOOR);
            if (uiData == DONE)
            {
                // ToDo: research if this is right
                DoUseDoorOrButton(GO_ICECROWN_GRATE);

                // deactivate the sigil and enable the teleporter if possible
                DoUseDoorOrButton(GO_BLOODWING_SIGIL);
                if (GetData(TYPE_PROFESSOR_PUTRICIDE) == DONE && GetData(TYPE_SINDRAGOSA) == DONE)
                {
                    if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_FROZEN_THRONE))
                        pTransporter->SetGoState(GO_STATE_ACTIVE);
                }
            }
            break;
        case TYPE_VALITHRIA:
            m_auiEncounter[uiType] = uiData;

            if (uiData == FAIL || uiData == NOT_STARTED)
                RespawnValithriaStarterPack();

            DoUseDoorOrButton(GO_GREEN_DRAGON_ENTRANCE);
            // Side doors
            DoUseDoorOrButton(GO_VALITHRIA_DOOR_1);
            DoUseDoorOrButton(GO_VALITHRIA_DOOR_2);
            // Some doors are used only in 25 man mode
            if (Is25ManDifficulty())
            {
                DoUseDoorOrButton(GO_VALITHRIA_DOOR_3);
                DoUseDoorOrButton(GO_VALITHRIA_DOOR_4);
            }
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_GREEN_DRAGON_EXIT);
                DoRespawnGameObject(GO_DREAMWALKER_CACHE, 60 * MINUTE);
                if (GameObject* cache = GetSingleGameObjectFromStorage(GO_DREAMWALKER_CACHE))
                {
                    cache->RemoveFlag(GAMEOBJECT_FLAGS,
                        GO_FLAG_LOCKED | GO_FLAG_INTERACT_COND | GO_FLAG_NO_INTERACT);
                    cache->SetLootState(GO_READY);
                    cache->SetGoState(GO_STATE_READY);
                }
            }
            if (uiData == DONE || uiData == FAIL)
            {
                // remove encounter frames
                if (Creature* pDragon = GetSingleCreatureFromStorage(NPC_VALITHRIA))
                    SendEncounterFrame(ENCOUNTER_FRAME_DISENGAGE, pDragon->GetObjectGuid());
            }
            else if (uiData == IN_PROGRESS)
            {
                // add encounter frames
                if (Creature* pDragon = GetSingleCreatureFromStorage(NPC_VALITHRIA))
                    SendEncounterFrame(ENCOUNTER_FRAME_ENGAGE, pDragon->GetObjectGuid());
            }
            break;
        case TYPE_SINDRAGOSA:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                // deactivate the sigil and enable the teleporter if possible
                DoUseDoorOrButton(GO_FROSTWING_SIGIL);
                if (GetData(TYPE_QUEEN_LANATHEL) == DONE && GetData(TYPE_PROFESSOR_PUTRICIDE) == DONE)
                {
                    if (GameObject* pTransporter = GetSingleGameObjectFromStorage(GO_TRANSPORTER_FROZEN_THRONE))
                        pTransporter->SetGoState(GO_STATE_ACTIVE);
                }
            }
            break;
        case TYPE_LICH_KING:
            m_auiEncounter[uiType] = uiData;
            if (uiData == FAIL || uiData == NOT_STARTED)
            {
                SetLichKingPlatformDamaged(false);
                if (Creature* tirion = GetSingleCreatureFromStorage(NPC_TIRION_FORDRING))
                    tirion->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
            else if (uiData == DONE)
            {
                if (Creature* tirion = GetSingleCreatureFromStorage(NPC_TIRION_FORDRING))
                    tirion->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
            break;
        case TYPE_BLOOD_WING_ENTRANCE:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoUseDoorOrButton(GO_CRIMSON_HALL_DOOR);
            break;
        case TYPE_FROST_WING_ENTRANCE:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoUseOpenableObject(GO_GREEN_DRAGON_ENTRANCE, true);
            break;
        case TYPE_PLAGUE_WING_ENTRANCE:
            m_auiEncounter[uiType] = uiData;
            // The lower collision gate seals only for the trap event. Use an
            // explicit state so IN_PROGRESS -> DONE and reloads are stable.
            DoUseOpenableObject(GO_SCIENTIST_DOOR_COLLISION, uiData != IN_PROGRESS);
            if (uiData == DONE)
                DoUseOpenableObject(GO_SCIENTIST_DOOR, true);
            // combat doors with custom anim
            else if (uiData == IN_PROGRESS)
            {
                DoUseDoorOrButton(GO_SCIENTIST_DOOR_GREEN);
                DoUseDoorOrButton(GO_SCIENTIST_DOOR_ORANGE);
            }
            if (uiData == FAIL || uiData == DONE)
            {
                if (GameObject* pDoor = GetSingleGameObjectFromStorage(GO_SCIENTIST_DOOR_GREEN))
                    pDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                if (GameObject* pDoor = GetSingleGameObjectFromStorage(GO_SCIENTIST_DOOR_ORANGE))
                    pDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            }
            break;
        case TYPE_SPIRE_FROSTWYRM:
            m_auiEncounter[uiType] = uiData;
            break;
        default:
            script_error_log("Instance Icecrown Citadel: ERROR SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
            return;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;

        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                   << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
                   << m_auiEncounter[9] << " " << m_auiEncounter[10] << " " << m_auiEncounter[11] << " "
                   << m_auiEncounter[12] << " " << m_auiEncounter[13] << " " << m_auiEncounter[14] << " "
                   << m_auiEncounter[15] << " " << m_uiColdflameJetsState << " " << m_uiSindragosaGauntletState;

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }

    if (uiData == FAIL || uiData == DONE) // this would need to be done in all dynamic difficulty instances but wotlk only really needs it in icc
    {
        instance->SetNewDifficultyCooldown(instance->GetCurrentClockTime() + std::chrono::milliseconds(60000));
    }
}

void instance_icecrown_citadel::JustDidDialogueStep(int32 iEntry)
{
    switch (iEntry)
    {
        case SAY_GUNSHIP_START_ALLY_5:
            StartGunshipTransport(instance, GO_ORGRIMS_HAMMER_A);
            break;
        case SAY_GUNSHIP_START_HORDE_4:
            StartGunshipTransport(instance, GO_THE_SKYBREAKER_H);
            break;
        // Retail's explicit fire orders are the encounter boundary.  Keeping
        // the approach in SPECIAL prevents cannon boarding and heat building
        // while the opposing transport is still moving into combat position.
        case SAY_GUNSHIP_START_ALLY_8:
        case SAY_GUNSHIP_START_HORDE_7:
            SetData(TYPE_GUNSHIP_BATTLE, IN_PROGRESS);
            break;
        case SAY_GUNSHIP_START_ALLY_3:
        {
            if (!GetGunshipTransport(instance, GO_ORGRIMS_HAMMER_A))
            {
                if (TransportTemplate* enemyGunship = sTransportMgr.GetTransportTemplate(GO_ORGRIMS_HAMMER_A))
                    Transport::LoadTransport(*enemyGunship, instance, true);
                else
                    script_error_log("instance_icecrown_citadel: missing transport template %u", GO_ORGRIMS_HAMMER_A);
            }
            if (Transport* gunship = GetGunshipTransport(instance, GO_ORGRIMS_HAMMER_A))
                gunship->GetVisibilityData().SetVisibilityDistanceOverride(VisibilityDistanceType::Infinite);
            break;
        }
        case SAY_GUNSHIP_START_HORDE_3:
        {
            if (!GetGunshipTransport(instance, GO_THE_SKYBREAKER_H))
            {
                if (TransportTemplate* enemyGunship = sTransportMgr.GetTransportTemplate(GO_THE_SKYBREAKER_H))
                    Transport::LoadTransport(*enemyGunship, instance, true);
                else
                    script_error_log("instance_icecrown_citadel: missing transport template %u", GO_THE_SKYBREAKER_H);
            }
            if (Transport* gunship = GetGunshipTransport(instance, GO_THE_SKYBREAKER_H))
                gunship->GetVisibilityData().SetVisibilityDistanceOverride(VisibilityDistanceType::Infinite);
            break;
        }
    }
}

uint32 instance_icecrown_citadel::GetData(uint32 uiType) const
{
    if (uiType == DATA_COLDFLAME_JETS)
        return m_uiColdflameJetsState;
    if (uiType == DATA_SINDRAGOSA_GAUNTLET)
        return m_uiSindragosaGauntletState;

    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_icecrown_citadel::SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet)
{
    if (uiType < MAX_SPECIAL_ACHIEV_CRITS)
        m_abAchievCriteria[uiType] = bIsMet;
}

bool instance_icecrown_citadel::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscvalue1*/) const
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRIT_BONED_10N:
        case ACHIEV_CRIT_BONED_25N:
        case ACHIEV_CRIT_BONED_10H:
        case ACHIEV_CRIT_BONED_25H:
            return m_abAchievCriteria[TYPE_ACHIEV_BONED];
        case ACHIEV_CRIT_MADE_A_MESS_10N:
        case ACHIEV_CRIT_MADE_A_MESS_25N:
        case ACHIEV_CRIT_MADE_A_MESS_10H:
        case ACHIEV_CRIT_MADE_A_MESS_25H:
            return m_abAchievCriteria[TYPE_ACHIEV_MADE_A_MESS];
        case ACHIEV_CRIT_DANCES_WITH_OOZES_10N:
        case ACHIEV_CRIT_DANCES_WITH_OOZES_25N:
        case ACHIEV_CRIT_DANCES_WITH_OOZES_10H:
        case ACHIEV_CRIT_DANCES_WITH_OOZES_25H:
            return m_abAchievCriteria[TYPE_ACHIEV_DANCES_OOZES];
        case ACHIEV_CRIT_NAUSEA_10N:
        case ACHIEV_CRIT_NAUSEA_25N:
        case ACHIEV_CRIT_NAUSEA_10H:
        case ACHIEV_CRIT_NAUSEA_25H:
            return m_abAchievCriteria[TYPE_ACHIEV_NAUSEA];
        case ACHIEV_CRIT_ORB_WHISPERER_10N:
        case ACHIEV_CRIT_ORB_WHISPERER_25N:
        case ACHIEV_CRIT_ORB_WHISPERER_10H:
        case ACHIEV_CRIT_ORB_WHISPERER_25H:
            return m_abAchievCriteria[TYPE_ACHIEV_ORB_WHISPERER];
        case ACHIEV_CRIT_PORTAL_JOCKEY_10N:
        case ACHIEV_CRIT_PORTAL_JOCKEY_25N:
        case ACHIEV_CRIT_PORTAL_JOCKEY_10H:
        case ACHIEV_CRIT_PORTAL_JOCKEY_25H:
            return m_abAchievCriteria[TYPE_ACHIEV_PORTAL_JOCKEY];
        case ACHIEV_CRIT_ALL_YOU_CAN_EAT_10N:
        case ACHIEV_CRIT_ALL_YOU_CAN_EAT_25N:
        case ACHIEV_CRIT_ALL_YOU_CAN_EAT_10V:
        case ACHIEV_CRIT_ALL_YOU_CAN_EAT_25V:
            return m_abAchievCriteria[TYPE_ACHIEV_ALL_YOU_CAN_EAT];
        case ACHIEV_CRIT_FLU_SHOT_SHORTAGE_10N:
        case ACHIEV_CRIT_FLU_SHOT_SHORTAGE_25N:
        case ACHIEV_CRIT_FLU_SHOT_SHORTAGE_10H:
        case ACHIEV_CRIT_FLU_SHOT_SHORTAGE_25H:
            return m_abAchievCriteria[TYPE_ACHIEV_FLU_SHOT_SHORTAGE];
    }

    return false;
}

bool instance_icecrown_citadel::CheckConditionCriteriaMeet(Player const* source, uint32 instance_condition_id, WorldObject const* conditionSource, uint32 conditionSourceType) const
{
    switch (instance_condition_id)
    {
        case INSTANCE_CONDITION_ID_INNER_SPIRE_TELEPORT:
            return GetData(TYPE_DEATHBRINGER_SAURFANG) == DONE;
    }

    return false;
}

void instance_icecrown_citadel::Load(const char* strIn)
{
    if (!strIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(strIn);

    std::istringstream loadStream(strIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
               >> m_auiEncounter[8] >> m_auiEncounter[9] >> m_auiEncounter[10] >> m_auiEncounter[11]
               >> m_auiEncounter[12] >> m_auiEncounter[13] >> m_auiEncounter[14] >> m_auiEncounter[15];

    if (!(loadStream >> m_uiColdflameJetsState))
        m_uiColdflameJetsState = NOT_STARTED;
    else if (m_uiColdflameJetsState != DONE)
        m_uiColdflameJetsState = NOT_STARTED;

    if (!(loadStream >> m_uiSindragosaGauntletState))
        m_uiSindragosaGauntletState = NOT_STARTED;
    else if (m_uiSindragosaGauntletState != DONE)
        m_uiSindragosaGauntletState = NOT_STARTED;

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_icecrown_citadel::Update(uint32 uiDiff)
{
    DialogueUpdate(uiDiff);

    if (m_uiGunshipVictoryTeleportTimer)
    {
        if (m_uiGunshipVictoryTeleportTimer > uiDiff)
            m_uiGunshipVictoryTeleportTimer -= uiDiff;
        else
        {
            m_uiGunshipVictoryTeleportTimer = 0;
            Player* eventPlayer = nullptr;
            for (auto& playerRef : instance->GetPlayers())
                if (Player* player = playerRef.getSource())
                {
                    if (player->IsBoarded())
                        player->ExitVehicle();
                    player->DestroyItemCount(ITEM_GOBLIN_ROCKET_PACK,
                        player->GetItemCount(ITEM_GOBLIN_ROCKET_PACK), true);
                    // This is the destination of the retail victory spell.
                    // Direct relocation occurs only after the vehicle exit
                    // has been processed, preventing a stale cannon mover.
                    player->TeleportTo(instance->GetId(), -548.983f, 2211.24f, 539.29f, 0.0f);
                    if (!eventPlayer)
                        eventPlayer = player;
                }

            // Build the post-Gunship faction scene only once the player is
            // standing at its destination, not while attached to a transport.
            if (eventPlayer)
                ProcessEventNpcs(eventPlayer);
        }
    }

    if (m_uiGunshipResetTimer)
    {
        if (m_uiGunshipResetTimer <= uiDiff)
        {
            if (!m_bGunshipReloadPending)
            {
                uint32 playerTransportEntry = m_uiTeam == ALLIANCE ? GO_THE_SKYBREAKER_A : GO_ORGRIMS_HAMMER_H;
                uint32 enemyTransportEntry = m_uiTeam == ALLIANCE ? GO_ORGRIMS_HAMMER_A : GO_THE_SKYBREAKER_H;
                Transport* enemyGunship = GetGunshipTransport(instance, enemyTransportEntry);
                Transport* playerGunship = GetGunshipTransport(instance, playerTransportEntry);

                // Reset spells are data-driven and can fail if their implicit
                // target data is incomplete. Relocate any remaining player
                // passengers before removing a transport; this also guarantees
                // that a failed pull cannot leave the client floating in space.
                for (auto& playerRef : instance->GetPlayers())
                    if (Player* player = playerRef.getSource())
                        if ((playerGunship && playerGunship->HasPassenger(player)) ||
                            (enemyGunship && enemyGunship->HasPassenger(player)))
                        {
                            if (player->IsBoarded())
                                player->ExitVehicle();
                            player->TeleportTo(instance->GetId(), -17.0711f, 2211.47f, 30.0546f, 3.66333f);
                        }

                for (auto& playerRef : instance->GetPlayers())
                    if (Player* player = playerRef.getSource())
                        if ((playerGunship && playerGunship->HasPassenger(player)) ||
                            (enemyGunship && enemyGunship->HasPassenger(player)))
                        {
                            m_uiGunshipResetTimer = 1000;
                            return;
                        }

                if (enemyGunship)
                    enemyGunship->RemoveFromMap();
                if (playerGunship)
                    playerGunship->RemoveFromMap();

                m_bGunshipReloadPending = true;
                m_uiGunshipResetTimer = 1000;
            }
            else
            {
                uint32 playerTransportEntry = m_uiTeam == ALLIANCE ? GO_THE_SKYBREAKER_A : GO_ORGRIMS_HAMMER_H;
                if (!GetGunshipTransport(instance, playerTransportEntry))
                {
                    if (TransportTemplate* playerGunship = sTransportMgr.GetTransportTemplate(playerTransportEntry))
                        Transport::LoadTransport(*playerGunship, instance, true);
                    else
                        script_error_log("instance_icecrown_citadel: missing transport template %u", playerTransportEntry);
                }

                if (Transport* gunship = GetGunshipTransport(instance, playerTransportEntry))
                {
                    gunship->GetVisibilityData().SetVisibilityDistanceOverride(VisibilityDistanceType::Infinite);
                    // FAIL is useful while the reset is underway, but the
                    // freshly loaded ship and captain must expose a pristine
                    // pull to clients and scripts.
                    m_auiEncounter[TYPE_GUNSHIP_BATTLE] = NOT_STARTED;
                    m_bGunshipReloadPending = false;
                    m_uiGunshipResetTimer = 0;
                }
                else
                {
                    // Passenger deletion is deferred by the map.  If that
                    // prevented recreation this tick, retry instead of
                    // silently completing with no player ship.
                    m_uiGunshipResetTimer = 1000;
                    return;
                }
            }
        }
        else
            m_uiGunshipResetTimer -= uiDiff;
    }

    if (m_uiPutricideValveTimer)
    {
        if (m_uiPutricideValveTimer <= uiDiff)
        {
            // Open the pathway to Putricide when the timer expires
            DoUseDoorOrButton(GO_SCIENTIST_DOOR_COLLISION);
            if (GameObject* pDoor = GetSingleGameObjectFromStorage(GO_SCIENTIST_DOOR_GREEN))
                pDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            if (GameObject* pDoor = GetSingleGameObjectFromStorage(GO_SCIENTIST_DOOR_ORANGE))
                pDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);

            m_uiPutricideValveTimer = 0;
        }
        else
            m_uiPutricideValveTimer -= uiDiff;
    }
}

void instance_icecrown_citadel::ProcessEventNpcs(Player* pPlayer)
{
    if (GetData(TYPE_GUNSHIP_BATTLE) == DONE)
    {
        // The faction group exists only for Saurfang's intro/outro. On an
        // instance reload after Saurfang is complete, respawning this group
        // leaves Muradin and the marines permanently stranded on the rise.
        if (GetData(TYPE_DEATHBRINGER_SAURFANG) != DONE)
        {
            for (const auto& aEventBeginLocation : aSaurfangLocations)
            {
                pPlayer->SummonCreature(m_uiTeam == HORDE ? aEventBeginLocation.uiEntryHorde : aEventBeginLocation.uiEntryAlliance,
                    aEventBeginLocation.fSpawnX, aEventBeginLocation.fSpawnY, aEventBeginLocation.fSpawnZ, aEventBeginLocation.fSpawnO, TEMPSPAWN_DEAD_DESPAWN, 24 * HOUR * IN_MILLISECONDS, true);
            }
        }

        // Show portal gameobjects
        uint8 teamIndex = GetTeamIndexByTeamId(Team(m_uiTeam));

        for (const auto& guid : m_lFactionTeleporterGuids[teamIndex])
        {
            DoRespawnGameObject(guid, 24 * HOUR);
            DoUseDoorOrButton(guid);
        }
    }
}

void instance_icecrown_citadel::SetLichKingPlatformDamaged(bool damaged)
{
    uint32 const entries[] =
    {
        GO_FROSTY_WIND, GO_FROSTY_EDGE, GO_SNOW_EDGE,
        GO_ARTHAS_PLATFORM, GO_ARTHAS_PRECIPICE
    };

    for (uint32 entry : entries)
    {
        if (GameObject* object = GetSingleGameObjectFromStorage(entry))
        {
            if (damaged)
                object->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_NODESPAWN);
            else
                object->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED);
            object->SetGoState(damaged ? GO_STATE_ACTIVE : GO_STATE_READY);
        }
    }
}

void instance_icecrown_citadel::ShowChatCommands(ChatHandler* handler)
{
    handler->SendSysMessage("This instance supports the following commands:\n startelevator, continuegunship, lichkingfloor, spawnzeppelin");
}

void instance_icecrown_citadel::ExecuteChatCommand(ChatHandler* handler, char* args)
{
    char* result = handler->ExtractLiteralArg(&args);
    if (!result)
        return;

    std::string val = result;
    if (val == "startelevator")
    {
        if (GameObject* elevator = GetSingleGameObjectFromStorage(GO_DEATHWHISPER_ELEVATOR))
            elevator->SetGoState(GO_STATE_READY);
    }
    else if (val == "continuegunship")
    {
        StartGunshipTransport(instance, GO_THE_SKYBREAKER_A);
        StartGunshipTransport(instance, GO_ORGRIMS_HAMMER_H);
    }
    else if (val == "continueenemygunship")
    {
        StartGunshipTransport(instance, GO_THE_SKYBREAKER_H);
        StartGunshipTransport(instance, GO_ORGRIMS_HAMMER_A);
    }
    else if (val == "lichkingfloor")
    {
        if (GameObject* frosty = GetSingleGameObjectFromStorage(GO_FROSTY_WIND))
            frosty->SetGoState(GO_STATE_ACTIVE);
        if (GameObject* frosty = GetSingleGameObjectFromStorage(GO_FROSTY_EDGE))
            frosty->SetGoState(GO_STATE_ACTIVE);
        if (GameObject* frosty = GetSingleGameObjectFromStorage(GO_SNOW_EDGE))
            frosty->SetGoState(GO_STATE_ACTIVE);
        if (GameObject* frosty = GetSingleGameObjectFromStorage(GO_ARTHAS_PLATFORM))
        {
            frosty->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_NODESPAWN);
            frosty->SetGoState(GO_STATE_ACTIVE);
        }
        if (GameObject* frosty = GetSingleGameObjectFromStorage(GO_ARTHAS_PRECIPICE))
        {
            frosty->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_NODESPAWN);
            frosty->SetGoState(GO_STATE_ACTIVE);
        }
    }
    else if (val == "spawnzeppelin")
    {
        TransportTemplate* const zeppelinHorde = sTransportMgr.GetTransportTemplate(GO_ZEPPELIN_HORDE);
        Transport::LoadTransport(*zeppelinHorde, instance, true);
    }
}

InstanceData* GetInstanceData_instance_icecrown_citadel(Map* pMap)
{
    return new instance_icecrown_citadel(pMap);
}

bool AreaTrigger_at_icecrown_citadel(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    // The Upper Spire transport and frost-jet shutdown are progression
    // triggers, not RP proximity triggers.  GMs still need these to work
    // while validating the instance; only dead players are excluded.
    if (pAt->id == AT_SAURFANG_PORTAL || pAt->id == AT_SHUTDOWN_FROST_JETS ||
            pAt->id == AT_SINDRAGOSA_GAUNTLET)
    {
        if (pPlayer->IsDead())
            return false;

        if (instance_icecrown_citadel* pInstance = (instance_icecrown_citadel*)pPlayer->GetInstanceData())
            pInstance->DoHandleCitadelAreaTrigger(pAt->id, pPlayer);

        return true;
    }

    if (pAt->id == AT_MARROWGAR_INTRO || pAt->id == AT_DEATHWHISPER_INTRO ||
            pAt->id == AT_SINDRAGOSA_PLATFORM)
    {
        if (pPlayer->IsGameMaster() || pPlayer->IsDead())
            return false;

        if (instance_icecrown_citadel* pInstance = (instance_icecrown_citadel*)pPlayer->GetInstanceData())
            pInstance->DoHandleCitadelAreaTrigger(pAt->id, pPlayer);

    }

    return false;
}

bool ProcessEventId_event_gameobject_citadel_valve(uint32 /*uiEventId*/, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (bIsStart && pSource->GetTypeId() == TYPEID_PLAYER)
    {
        if (instance_icecrown_citadel* pInstance = (instance_icecrown_citadel*)((Player*)pSource)->GetInstanceData())
        {
            // Note: the Tubes and doors are activated by DB script
            if (pInstance->GetData(TYPE_FESTERGUT) == DONE && pInstance->GetData(TYPE_ROTFACE) == DONE)
                pInstance->DoPreparePutricideDoor();

            return false;
        }
    }
    return false;
}

void AddSC_instance_icecrown_citadel()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_icecrown_citadel";
    pNewScript->GetInstanceData = &GetInstanceData_instance_icecrown_citadel;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_icecrown_citadel";
    pNewScript->pAreaTrigger = &AreaTrigger_at_icecrown_citadel;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_frost_freeze_trap";
    pNewScript->GetAI = &GetAI_npc_frost_freeze_trap;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_sindragosa_gauntlet_controller";
    pNewScript->GetAI = &GetAI_npc_sindragosa_gauntlet_controller;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_gameobject_citadel_valve";
    pNewScript->pProcessEventId = &ProcessEventId_event_gameobject_citadel_valve;
    pNewScript->RegisterSelf();
}
