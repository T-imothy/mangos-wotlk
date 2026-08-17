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
SDName: boss_valithria
SD%Complete: 90%
SDComment: Native encounter flow, add waves, portals, dream clouds and Portal Jockey tracking.
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"

#include <cmath>

enum ValithriaTexts
{
    SAY_AGGRO                   = -1631140,
    SAY_PORTAL                  = -1631141,
    SAY_75_HEALTH               = -1631142,
    SAY_25_HEALTH               = -1631143,
    SAY_0_HEALTH                = -1631144,
    SAY_PLAYER_DIES             = -1631145,
    SAY_BERSERK                 = -1631146,
    SAY_VICTORY                 = -1631147,
};

enum ValithriaSpells
{
    // Valithria and the dream realm
    SPELL_TWISTED_NIGHTMARES        = 71941,
    SPELL_NIGHTMARE_CLOUD           = 71970,
    SPELL_NIGHTMARE_CLOUD_VISUAL    = 71939,
    SPELL_EMERALD_VIGOR             = 70873,
    SPELL_DREAM_CLOUD_VISUAL        = 70876,
    SPELL_DREAM_STATE               = 70766,
    SPELL_DREAMWALKER_RAGE          = 71189,
    SPELL_IMMUNITY                  = 72724,
    SPELL_CORRUPTION                = 70904,
    SPELL_DREAM_SLIP                = 71196,

    // Risen Archmage
    SPELL_ARCHMAGE_CORRUPTION       = 70602,
    SPELL_FROSTBOLT_VOLLEY          = 70759,
    SPELL_MANA_VOID                 = 71179,
    SPELL_COLUMN_OF_FROST           = 70704,
    SPELL_COLUMN_OF_FROST_DAMAGE    = 70702,

    // Other adds
    SPELL_FIREBALL                  = 70754,
    SPELL_LAY_WASTE                 = 69325,
    SPELL_SUPPRESSION               = 70588,
    SPELL_ACID_BURST                = 70744,
    SPELL_GUT_SPRAY                 = 70633,
    SPELL_ROT_WORM_SPAWNER          = 70675,
};

enum ValithriaCreatures
{
    NPC_RISEN_ARCHMAGE          = 37868,
    NPC_BLAZING_SKELETON        = 36791,
    NPC_SUPPRESSER              = 37863,
    NPC_BLISTERING_ZOMBIE       = 37934,
    NPC_GLUTTONOUS_ABOMINATION  = 37886,
    NPC_MANA_VOID               = 38068,
    NPC_COLUMN_OF_FROST         = 37918,
    NPC_ROT_WORM                = 37907,
    NPC_NIGHTMARE_PORTAL        = 38430,
    NPC_NIGHTMARE_CLOUD         = 38421,
    NPC_DREAM_PORTAL            = 37945,
    NPC_DREAM_CLOUD             = 37985,
};

struct ValithriaSpawnLocation
{
    float x, y, z;
};

// The four gates used by the initial Risen Archmage spawns in the CMaNGOS DB.
static ValithriaSpawnLocation const aValithriaGateLocations[4] =
{
    {4230.439f, 2478.563f, 364.961f},
    {4230.535f, 2490.222f, 364.961f},
    {4222.863f, 2504.575f, 364.961f},
    {4223.405f, 2465.113f, 364.961f},
};

static uint32 const aCleanupEntries[] =
{
    NPC_RISEN_ARCHMAGE,
    NPC_BLAZING_SKELETON,
    NPC_SUPPRESSER,
    NPC_BLISTERING_ZOMBIE,
    NPC_GLUTTONOUS_ABOMINATION,
    NPC_MANA_VOID,
    NPC_COLUMN_OF_FROST,
    NPC_ROT_WORM,
    NPC_NIGHTMARE_PORTAL,
    NPC_NIGHTMARE_CLOUD,
    NPC_DREAM_PORTAL,
    NPC_DREAM_CLOUD,
};

struct boss_valithria_dreamwalkerAI : public ScriptedAI
{
    boss_valithria_dreamwalkerAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
        SetCombatMovement(false);
        SetReactState(REACT_PASSIVE);
        SetDeathPrevention(true);

