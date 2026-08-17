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
SDName: gunship_battle
SD%Complete: 90%
SDComment: Transport movement and intro/outro cinematics are driven by the ICC instance script.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"
#include "Entities/Transports.h"

enum
{
    // gossip options
    GOSSIP_ITEM_ID_START_HORDE      = -3631006,
    GOSSIP_ITEM_ID_START_ALLIANCE   = -3631007,

    // gossip texts
    TEXT_ID_START_HORDE             = 15219,
    TEXT_ID_START_ALLIANCE          = 15101,

    // encounter dialogue
    SAY_HORDE_BOARDERS             = -1631043,
    SAY_HORDE_MAGE                 = -1631044,
    SAY_HORDE_GUNNERS              = -1631045,
    SAY_HORDE_ROCKETEERS           = -1631046,
    SAY_ALLIANCE_BOARDERS          = -1631055,
    SAY_ALLIANCE_MAGE              = -1631056,
    SAY_ALLIANCE_GUNNERS           = -1631057,
    SAY_ALLIANCE_MORTAR            = -1631058,
    SAY_MURADIN_AGGRO              = -1631060,
    SAY_SAURFANG_AGGRO             = -1631061,

    // spells
    SPELL_FRIENDLY_BOSS_DAMAGE_MOD = 70339,
    SPELL_TELEPORT_PLAYERS_RESET_A  = 70446,
    SPELL_TELEPORT_PLAYERS_RESET_H  = 71284,
    SPELL_GUNSHIP_FALL_TELEPORT     = 67335,
    SPELL_LOCK_PLAYERS_TAP_CHEST    = 72347,            // targets creature 38569
    // SPELL_SKYBREAKER_DECK        = 70120,            // applied by creature 37519 on the Alliance ship; handled in creature_addon
    // SPELL_ORGRIMS_HAMMER_DECK    = 70121,            // applied by creature 37519 on the Horde ship; handled in creature_addon
    SPELL_HATE_TO_ZERO              = 63984,

    SPELL_MELEE_TARGETING_A         = 70219,            // cast by horde soldiers: 36957, 36960 to target hostile players
    SPELL_MELEE_TARGETING_H         = 70294,            // cast by alliance soldiers 36950 and 36961 to target hostile players

    SPELL_EXPLOSION_FAIL            = 72134,
    SPELL_EXPLOSION_VICTORY         = 72137,            // cast by creature 37547 on enemy ship

    SPELL_TELEPORT_ENEMY_SHIP       = 70104,            // cast by enemy combatants when teleporting to ship; soldiers are spawned on enemy ship and then teleport to player ship with this spell
    SPELL_BERSERK                   = 72525,

    SPELL_BATTLE_EXPERIENCE         = 71201,            // cast by enemy soldiers; related to 71188, 71193, 71195
    SPELL_EXPERIENCED               = 71188,
    SPELL_VETERAN                   = 71193,
    SPELL_ELITE                     = 71195,

    SPELL_CAPTAIN_BATTLE_FURY       = 69637,
    SPELL_CAPTAIN_CLEAVE            = 15284,
    SPELL_CAPTAIN_RENDING_THROW     = 69634,
    SPELL_SHADOW_CHANNELING         = 43897,
    SPELL_BELOW_ZERO                = 69705,
    SPELL_SHOOT                     = 70162,
    SPELL_HURL_AXE                  = 70161,
    SPELL_ROCKET_ARTILLERY_A        = 70609,
    SPELL_ROCKET_ARTILLERY_H        = 69678,
    SPELL_DESPERATE_RESOLVE         = 69647,
    SPELL_BLADESTORM                = 69652,
    SPELL_WOUNDING_STRIKE           = 69651,
    SPELL_CREATE_ROCKET_PACK        = 70055,

    // Timings match the 3.3.5 encounter: first boarders at 12 sec,
    // subsequent waves each minute, and first freeze mage no sooner
    // than one minute after combat begins.
    TIMER_FIRST_BOARDING            = 12000,
    TIMER_BOARDING_WAVE             = 60000,
    TIMER_FIRST_FREEZE_MAGE         = 60000,
    TIMER_FREEZE_MAGE_RESPAWN       = 30000,
};

struct GunshipPosition
{
    float x, y, z, o;
};