        // Retail prevents percentage-based heals from bypassing the encounter.
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_STATE, SPELL_AURA_OBS_MOD_HEALTH, true);
        m_creature->ApplySpellImmune(nullptr, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_PCT, true);
        Reset();
    }

    instance_icecrown_citadel* m_instance;
    GuidList m_summonGuids;

    uint32 m_portalTimer;
    uint32 m_abominationTimer;
    uint32 m_suppresserTimer;
    uint32 m_zombieTimer;
    uint32 m_archmageTimer;
    uint32 m_skeletonTimer;
    uint32 m_wipeCheckTimer;
    uint32 m_berserkTimer;
    uint32 m_dreamSlipTimer;
    uint32 m_elapsedTime;

    bool m_encounterActive;
    bool m_victory;
    bool m_said75Percent;
    bool m_said25Percent;
    bool m_berserk;
    uint32 m_portalsSpawned;
    uint32 m_portalsUsed;

    void Reset() override
    {
        m_encounterActive = false;
        m_victory = m_instance && m_instance->GetData(TYPE_VALITHRIA) == DONE;
        m_said75Percent = false;
        m_said25Percent = false;
        m_berserk = false;

        m_portalTimer = urand(45000, 48000);
        m_abominationTimer = 5000;
        m_suppresserTimer = 10000;
        m_zombieTimer = 15000;
        m_archmageTimer = 20000;
        m_skeletonTimer = 30000;
        m_wipeCheckTimer = 3000;
        m_berserkTimer = 7 * MINUTE * IN_MILLISECONDS;
        m_dreamSlipTimer = 0;
        m_elapsedTime = 0;
        m_portalsSpawned = 0;
        m_portalsUsed = 0;

        if (m_victory)
        {
            m_creature->SetHealth(m_creature->GetMaxHealth());
            m_creature->RemoveAurasDueToSpell(SPELL_CORRUPTION);
        }
        else
            m_creature->SetHealth(m_creature->GetMaxHealth() / 2);
    }

    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }

    void HealedBy(Unit* /*healer*/, uint32& /*healedAmount*/) override
    {
        if (m_instance)
        {
            uint32 state = m_instance->GetData(TYPE_VALITHRIA);
            if (state != IN_PROGRESS && state != DONE)
                m_instance->SetData(TYPE_VALITHRIA, IN_PROGRESS);
        }
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damageType*/, SpellEntry const* /*spellInfo*/) override
    {
        if (!m_encounterActive || m_victory)
            return;

        if (!m_said25Percent && damage < m_creature->GetHealth() &&
            m_creature->GetHealth() - damage <= m_creature->GetMaxHealth() / 4)
        {
            m_said25Percent = true;
            DoScriptText(SAY_25_HEALTH, m_creature);
        }

        if (damage >= m_creature->GetHealth())
        {
            damage = 0;
            DoScriptText(SAY_0_HEALTH, m_creature);
            FailEncounter();
        }
    }

    void JustReachedHome() override
    {
        if (m_encounterActive)
            FailEncounter();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!m_victory)
        {
            DoScriptText(SAY_0_HEALTH, m_creature);
            FailEncounter();
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        m_summonGuids.push_back(summoned->GetObjectGuid());

        switch (summoned->GetEntry())
        {
            case NPC_DREAM_PORTAL:
            case NPC_NIGHTMARE_PORTAL:
                summoned->SetPhaseMask(1, true);
                summoned->AI()->SetReactState(REACT_PASSIVE);
                break;
            case NPC_DREAM_CLOUD:
                summoned->SetPhaseMask(16, true);
                summoned->CastSpell(summoned, SPELL_DREAM_CLOUD_VISUAL, TRIGGERED_OLD_TRIGGERED);
                summoned->AI()->SetReactState(REACT_PASSIVE);
                break;
            case NPC_NIGHTMARE_CLOUD:
                summoned->SetPhaseMask(16, true);
                summoned->CastSpell(summoned, SPELL_NIGHTMARE_CLOUD_VISUAL, TRIGGERED_OLD_TRIGGERED);
                summoned->CastSpell(summoned, SPELL_NIGHTMARE_CLOUD, TRIGGERED_OLD_TRIGGERED);
                summoned->AI()->SetReactState(REACT_PASSIVE);
                break;
            case NPC_SUPPRESSER:
                summoned->SetPhaseMask(1, true);
                summoned->SetInCombatWithZone();
                summoned->AI()->AttackStart(m_creature);
                break;
            default:
                summoned->SetPhaseMask(1, true);
                summoned->SetInCombatWithZone();
                summoned->AI()->AttackClosestEnemy();
                break;
        }
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(SAY_PLAYER_DIES, m_creature, victim);
    }

    void StartEncounter()
    {
        if (m_encounterActive || m_victory)
            return;

        m_encounterActive = true;
        m_elapsedTime = 0;
        m_portalsSpawned = 0;
        m_portalsUsed = 0;
        if (m_instance)
            m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_PORTAL_JOCKEY, true);
        DoScriptText(SAY_AGGRO, m_creature);

        // Pull every initial channeler when any one of them (or Valithria) is
        // engaged. The four static spawns are the retail encounter starters.
        CreatureList archmages;
        GetCreatureListWithEntryInGrid(archmages, m_creature, NPC_RISEN_ARCHMAGE, 130.0f);
        for (CreatureList::iterator itr = archmages.begin(); itr != archmages.end(); ++itr)
        {
            Creature* archmage = *itr;
            if (!archmage->IsTemporarySummon() && archmage->IsAlive())
            {
                archmage->InterruptNonMeleeSpells(false);
                archmage->SetInCombatWithZone();
                archmage->AI()->AttackClosestEnemy();
            }
        }
    }

    uint32 GetNextSummonDelay(uint32 baseDelay, uint32 minimumDelay, uint32 decayPerMinute) const
    {
        if (m_berserk)
            return urand(5000, 7000);

        uint32 reduction = (m_elapsedTime / MINUTE / IN_MILLISECONDS) * decayPerMinute;
        uint32 delay = baseDelay > minimumDelay + reduction ? baseDelay - reduction : minimumDelay;
        return urand(delay - 1000, delay + 2000);
    }

    void SummonAtGate(uint32 entry, uint32 count)
    {
        for (uint32 i = 0; i < count; ++i)
        {
            ValithriaSpawnLocation const& gate = aValithriaGateLocations[urand(0, 3)];
            float x = gate.x + frand(-3.0f, 3.0f);
            float y = gate.y + frand(-3.0f, 3.0f);
            float orientation = std::atan2(m_creature->GetPositionY() - y, m_creature->GetPositionX() - x);
            m_creature->SummonCreature(entry, x, y, gate.z, orientation, TEMPSPAWN_DEAD_DESPAWN, 0);
        }
    }

    void SummonPortalWave()
    {
        bool heroic = m_instance && m_instance->IsHeroicDifficulty();
        bool is25Man = m_instance && m_instance->Is25ManDifficulty();
        uint32 portalEntry = heroic ? NPC_NIGHTMARE_PORTAL : NPC_DREAM_PORTAL;
        uint32 cloudEntry = heroic ? NPC_NIGHTMARE_CLOUD : NPC_DREAM_CLOUD;
        uint32 portalCount = is25Man ? 8 : 3;
        float angleRange = is25Man ? 6.28318531f : 3.14159265f;

        if (!heroic)
            DoScriptText(SAY_PORTAL, m_creature);

        for (uint32 i = 0; i < portalCount; ++i)
        {
            float angle = 4.71238898f + frand(0.0f, angleRange);
            float distance = frand(20.0f, 30.0f);
            float x = m_creature->GetPositionX() + std::cos(angle) * distance;
            float y = m_creature->GetPositionY() + std::sin(angle) * distance;
            m_creature->SummonCreature(portalEntry, x, y, m_creature->GetPositionZ(), angle,
                TEMPSPAWN_TIMED_DESPAWN, 15000);
            ++m_portalsSpawned;
        }

        // Clouds live only in phase 16 and are consumed by players in Dream State.
        for (uint32 i = 0; i < portalCount * 3; ++i)
        {
            float angle = frand(0.0f, 6.28318531f);
            float distance = frand(10.0f, 38.0f);
            float x = m_creature->GetPositionX() + std::cos(angle) * distance;
            float y = m_creature->GetPositionY() + std::sin(angle) * distance;
            m_creature->SummonCreature(cloudEntry, x, y, m_creature->GetPositionZ() + frand(0.0f, 4.0f), angle,
                TEMPSPAWN_TIMED_DESPAWN, 40000);
        }
    }

    bool HasLivingPlayersNearby() const
    {
        Map::PlayerList const& players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player* player = itr->getSource();
            if (player && player->IsAlive() && !player->IsGameMaster() &&
                m_creature->IsWithinDistInMap(player, 120.0f))
                return true;
        }
        return false;
    }

    void ClearDreamAuras()
    {
        Map::PlayerList const& players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            if (Player* player = itr->getSource())
            {
                player->RemoveAurasDueToSpell(SPELL_DREAM_STATE);
                player->RemoveAurasDueToSpell(SPELL_EMERALD_VIGOR);
                player->RemoveAurasDueToSpell(SPELL_TWISTED_NIGHTMARES);
            }
        }
    }

    void CleanupEncounterCreatures(bool respawnInitialArchmages)
    {
        for (GuidList::const_iterator itr = m_summonGuids.begin(); itr != m_summonGuids.end(); ++itr)
        {
            if (Creature* summon = m_creature->GetMap()->GetCreature(*itr))
                summon->ForcedDespawn();
        }
        m_summonGuids.clear();

        for (uint32 entry : aCleanupEntries)
        {
            CreatureList creatures;
            GetCreatureListWithEntryInGrid(creatures, m_creature, entry, 130.0f);
            for (CreatureList::iterator itr = creatures.begin(); itr != creatures.end(); ++itr)
            {
                Creature* creature = *itr;
                if (entry == NPC_RISEN_ARCHMAGE && !creature->IsTemporarySummon())
                {
                    if (!respawnInitialArchmages)
                        continue;

                    if (!creature->IsAlive())
                        creature->Respawn();
                    else
                        creature->AI()->EnterEvadeMode();
                }
                else if (creature->IsTemporarySummon())
                    creature->ForcedDespawn();
            }
        }
    }

    void FailEncounter()
    {
        if (!m_encounterActive || m_victory)
            return;

        m_encounterActive = false;
        if (m_instance)
            m_instance->SetData(TYPE_VALITHRIA, FAIL);

        CleanupEncounterCreatures(true);
        ClearDreamAuras();
        Reset();
    }

    void CompleteEncounter()
    {
        if (!m_encounterActive || m_victory)
            return;

        m_encounterActive = false;
        m_victory = true;
        DoScriptText(SAY_VICTORY, m_creature);
        m_creature->RemoveAurasDueToSpell(SPELL_CORRUPTION);
        DoCastSpellIfCan(m_creature, SPELL_DREAMWALKER_RAGE);

        CleanupEncounterCreatures(false);
        ClearDreamAuras();

        if (m_instance)
        {
            if (m_portalsUsed != m_portalsSpawned)
                m_instance->SetSpecialAchievementCriteria(TYPE_ACHIEV_PORTAL_JOCKEY, false);
            m_instance->SetData(TYPE_VALITHRIA, DONE);
        }

        m_dreamSlipTimer = 3500;
    }

    void UpdateSummonTimers(uint32 diff)
    {
        if (m_abominationTimer <= diff)
        {
            SummonAtGate(NPC_GLUTTONOUS_ABOMINATION, 1);
            m_abominationTimer = GetNextSummonDelay(30000, 10000, 1000);
        }
        else
            m_abominationTimer -= diff;

        if (m_suppresserTimer <= diff)
        {
            SummonAtGate(NPC_SUPPRESSER, m_instance && m_instance->Is25ManDifficulty() ? 6 : 3);
            m_suppresserTimer = GetNextSummonDelay(30000, 10000, 1000);
        }
        else
            m_suppresserTimer -= diff;

        if (m_zombieTimer <= diff)
        {
            SummonAtGate(NPC_BLISTERING_ZOMBIE, 1);
            m_zombieTimer = GetNextSummonDelay(30000, 10000, 1000);
        }
        else
            m_zombieTimer -= diff;

        if (m_archmageTimer <= diff)
        {
            SummonAtGate(NPC_RISEN_ARCHMAGE, 1);
            m_archmageTimer = GetNextSummonDelay(45000, 10000, 1000);
        }
        else
            m_archmageTimer -= diff;

        if (m_skeletonTimer <= diff)
        {
            SummonAtGate(NPC_BLAZING_SKELETON, 1);
            m_skeletonTimer = GetNextSummonDelay(60000, 10000, 5000);
        }
        else
            m_skeletonTimer -= diff;
    }

    void UpdateAI(uint32 diff) override
    {
        if (m_dreamSlipTimer)
        {
            if (m_dreamSlipTimer <= diff)
            {
                DoCastSpellIfCan(m_creature, SPELL_DREAM_SLIP);
                m_dreamSlipTimer = 0;
            }
            else
                m_dreamSlipTimer -= diff;
        }

        if (!m_instance)
            return;

        if (m_instance->GetData(TYPE_VALITHRIA) == IN_PROGRESS && !m_encounterActive && !m_victory)
            StartEncounter();

        if (!m_encounterActive)
            return;

        m_elapsedTime += diff;

        if (!m_said75Percent && m_creature->GetHealth() >= m_creature->GetMaxHealth() * 3 / 4)
        {
            m_said75Percent = true;
            DoScriptText(SAY_75_HEALTH, m_creature);
        }

        if (m_creature->GetHealth() >= m_creature->GetMaxHealth())
        {
            CompleteEncounter();
            return;
        }

        if (m_wipeCheckTimer <= diff)
        {
            if (!HasLivingPlayersNearby())
            {
                FailEncounter();
                return;
            }
            m_wipeCheckTimer = 3000;
        }
        else
            m_wipeCheckTimer -= diff;

        if (m_instance->IsHeroicDifficulty() && !m_berserk)
        {
            if (m_berserkTimer <= diff)
            {
                m_berserk = true;
                DoScriptText(SAY_BERSERK, m_creature);
            }
            else
                m_berserkTimer -= diff;
        }

        if (m_portalTimer <= diff)
        {
            SummonPortalWave();
            m_portalTimer = urand(45000, 48000);
        }
        else
            m_portalTimer -= diff;

        UpdateSummonTimers(diff);
    }

    void PortalUsed() { ++m_portalsUsed; }
};