static GunshipPosition const sSkybreakerRanged[] =
{
    {-29.563900f, -17.95801f, 20.73837f, 4.747295f},
    {-18.017210f, -18.82056f, 20.79150f, 4.747295f},
    {-9.1193850f, -18.79102f, 20.58887f, 4.712389f},
    {-0.3364258f, -18.87183f, 20.56824f, 4.712389f},
    {-34.705810f, -17.67261f, 20.51523f, 4.729842f},
    {-23.562010f, -18.28564f, 20.67859f, 4.729842f},
    {-13.602780f, -18.74268f, 20.59622f, 4.712389f},
    {-4.3350220f, -18.84619f, 20.58234f, 4.712389f},
};

static GunshipPosition const sSkybreakerMortar[] =
{
    {-31.70142f, 18.02783f, 20.77197f, 4.712389f},
    {-9.368652f, 18.75806f, 20.65335f, 4.712389f},
    {-20.40851f, 18.40381f, 20.50647f, 4.694936f},
    {0.1585693f, 18.11523f, 20.41949f, 4.729842f},
};

static GunshipPosition const sSkybreakerMages[] =
{
    {-9.479858f, 0.05663967f, 20.77026f, 4.729842f},
    {6.385986f, 4.978760f, 20.55417f, 4.694936f},
    {6.579102f, -4.674561f, 20.55060f, 1.553343f},
};

static GunshipPosition const sOrgrimsRanged[] =
{
    {-12.09280f, 27.65942f, 33.58557f, 1.53589f},
    {-3.170555f, 28.30652f, 34.21082f, 1.53589f},
    {14.928040f, 26.18018f, 35.47803f, 1.53589f},
    {24.703310f, 25.36584f, 35.97845f, 1.53589f},
    {-16.65302f, 27.59668f, 33.18726f, 1.53589f},
    {-8.084572f, 28.21448f, 33.93805f, 1.53589f},
    {7.594765f, 27.41968f, 35.00775f, 1.53589f},
    {20.763390f, 25.58215f, 35.75287f, 1.53589f},
};

static GunshipPosition const sOrgrimsRocket[] =
{
    {-11.44849f, -25.71838f, 33.64343f, 1.518436f},
    {12.30336f, -25.69653f, 35.32373f, 1.518436f},
    {-0.05931854f, -25.46399f, 34.50592f, 1.518436f},
    {27.621490f, -23.48108f, 36.12708f, 1.518436f},
};

static GunshipPosition const sOrgrimsMages[] =
{
    {13.58548f, 0.3867192f, 34.99243f, 1.53589f},
    {47.29290f, -4.308941f, 37.55550f, 1.570796f},
    {47.34621f, 4.032004f, 37.70952f, 4.817109f},
};

static GunshipPosition const sSkybreakerPortal = {6.666975f, 0.013001f, 20.87888f, 0.0f};
static GunshipPosition const sSkybreakerExit   = {-17.55738f, -0.090421f, 21.18366f, 0.0f};
static GunshipPosition const sOrgrimsPortal    = {47.550990f, -0.101778f, 37.61111f, 0.0f};
static GunshipPosition const sOrgrimsExit      = {7.461699f, 0.158853f, 35.72989f, 0.0f};

static Player* SelectGunshipPlayer(Creature* source, bool sameTransport)
{
    Player* selected = nullptr;
    for (auto& playerRef : source->GetMap()->GetPlayers())
    {
        Player* player = playerRef.getSource();
        if (!player || !player->IsAlive() || player->IsGameMaster())
            continue;
        if (sameTransport)
        {
            if (player->GetTransport() != source->GetTransport())
                continue;
        }
        else if (!player->GetTransport() || player->GetTransport() == source->GetTransport())
            continue;
        if (!selected || source->GetDistance(player) < source->GetDistance(selected))
            selected = player;
    }
    return selected;
}

static uint32 CountAliveOnTransport(Creature* source, uint32 entry)
{
    CreatureList creatures;
    GetCreatureListWithEntryInGrid(creatures, source, entry, 300.0f);
    uint32 count = 0;
    for (Creature* creature : creatures)
        if (creature->IsAlive() && creature->GetTransport() == source->GetTransport())
            ++count;
    return count;
}