struct valithria_hostile_addAI : public ScriptedAI
{
    valithria_hostile_addAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
    }

    instance_icecrown_citadel* m_instance;

    void Aggro(Unit* /*who*/) override
    {
        if (m_instance)
        {
            uint32 state = m_instance->GetData(TYPE_VALITHRIA);
            if (state != IN_PROGRESS && state != DONE)
                m_instance->SetData(TYPE_VALITHRIA, IN_PROGRESS);
        }
        m_creature->SetInCombatWithZone();
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() != TYPEID_PLAYER || !m_instance)
            return;

        if (Creature* valithria = m_instance->GetSingleCreatureFromStorage(NPC_VALITHRIA))
            DoScriptText(SAY_PLAYER_DIES, valithria, victim);
    }
};

struct npc_risen_archmage_iccAI : public valithria_hostile_addAI
{
    npc_risen_archmage_iccAI(Creature* creature) : valithria_hostile_addAI(creature),
        m_initialArchmage(!creature->IsTemporarySummon())
    {
        Reset();
    }

    uint32 m_frostboltVolleyTimer;
    uint32 m_manaVoidTimer;
    uint32 m_columnOfFrostTimer;
    bool m_initialArchmage;

    void Reset() override
    {
        m_frostboltVolleyTimer = urand(5000, 15000);
        m_manaVoidTimer = urand(20000, 25000);
        m_columnOfFrostTimer = urand(10000, 20000);
    }

    void Aggro(Unit* who) override
    {
        m_creature->InterruptNonMeleeSpells(false);
        valithria_hostile_addAI::Aggro(who);
    }

    void JustSummoned(Creature* summoned) override
    {
        summoned->SetPhaseMask(1, true);
        summoned->AI()->SetReactState(REACT_PASSIVE);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_creature->IsInCombat() && m_initialArchmage &&
            (!m_instance || m_instance->GetData(TYPE_VALITHRIA) != DONE))
        {
            if (!m_creature->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                DoCastSpellIfCan(m_creature, SPELL_ARCHMAGE_CORRUPTION);
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_frostboltVolleyTimer <= diff)
        {
            DoCastSpellIfCan(m_creature, SPELL_FROSTBOLT_VOLLEY);
            m_frostboltVolleyTimer = urand(8000, 15000);
        }
        else
            m_frostboltVolleyTimer -= diff;

        if (m_manaVoidTimer <= diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_MANA_VOID,
                SELECT_FLAG_PLAYER | SELECT_FLAG_POWER_MANA))
                DoCastSpellIfCan(target, SPELL_MANA_VOID);
            m_manaVoidTimer = urand(20000, 25000);
        }
        else
            m_manaVoidTimer -= diff;

        if (m_columnOfFrostTimer <= diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_COLUMN_OF_FROST,
                SELECT_FLAG_PLAYER))
                DoCastSpellIfCan(target, SPELL_COLUMN_OF_FROST);
            m_columnOfFrostTimer = urand(15000, 25000);
        }
        else
            m_columnOfFrostTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct npc_blazing_skeleton_iccAI : public valithria_hostile_addAI
{
    npc_blazing_skeleton_iccAI(Creature* creature) : valithria_hostile_addAI(creature) { Reset(); }

    uint32 m_fireballTimer;
    uint32 m_layWasteTimer;

    void Reset() override
    {
        m_fireballTimer = urand(2000, 4000);
        m_layWasteTimer = urand(15000, 20000);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_fireballTimer <= diff)
        {
            if (!m_creature->CanReachWithMeleeAttack(m_creature->GetVictim()))
                DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FIREBALL);
            m_fireballTimer = urand(2000, 4000);
        }
        else
            m_fireballTimer -= diff;

        if (m_layWasteTimer <= diff)
        {
            DoCastSpellIfCan(m_creature, SPELL_LAY_WASTE);
            m_layWasteTimer = urand(15000, 20000);
        }
        else
            m_layWasteTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct npc_suppresser_iccAI : public valithria_hostile_addAI
{
    npc_suppresser_iccAI(Creature* creature) : valithria_hostile_addAI(creature)
    {
        SetReactState(REACT_DEFENSIVE);
        Reset();
    }

    uint32 m_suppressionTimer;

    void Reset() override { m_suppressionTimer = 500; }

    void AttackStart(Unit* who) override
    {
        if (who && who->GetEntry() == NPC_VALITHRIA)
            ScriptedAI::AttackStart(who);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_creature->GetVictim() && m_instance)
        {
            if (Creature* valithria = m_instance->GetSingleCreatureFromStorage(NPC_VALITHRIA))
                AttackStart(valithria);
        }

        Unit* victim = m_creature->GetVictim();
        if (!victim || victim->GetEntry() != NPC_VALITHRIA)
            return;

        if (m_suppressionTimer <= diff)
        {
            if (m_creature->CanReachWithMeleeAttack(victim) && !m_creature->IsNonMeleeSpellCasted(false))
                DoCastSpellIfCan(m_creature, SPELL_SUPPRESSION);
            m_suppressionTimer = 1000;
        }
        else
            m_suppressionTimer -= diff;
    }
};