static void DespawnGunshipAdds(Creature* source)
{
    uint32 const entries[] =
    {
        NPC_SKYBREAKER_SORCERER, NPC_SKYBREAKER_RIFLEMAN, NPC_SKYBREAKER_MORTAR_SOLDIER,
        NPC_SKYBREAKER_MARINE, NPC_SKYBREAKER_SERGEANT, NPC_KORKRON_BATTLE_MAGE,
        NPC_KORKRON_AXETHROWER, NPC_KORKRON_ROCKETEER, NPC_KORKRON_REAVER,
        NPC_KORKRON_SERGEANT, NPC_TELEPORT_PORTAL, NPC_TELEPORT_EXIT,
    };

    for (uint32 entry : entries)
    {
        CreatureList creatures;
        GetCreatureListWithEntryInGrid(creatures, source, entry, 300.0f);
        for (Creature* creature : creatures)
            if (creature->IsTemporarySummon())
                creature->ForcedDespawn();
    }
}

struct npc_gunshipAI : public Scripted_NoMovementAI
{
    npc_gunshipAI(Creature* creature) : Scripted_NoMovementAI(creature),
        m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData())), m_ended(false), m_mageRequested(false), m_wipeTimer(5000) { }

    instance_icecrown_citadel* m_instance;
    bool m_ended;
    bool m_mageRequested;
    uint32 m_wipeTimer;

    void Reset() override
    {
        m_ended = false;
        m_mageRequested = false;
        m_wipeTimer = 5000;
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
            Reset();
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damageType*/, SpellEntry const* /*spellInfo*/) override
    {
        if (!m_instance || m_instance->GetData(TYPE_GUNSHIP_BATTLE) != IN_PROGRESS)
        {
            damage = 0;
            return;
        }

        // The first cannon breakpoint calls the enemy freeze mage. Blizzard
        // also gates this to no earlier than one minute; the captain enforces
        // that timer when it receives this request.
        bool isEnemyShip = (m_instance->GetPlayerTeam() == ALLIANCE && m_creature->GetEntry() == NPC_ORGRIMS_HAMMER) ||
            (m_instance->GetPlayerTeam() == HORDE && m_creature->GetEntry() == NPC_SKYBREAKER);
        if (isEnemyShip && !m_mageRequested && m_creature->GetHealthPercent() > 90.0f &&
                damage >= m_creature->GetHealth() - m_creature->GetMaxHealth() * 9 / 10)
        {
            m_mageRequested = true;
            if (Creature* captain = m_instance->GetSingleCreatureFromStorage(
                    m_instance->GetPlayerTeam() == ALLIANCE ? NPC_GUNSHIP_SAURFANG : NPC_GUNSHIP_MURADIN))
                captain->AI()->SendAIEvent(AI_EVENT_CUSTOM_C, m_creature, captain);
        }

        if (damage < m_creature->GetHealth() || m_ended)
            return;

        m_ended = true;
        damage = m_creature->GetHealth() - 1;
        m_creature->CastSpell(m_creature,
            isEnemyShip ? SPELL_EXPLOSION_VICTORY : SPELL_EXPLOSION_FAIL, TRIGGERED_OLD_TRIGGERED);
        m_instance->SetData(TYPE_GUNSHIP_BATTLE, isEnemyShip ? DONE : FAIL);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_instance || m_instance->GetData(TYPE_GUNSHIP_BATTLE) != IN_PROGRESS)
            return;

        bool isPlayerShip = (m_instance->GetPlayerTeam() == ALLIANCE && m_creature->GetEntry() == NPC_SKYBREAKER) ||
            (m_instance->GetPlayerTeam() == HORDE && m_creature->GetEntry() == NPC_ORGRIMS_HAMMER);
        if (!isPlayerShip)
            return;

        bool hasLivingPlayer = false;
        for (auto& playerRef : m_creature->GetMap()->GetPlayers())
            if (Player* player = playerRef.getSource())
                if (player->IsAlive() && !player->IsGameMaster())
                    for (Transport* transport : m_creature->GetMap()->GetTransports())
                    {
                        uint32 transportEntry = transport->GetEntry();
                        if ((transportEntry == GO_THE_SKYBREAKER_A || transportEntry == GO_THE_SKYBREAKER_H ||
                                transportEntry == GO_ORGRIMS_HAMMER_A || transportEntry == GO_ORGRIMS_HAMMER_H) &&
                                transport->HasPassenger(player))
                        {
                            hasLivingPlayer = true;
                            break;
                        }
                    }

        if (hasLivingPlayer)
        {
            m_wipeTimer = 5000;
            return;
        }

        if (m_wipeTimer > diff)
            m_wipeTimer -= diff;
        else
            m_instance->SetData(TYPE_GUNSHIP_BATTLE, FAIL);
    }
};

UnitAI* GetAI_npc_gunship(Creature* creature)
{
    return new npc_gunshipAI(creature);
}

struct npc_gunship_cannonAI : public ScriptedAI
{
    npc_gunship_cannonAI(Creature* creature) : ScriptedAI(creature)
    {
        SetReactState(REACT_PASSIVE);
        Reset();
    }

    void Reset() override
    {
        m_creature->SetImmobilizedState(true);
    }

    void OnPassengerRide(Unit* /*passenger*/, bool boarded, uint8 /*seat*/) override
    {
        if (!boarded)
        {
            // The client legitimately reports ROOT for these turrets, but the
            // generic movement validation can clear it while controlled.
            m_creature->SetImmobilizedState(false);
            m_creature->SetImmobilizedState(true);
        }
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

UnitAI* GetAI_npc_gunship_cannon(Creature* creature)
{
    return new npc_gunship_cannonAI(creature);
}

struct npc_gunship_captainAI : public ScriptedAI
{
    npc_gunship_captainAI(Creature* creature) : ScriptedAI(creature),
        m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData()))
    {
        Reset();
    }

    instance_icecrown_citadel* m_instance;
    uint32 m_boardingTimer;
    uint32 m_refillTimer;
    uint32 m_freezeTimer;
    uint32 m_cleaveTimer;
    uint32 m_throwTimer;
    uint32 m_attackCallTimer;
    bool m_nextArtilleryCall;
    bool m_active;
    bool m_freezeRequested;
    ObjectGuid m_channelMageGuids[2];
    ObjectGuid m_freezeMageGuid;

    bool IsEnemyCaptain() const
    {
        if (!m_instance)
            return false;
        return (m_instance->GetPlayerTeam() == ALLIANCE && m_creature->GetEntry() == NPC_GUNSHIP_SAURFANG) ||
            (m_instance->GetPlayerTeam() == HORDE && m_creature->GetEntry() == NPC_GUNSHIP_MURADIN);
    }