struct npc_blistering_zombie_iccAI : public valithria_hostile_addAI
{
    npc_blistering_zombie_iccAI(Creature* creature) : valithria_hostile_addAI(creature) { Reset(); }

    uint32 m_burstTimer;
    bool m_burstStarted;

    void Reset() override
    {
        m_burstTimer = 0;
        m_burstStarted = false;
        SetCombatMovement(true);
    }

    void DamageTaken(Unit* /*dealer*/, uint32& damage, DamageEffectType /*damageType*/, SpellEntry const* /*spellInfo*/) override
    {
        if (!m_burstStarted && damage >= m_creature->GetHealth())
        {
            damage = m_creature->GetHealth() > 1 ? m_creature->GetHealth() - 1 : 0;
            m_burstStarted = true;
            m_burstTimer = 750;
            SetCombatMovement(false);
            DoStopAttack();
            DoCastSpellIfCan(m_creature, SPELL_ACID_BURST);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (m_burstStarted)
        {
            if (m_burstTimer <= diff)
            {
                m_creature->SetDisplayId(11686);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                m_creature->ForcedDespawn(2000);
                m_burstTimer = 0;
            }
            else
                m_burstTimer -= diff;
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
        DoMeleeAttackIfReady();
    }
};

struct npc_gluttonous_abomination_iccAI : public valithria_hostile_addAI
{
    npc_gluttonous_abomination_iccAI(Creature* creature) : valithria_hostile_addAI(creature) { Reset(); }

    uint32 m_gutSprayTimer;

    void Reset() override { m_gutSprayTimer = urand(10000, 13000); }

    void JustSummoned(Creature* summoned) override
    {
        summoned->SetPhaseMask(1, true);
        summoned->SetInCombatWithZone();
        summoned->AI()->AttackClosestEnemy();
    }

    void JustDied(Unit* /*killer*/) override
    {
        m_creature->CastSpell(m_creature, SPELL_ROT_WORM_SPAWNER, TRIGGERED_OLD_TRIGGERED);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_gutSprayTimer <= diff)
        {
            DoCastSpellIfCan(m_creature, SPELL_GUT_SPRAY);
            m_gutSprayTimer = urand(10000, 13000);
        }
        else
            m_gutSprayTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct npc_valithria_rot_wormAI : public valithria_hostile_addAI
{
    npc_valithria_rot_wormAI(Creature* creature) : valithria_hostile_addAI(creature) { }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
        DoMeleeAttackIfReady();
    }
};

struct npc_valithria_column_of_frostAI : public ScriptedAI
{
    npc_valithria_column_of_frostAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    uint32 m_damageTimer;

    void Reset() override
    {
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
        m_damageTimer = 2000;
    }

    void AttackStart(Unit* /*who*/) override { }

    void UpdateAI(uint32 diff) override
    {
        if (!m_damageTimer)
            return;
        if (m_damageTimer <= diff)
        {
            DoCastSpellIfCan(m_creature, SPELL_COLUMN_OF_FROST_DAMAGE);
            m_creature->ForcedDespawn(8000);
            m_damageTimer = 0;
        }
        else
            m_damageTimer -= diff;
    }
};

struct npc_valithria_mana_voidAI : public ScriptedAI
{
    npc_valithria_mana_voidAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    void Reset() override
    {
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
        m_creature->ForcedDespawn(36000);
    }

    void AttackStart(Unit* /*who*/) override { }
    void UpdateAI(uint32 /*diff*/) override { }
};

struct npc_valithria_portalAI : public ScriptedAI
{
    npc_valithria_portalAI(Creature* creature) : ScriptedAI(creature)
    {
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
    }

    void Reset() override { }
    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }
    void UpdateAI(uint32 /*diff*/) override { }
};

struct npc_valithria_cloudAI : public ScriptedAI
{
    npc_valithria_cloudAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
        SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
        Reset();
    }