    void Reset() override
    {
        m_boardingTimer = TIMER_FIRST_BOARDING;
        m_refillTimer = 1000;
        m_freezeTimer = TIMER_FIRST_FREEZE_MAGE;
        m_cleaveTimer = urand(2000, 10000);
        m_throwTimer = urand(3000, 6000);
        m_attackCallTimer = urand(25000, 35000);
        m_nextArtilleryCall = false;
        m_active = false;
        m_freezeRequested = false;
        m_channelMageGuids[0].Clear();
        m_channelMageGuids[1].Clear();
        m_freezeMageGuid.Clear();
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 /*miscValue*/) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && IsEnemyCaptain())
        {
            Reset();
            m_active = true;
            m_creature->CastSpell(m_creature, SPELL_CAPTAIN_BATTLE_FURY, TRIGGERED_OLD_TRIGGERED);
            SpawnRangedCrew();
        }
        else if (eventType == AI_EVENT_CUSTOM_B)
        {
            m_active = false;
            DespawnGunshipAdds(m_creature);
            m_creature->CombatStop(true);
        }
        else if (eventType == AI_EVENT_CUSTOM_C && IsEnemyCaptain())
            m_freezeRequested = true;
        else if (eventType == AI_EVENT_CUSTOM_D && IsEnemyCaptain())
        {
            // The replacement cooldown begins when the Below Zero mage dies,
            // not when she initially appears.  Starting it at summon time
            // allowed an immediate replacement after a late kill.
            m_freezeMageGuid.Clear();
            m_freezeTimer = TIMER_FREEZE_MAGE_RESPAWN;
        }
    }

    Creature* SummonLocal(uint32 entry, GunshipPosition const& pos, uint32 despawn = 0)
    {
        return m_creature->SummonCreature(entry, pos.x, pos.y, pos.z, pos.o,
            despawn ? TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN : TEMPSPAWN_CORPSE_TIMED_DESPAWN,
            despawn ? despawn : 15000, true);
    }

    void SpawnRangedCrew()
    {
        if (!m_instance || !IsEnemyCaptain())
            return;

        bool allianceCrew = m_creature->GetEntry() == NPC_GUNSHIP_MURADIN;
        GunshipPosition const* ranged = allianceCrew ? sSkybreakerRanged : sOrgrimsRanged;
        GunshipPosition const* artillery = allianceCrew ? sSkybreakerMortar : sOrgrimsRocket;
        uint32 rangedEntry = allianceCrew ? NPC_SKYBREAKER_RIFLEMAN : NPC_KORKRON_AXETHROWER;
        uint32 artilleryEntry = allianceCrew ? NPC_SKYBREAKER_MORTAR_SOLDIER : NPC_KORKRON_ROCKETEER;
        uint32 rangedCount = m_instance->Is25ManDifficulty() ? 8 : 4;
        uint32 artilleryCount = m_instance->Is25ManDifficulty() ? 4 : 2;

        uint32 existing = CountAliveOnTransport(m_creature, rangedEntry);
        for (uint32 i = existing; i < rangedCount; ++i)
            SummonLocal(rangedEntry, ranged[i]);

        existing = CountAliveOnTransport(m_creature, artilleryEntry);
        for (uint32 i = existing; i < artilleryCount; ++i)
            SummonLocal(artilleryEntry, artillery[i]);

        GunshipPosition const* magePositions = allianceCrew ? sSkybreakerMages : sOrgrimsMages;
        uint32 mageEntry = allianceCrew ? NPC_SKYBREAKER_SORCERER : NPC_KORKRON_BATTLE_MAGE;
        for (uint32 i = 0; i < 2; ++i)
        {
            Creature* mage = m_channelMageGuids[i] ? m_creature->GetMap()->GetCreature(m_channelMageGuids[i]) : nullptr;
            if (mage && mage->IsAlive())
                continue;
            if ((mage = SummonLocal(mageEntry, magePositions[i + 1])))
            {
                m_channelMageGuids[i] = mage->GetObjectGuid();
                mage->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, mage, 0);
            }
        }
    }

    void SpawnFreezeMage()
    {
        bool allianceCrew = m_creature->GetEntry() == NPC_GUNSHIP_MURADIN;
        uint32 entry = allianceCrew ? NPC_SKYBREAKER_SORCERER : NPC_KORKRON_BATTLE_MAGE;
        Creature* current = m_freezeMageGuid ? m_creature->GetMap()->GetCreature(m_freezeMageGuid) : nullptr;
        if (current && current->IsAlive())
            return;

        GunshipPosition const* positions = allianceCrew ? sSkybreakerMages : sOrgrimsMages;
        if (Creature* mage = SummonLocal(entry, positions[0]))
        {
            m_freezeMageGuid = mage->GetObjectGuid();
            mage->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, mage, 1);
        }
        DoScriptText(allianceCrew ? SAY_ALLIANCE_MAGE : SAY_HORDE_MAGE, m_creature);
    }

    void SpawnBoardingWave()
    {
        if (!m_instance)
            return;

        bool allianceCrew = m_creature->GetEntry() == NPC_GUNSHIP_MURADIN;
        Creature* playerCaptain = m_instance->GetSingleCreatureFromStorage(
            allianceCrew ? NPC_GUNSHIP_SAURFANG : NPC_GUNSHIP_MURADIN);
        if (!playerCaptain || !playerCaptain->GetTransport())
            return;

        GunshipPosition const& portalPos = allianceCrew ? sSkybreakerPortal : sOrgrimsPortal;
        GunshipPosition const& exitPos = allianceCrew ? sOrgrimsExit : sSkybreakerExit;
        SummonLocal(NPC_TELEPORT_PORTAL, portalPos, 21000);
        playerCaptain->SummonCreature(NPC_TELEPORT_EXIT, exitPos.x, exitPos.y, exitPos.z, exitPos.o,
            TEMPSPAWN_TIMED_DESPAWN, 23000, true);

        uint32 marineEntry = allianceCrew ? NPC_SKYBREAKER_MARINE : NPC_KORKRON_REAVER;
        uint32 leaderEntry = allianceCrew ? NPC_SKYBREAKER_SERGEANT : NPC_KORKRON_SERGEANT;
        uint32 marineCount = m_instance->Is25ManDifficulty() ? 4 : 2;
        uint32 leaderCount = m_instance->Is25ManDifficulty() ? 2 : 1;

        for (uint32 i = 0; i < marineCount; ++i)
        {
            GunshipPosition pos = exitPos;
            pos.x += float(i % 2) * 2.0f - 1.0f;
            pos.y += float(i / 2) * 2.0f - 1.0f;
            if (Creature* add = playerCaptain->SummonCreature(marineEntry, pos.x, pos.y, pos.z, pos.o,
                    TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 70000, true))
                add->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, add);
        }

        for (uint32 i = 0; i < leaderCount; ++i)
        {
            GunshipPosition pos = exitPos;
            pos.x += i ? 3.0f : -3.0f;
            if (Creature* add = playerCaptain->SummonCreature(leaderEntry, pos.x, pos.y, pos.z, pos.o,
                    TEMPSPAWN_TIMED_OOC_OR_DEAD_DESPAWN, 70000, true))
                add->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, add);
        }

        DoScriptText(allianceCrew ? SAY_ALLIANCE_BOARDERS : SAY_HORDE_BOARDERS, m_creature);
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damageType*/, SpellEntry const* /*spellInfo*/) override
    {
        if (damage >= m_creature->GetHealth())
            damage = m_creature->GetHealth() - 1;
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(m_creature->GetEntry() == NPC_GUNSHIP_MURADIN ? SAY_MURADIN_AGGRO : SAY_SAURFANG_AGGRO, m_creature);
        m_creature->CastSpell(m_creature, SPELL_CAPTAIN_BATTLE_FURY, TRIGGERED_OLD_TRIGGERED);
    }

    void UpdateAI(uint32 diff) override
    {
        if (m_active && m_instance && m_instance->GetData(TYPE_GUNSHIP_BATTLE) == IN_PROGRESS)
        {
            if (m_boardingTimer <= diff)
            {
                SpawnBoardingWave();
                m_boardingTimer = TIMER_BOARDING_WAVE;
            }
            else
                m_boardingTimer -= diff;

            if (m_refillTimer <= diff)
            {
                SpawnRangedCrew();
                m_refillTimer = 5000;
            }
            else
                m_refillTimer -= diff;

            if (m_freezeTimer > diff)
                m_freezeTimer -= diff;
            else
            {
                m_freezeTimer = 0;
                if (m_freezeRequested)
                    SpawnFreezeMage();
            }

            if (m_attackCallTimer <= diff)
            {
                bool allianceCrew = m_creature->GetEntry() == NPC_GUNSHIP_MURADIN;
                DoScriptText(allianceCrew ?
                    (m_nextArtilleryCall ? SAY_ALLIANCE_MORTAR : SAY_ALLIANCE_GUNNERS) :
                    (m_nextArtilleryCall ? SAY_HORDE_ROCKETEERS : SAY_HORDE_GUNNERS), m_creature);
                m_nextArtilleryCall = !m_nextArtilleryCall;
                m_attackCallTimer = urand(30000, 45000);
            }
            else
                m_attackCallTimer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_cleaveTimer <= diff)
        {
            DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CAPTAIN_CLEAVE);
            m_cleaveTimer = urand(2000, 10000);
        }
        else
            m_cleaveTimer -= diff;

        if (!m_creature->CanReachWithMeleeAttack(m_creature->GetVictim()))
        {
            if (m_throwTimer <= diff)
            {
                DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CAPTAIN_RENDING_THROW);
                m_throwTimer = urand(3000, 6000);
            }
            else
                m_throwTimer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_gunship_captain(Creature* creature)
{
    return new npc_gunship_captainAI(creature);
}

struct npc_gunship_soldierAI : public ScriptedAI
{
    npc_gunship_soldierAI(Creature* creature) : ScriptedAI(creature),
        m_instance(static_cast<instance_icecrown_citadel*>(creature->GetInstanceData())),
        m_shotTimer(urand(2000, 4000)), m_artilleryTimer(urand(4000, 7000)),
        m_woundTimer(urand(8000, 10000)), m_bladeTimer(urand(13000, 18000)),
        m_experienceTimer(100000), m_experienceLevel(0), m_boarded(false), m_freezeMage(false)
    {
        uint32 entry = creature->GetEntry();
        if (entry == NPC_SKYBREAKER_RIFLEMAN || entry == NPC_KORKRON_AXETHROWER ||
                entry == NPC_SKYBREAKER_MORTAR_SOLDIER || entry == NPC_KORKRON_ROCKETEER ||
                entry == NPC_SKYBREAKER_SORCERER || entry == NPC_KORKRON_BATTLE_MAGE)
        {
            SetCombatMovement(false);
            SetReactState(REACT_PASSIVE);
        }
    }

    instance_icecrown_citadel* m_instance;
    uint32 m_shotTimer;
    uint32 m_artilleryTimer;
    uint32 m_woundTimer;
    uint32 m_bladeTimer;
    uint32 m_experienceTimer;
    uint8 m_experienceLevel;
    bool m_boarded;
    bool m_freezeMage;

    bool IsRanged() const
    {
        return m_creature->GetEntry() == NPC_SKYBREAKER_RIFLEMAN || m_creature->GetEntry() == NPC_KORKRON_AXETHROWER;
    }