    instance_icecrown_citadel* m_instance;
    uint32 m_playerCheckTimer;
    bool m_consumed;

    void Reset() override
    {
        m_playerCheckTimer = 750;
        m_consumed = false;
    }

    void AttackStart(Unit* /*who*/) override { }
    void MoveInLineOfSight(Unit* /*who*/) override { }

    void UpdateAI(uint32 diff) override
    {
        if (m_consumed || !m_instance || m_instance->GetData(TYPE_VALITHRIA) != IN_PROGRESS)
            return;

        if (m_playerCheckTimer > diff)
        {
            m_playerCheckTimer -= diff;
            return;
        }
        m_playerCheckTimer = 750;

        Map::PlayerList const& players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player* player = itr->getSource();
            if (!player || !player->IsAlive() || !(player->GetPhaseMask() & m_creature->GetPhaseMask()) ||
                !m_creature->IsWithinDistInMap(player, 5.0f))
                continue;

            m_consumed = true;
            player->CastSpell(player, SPELL_EMERALD_VIGOR, TRIGGERED_OLD_TRIGGERED);
            if (m_creature->GetEntry() == NPC_NIGHTMARE_CLOUD)
                m_creature->CastSpell(player, SPELL_TWISTED_NIGHTMARES, TRIGGERED_OLD_TRIGGERED);
            m_creature->ForcedDespawn(1000);
            return;
        }
    }
};