    bool IsArtillery() const
    {
        return m_creature->GetEntry() == NPC_SKYBREAKER_MORTAR_SOLDIER || m_creature->GetEntry() == NPC_KORKRON_ROCKETEER;
    }

    bool IsMage() const
    {
        return m_creature->GetEntry() == NPC_SKYBREAKER_SORCERER || m_creature->GetEntry() == NPC_KORKRON_BATTLE_MAGE;
    }

    bool IsLeader() const
    {
        return m_creature->GetEntry() == NPC_SKYBREAKER_SERGEANT || m_creature->GetEntry() == NPC_KORKRON_SERGEANT;
    }

    void Reset() override { }

    void ReceiveAIEvent(AIEventType eventType, Unit* /*sender*/, Unit* /*invoker*/, uint32 miscValue) override
    {
        if (eventType != AI_EVENT_CUSTOM_A)
            return;

        if (IsMage())
        {
            m_freezeMage = miscValue != 0;
            DoCastSpellIfCan(m_creature, m_freezeMage ? SPELL_BELOW_ZERO : SPELL_SHADOW_CHANNELING, CAST_TRIGGERED);
            return;
        }

        m_boarded = true;
        DoCastSpellIfCan(m_creature, SPELL_TELEPORT_ENEMY_SHIP, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_BATTLE_EXPERIENCE, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature,
            m_instance && m_instance->GetPlayerTeam() == ALLIANCE ? SPELL_MELEE_TARGETING_A : SPELL_MELEE_TARGETING_H,
            CAST_TRIGGERED);
        if (Player* player = SelectGunshipPlayer(m_creature, true))
            AttackStart(player);
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damageType*/, SpellEntry const* /*spellInfo*/) override
    {
        if (!m_boarded || m_creature->HasAura(SPELL_DESPERATE_RESOLVE))
            return;
        if (m_creature->GetHealthPercent() > 25.0f && damage >= m_creature->GetHealth() - m_creature->GetMaxHealth() / 4)
            DoCastSpellIfCan(m_creature, SPELL_DESPERATE_RESOLVE, CAST_TRIGGERED);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!m_freezeMage || !m_instance)
            return;

        if (Creature* captain = m_instance->GetSingleCreatureFromStorage(
                m_instance->GetPlayerTeam() == ALLIANCE ? NPC_GUNSHIP_SAURFANG : NPC_GUNSHIP_MURADIN))
            captain->AI()->SendAIEvent(AI_EVENT_CUSTOM_D, m_creature, captain);
    }

    void UpdateBattleExperience(uint32 diff)
    {
        if (!m_boarded || m_experienceLevel >= (m_instance && m_instance->IsHeroicDifficulty() ? 4 : 3))
            return;
        if (m_experienceTimer > diff)
        {
            m_experienceTimer -= diff;
            return;
        }

        static uint32 const experienceSpells[] = {SPELL_EXPERIENCED, SPELL_VETERAN, SPELL_ELITE, SPELL_BERSERK};
        static uint32 const experienceTimers[] = {70000, 60000, 90000, 90000};
        if (m_experienceLevel)
            m_creature->RemoveAurasDueToSpell(experienceSpells[m_experienceLevel - 1]);
        DoCastSpellIfCan(m_creature, experienceSpells[m_experienceLevel], CAST_TRIGGERED);
        m_experienceTimer = experienceTimers[m_experienceLevel];
        ++m_experienceLevel;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_instance || m_instance->GetData(TYPE_GUNSHIP_BATTLE) != IN_PROGRESS)
            return;

        if (IsMage())
        {
            uint32 spell = m_freezeMage ? SPELL_BELOW_ZERO : SPELL_SHADOW_CHANNELING;
            if (!m_creature->HasAura(spell))
                DoCastSpellIfCan(m_creature, spell, CAST_TRIGGERED);
            return;
        }

        if (IsRanged())
        {
            if (m_shotTimer <= diff)
            {
                if (Player* target = SelectGunshipPlayer(m_creature, false))
                    DoCastSpellIfCan(target, m_creature->GetEntry() == NPC_SKYBREAKER_RIFLEMAN ? SPELL_SHOOT : SPELL_HURL_AXE);
                m_shotTimer = urand(3000, 5000);
            }
            else
                m_shotTimer -= diff;
            return;
        }

        if (IsArtillery())
        {
            if (m_artilleryTimer <= diff)
            {
                DoCastSpellIfCan(m_creature,
                    m_creature->GetEntry() == NPC_SKYBREAKER_MORTAR_SOLDIER ? SPELL_ROCKET_ARTILLERY_A : SPELL_ROCKET_ARTILLERY_H,
                    CAST_TRIGGERED);
                m_artilleryTimer = 9000;
            }
            else
                m_artilleryTimer -= diff;
            return;
        }

        UpdateBattleExperience(diff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
        {
            if (Player* player = SelectGunshipPlayer(m_creature, true))
                AttackStart(player);
            return;
        }

        if (IsLeader())
        {
            if (m_bladeTimer <= diff)
            {
                DoCastSpellIfCan(m_creature, SPELL_BLADESTORM);
                m_bladeTimer = urand(25000, 30000);
            }
            else
                m_bladeTimer -= diff;

            if (m_woundTimer <= diff)
            {
                DoCastSpellIfCan(m_creature->GetVictim(), SPELL_WOUNDING_STRIKE);
                m_woundTimer = urand(9000, 13000);
            }
            else
                m_woundTimer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_gunship_soldier(Creature* creature)
{
    return new npc_gunship_soldierAI(creature);
}

bool GossipHello_npc_zafod_boombox(Player* player, Creature* creature)
{
    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetObjectGuid());
    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I need a jet pack.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_zafod_boombox(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
        creature->CastSpell(player, SPELL_CREATE_ROCKET_PACK, TRIGGERED_OLD_TRIGGERED);
    player->CLOSE_GOSSIP_MENU();
    return true;
}

bool GossipHello_npc_saurfang_gunship(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pCreature->GetInstanceData()))
    {
        if (pInstance->GetData(TYPE_GUNSHIP_BATTLE) == NOT_STARTED || pInstance->GetData(TYPE_GUNSHIP_BATTLE) == FAIL)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_START_HORDE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_START_HORDE, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_saurfang_gunship(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pCreature->GetInstanceData()))
                pInstance->SetData(TYPE_GUNSHIP_BATTLE, SPECIAL);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            break;
    }
    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