bool NpcSpellClick_npc_valithria_portal(Player* player, Creature* portal, uint32 /*spellId*/)
{
    instance_icecrown_citadel* instance = static_cast<instance_icecrown_citadel*>(portal->GetInstanceData());
    if (!instance || instance->GetData(TYPE_VALITHRIA) != IN_PROGRESS || !player->IsAlive())
        return true;

    player->CastSpell(player, SPELL_DREAM_STATE, TRIGGERED_OLD_TRIGGERED);
    if (Creature* valithria = instance->GetSingleCreatureFromStorage(NPC_VALITHRIA))
        if (boss_valithria_dreamwalkerAI* ai = dynamic_cast<boss_valithria_dreamwalkerAI*>(valithria->AI()))
            ai->PortalUsed();
    portal->ForcedDespawn();
    return true;
}

UnitAI* GetAI_boss_valithria_dreamwalker(Creature* creature) { return new boss_valithria_dreamwalkerAI(creature); }
UnitAI* GetAI_npc_risen_archmage_icc(Creature* creature) { return new npc_risen_archmage_iccAI(creature); }
UnitAI* GetAI_npc_blazing_skeleton_icc(Creature* creature) { return new npc_blazing_skeleton_iccAI(creature); }
UnitAI* GetAI_npc_suppresser_icc(Creature* creature) { return new npc_suppresser_iccAI(creature); }
UnitAI* GetAI_npc_blistering_zombie_icc(Creature* creature) { return new npc_blistering_zombie_iccAI(creature); }
UnitAI* GetAI_npc_gluttonous_abomination_icc(Creature* creature) { return new npc_gluttonous_abomination_iccAI(creature); }
UnitAI* GetAI_npc_valithria_rot_worm(Creature* creature) { return new npc_valithria_rot_wormAI(creature); }
UnitAI* GetAI_npc_valithria_column_of_frost(Creature* creature) { return new npc_valithria_column_of_frostAI(creature); }
UnitAI* GetAI_npc_valithria_mana_void(Creature* creature) { return new npc_valithria_mana_voidAI(creature); }
UnitAI* GetAI_npc_valithria_portal(Creature* creature) { return new npc_valithria_portalAI(creature); }
UnitAI* GetAI_npc_valithria_cloud(Creature* creature) { return new npc_valithria_cloudAI(creature); }