bool GossipHello_npc_muradin_gunship(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pCreature->GetInstanceData()))
    {
        if (pInstance->GetData(TYPE_GUNSHIP_BATTLE) == NOT_STARTED || pInstance->GetData(TYPE_GUNSHIP_BATTLE) == FAIL)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ID_START_ALLIANCE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_START_ALLIANCE, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_muradin_gunship(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (instance_icecrown_citadel* pInstance = static_cast<instance_icecrown_citadel*>(pCreature->GetInstanceData()))
                pInstance->SetData(TYPE_GUNSHIP_BATTLE, SPECIAL);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            break;
    }
    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

/*######
## spell_incinerating_blast - 69402, 70175
######*/

struct spell_incinerating_blast : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        Unit* caster = spell->GetAffectiveCaster();
        Unit* target = spell->GetUnitTarget();
        if (!target || !caster)
            return;

        if (effIdx == EFFECT_INDEX_1)
        {
            uint32 damage = spell->GetDamage();
            uint32 energy = caster->GetPower(caster->GetPowerType());

            // Note: this calculation has to be verified
            spell->SetDamage(damage + energy * energy * 8);
        }
        // remove all power
        else if (effIdx == EFFECT_INDEX_2)
            caster->SetPower(caster->GetPowerType(), 0);
    }
};

void AddSC_gunship_battle()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_saurfang_gunship";
    pNewScript->GetAI = &GetAI_npc_gunship_captain;
    pNewScript->pGossipHello = &GossipHello_npc_saurfang_gunship;
    pNewScript->pGossipSelect = &GossipSelect_npc_saurfang_gunship;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_muradin_gunship";
    pNewScript->GetAI = &GetAI_npc_gunship_captain;
    pNewScript->pGossipHello = &GossipHello_npc_muradin_gunship;
    pNewScript->pGossipSelect = &GossipSelect_npc_muradin_gunship;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_gunship";
    pNewScript->GetAI = &GetAI_npc_gunship;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_gunship_cannon";
    pNewScript->GetAI = &GetAI_npc_gunship_cannon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_gunship_soldier";
    pNewScript->GetAI = &GetAI_npc_gunship_soldier;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_zafod_boombox";
    pNewScript->pGossipHello = &GossipHello_npc_zafod_boombox;
    pNewScript->pGossipSelect = &GossipSelect_npc_zafod_boombox;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_incinerating_blast>("spell_incinerating_blast");
}