void AddSC_boss_valithria_dreamwalker()
{
    Script* script = new Script;
    script->Name = "boss_valithria_dreamwalker";
    script->GetAI = &GetAI_boss_valithria_dreamwalker;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_risen_archmage_icc";
    script->GetAI = &GetAI_npc_risen_archmage_icc;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_blazing_skeleton_icc";
    script->GetAI = &GetAI_npc_blazing_skeleton_icc;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_suppresser_icc";
    script->GetAI = &GetAI_npc_suppresser_icc;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_blistering_zombie_icc";
    script->GetAI = &GetAI_npc_blistering_zombie_icc;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_gluttonous_abomination_icc";
    script->GetAI = &GetAI_npc_gluttonous_abomination_icc;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_valithria_rot_worm";
    script->GetAI = &GetAI_npc_valithria_rot_worm;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_valithria_column_of_frost";
    script->GetAI = &GetAI_npc_valithria_column_of_frost;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_valithria_mana_void";
    script->GetAI = &GetAI_npc_valithria_mana_void;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_valithria_portal";
    script->GetAI = &GetAI_npc_valithria_portal;
    script->pNpcSpellClick = &NpcSpellClick_npc_valithria_portal;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_valithria_cloud";
    script->GetAI = &GetAI_npc_valithria_cloud;
    script->RegisterSelf();
}
