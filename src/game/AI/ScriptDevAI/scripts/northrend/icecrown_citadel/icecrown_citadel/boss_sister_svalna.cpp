/* Frostwing Halls gauntlet and Sister Svalna encounter.
 * Adapted to CMaNGOS ScriptDevAI from the retail flow preserved by
 * TrinityCore's 3.3.5 implementation. */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "AI/ScriptDevAI/base/escort_ai.h"
#include "AI/EventAI/CreatureEventAI.h"
#include "icecrown_citadel.h"

enum
{
    SAY_CROK_INTRO_1       = 36945,
    SAY_ARNATH_INTRO_2     = 36948,
    SAY_CROK_INTRO_3       = 36946,
    SAY_SVALNA_EVENT_START = 37024,
    SAY_SVALNA_RESURRECT   = 37020,
    SAY_SVALNA_AGGRO       = 37653,
    SAY_SVALNA_KILL_PLAYER = 37654,
    SAY_SVALNA_DEATH       = 37135,

    SPELL_REVIVE_CHAMPION  = 70053,
    SPELL_CARESS_OF_DEATH  = 70078,
    SPELL_UNDEATH          = 70089,
    SPELL_ICEBOUND_ARMOR   = 70714,
    SPELL_DEATH_STRIKE     = 71489,
    SPELL_IMPALING_SPEAR   = 71443,
    SPELL_AETHER_SHIELD    = 71463,
    SPELL_DIVINE_SURGE     = 71465,
    SPELL_HURL_SPEAR       = 71466,

    NPC_CAPTAIN_ARNATH_UNDEAD  = 37491,
    NPC_CAPTAIN_BRANDON_UNDEAD = 37493,
    NPC_CAPTAIN_GRONDEL_UNDEAD = 37494,
    NPC_CAPTAIN_RUPERT_UNDEAD  = 37495,

    POINT_SVALNA_LAND      = 1,
};

static uint32 const aCaptainEntries[] =
{
    NPC_CAPTAIN_ARNATH, NPC_CAPTAIN_BRANDON,
    NPC_CAPTAIN_GRONDEL, NPC_CAPTAIN_RUPERT
};

static uint32 GetUndeadCaptainEntry(uint32 entry)
{
    switch (entry)
    {
        case NPC_CAPTAIN_ARNATH:  return NPC_CAPTAIN_ARNATH_UNDEAD;
        case NPC_CAPTAIN_BRANDON: return NPC_CAPTAIN_BRANDON_UNDEAD;
        case NPC_CAPTAIN_GRONDEL: return NPC_CAPTAIN_GRONDEL_UNDEAD;
        case NPC_CAPTAIN_RUPERT:  return NPC_CAPTAIN_RUPERT_UNDEAD;
        default:                  return 0;
    }
}

static bool IsUndeadCaptain(uint32 entry)
{
    return entry == NPC_CAPTAIN_ARNATH_UNDEAD || entry == NPC_CAPTAIN_BRANDON_UNDEAD ||
           entry == NPC_CAPTAIN_GRONDEL_UNDEAD || entry == NPC_CAPTAIN_RUPERT_UNDEAD;
}

struct boss_sister_svalnaAI : public ScriptedAI
{
    boss_sister_svalnaAI(Creature* creature) : ScriptedAI(creature)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
        Reset();
    }

    instance_icecrown_citadel* m_instance;
    bool m_eventStarted;
    bool m_landed;
    uint32 m_eventStartTimer;
    uint32 m_reviveTimer;
    uint32 m_spearTimer;

    void Reset() override
    {
        m_eventStarted = false;
        m_landed = false;
        m_eventStartTimer = 0;
        m_reviveTimer = 0;
        m_spearTimer = 0;
        m_creature->SetActiveObjectState(false);
        m_creature->SetAnimTier(AnimTier::Hover);
        m_creature->SetLevitate(true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        SetCombatMovement(false);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit*, Unit*, uint32) override
    {
        if (eventType == AI_EVENT_CUSTOM_A && !m_eventStarted)
        {
            m_eventStarted = true;
            // The escort spans several grid cells; keep Svalna loaded while
            // she controls the remote gauntlet dialogue and captain deaths.
            m_creature->SetActiveObjectState(true);
            m_eventStartTimer = 25000;
        }
        else if (eventType == AI_EVENT_CUSTOM_B && !m_landed)
        {
            // Retail waits seven seconds at the end of the gauntlet before
            // reviving the two fallen captains and landing.
            m_reviveTimer = 7000;
        }
        else if (eventType == AI_EVENT_CUSTOM_C && m_eventStarted && !m_landed)
        {
            // Retail kills one random surviving captain at each of the two
            // middle gauntlet stops. The revive spell later converts them
            // into Svalna's undead champions.
            std::vector<Creature*> captains;
            for (uint32 entry : aCaptainEntries)
                if (Creature* captain = m_instance->GetSingleCreatureFromStorage(entry))
                    if (captain->IsAlive())
                        captains.push_back(captain);
            if (!captains.empty())
                m_creature->CastSpell(captains[urand(0, captains.size() - 1)], SPELL_CARESS_OF_DEATH, TRIGGERED_NONE);
        }
    }

    void MovementInform(uint32 type, uint32 point) override
    {
        if (type != POINT_MOTION_TYPE || point != POINT_SVALNA_LAND)
            return;

        m_landed = true;
        m_creature->SetLevitate(false);
        m_creature->SetAnimTier(AnimTier::Ground);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        SetCombatMovement(true);
        DoBroadcastText(SAY_SVALNA_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_DIVINE_SURGE, CAST_TRIGGERED);
        m_creature->SetInCombatWithZone();
        m_spearTimer = urand(40000, 50000);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            DoBroadcastText(SAY_SVALNA_KILL_PLAYER, m_creature);
    }

    void JustDied(Unit*) override
    {
        DoBroadcastText(SAY_SVALNA_DEATH, m_creature);
        m_creature->SetActiveObjectState(false);

        if (!m_instance)
            return;

        if (Creature* crok = m_instance->GetSingleCreatureFromStorage(NPC_CROK_SCOURGEBANE))
            crok->AI()->SendAIEvent(AI_EVENT_CUSTOM_E, m_creature, crok);
        for (uint32 entry : aCaptainEntries)
            if (Creature* captain = m_instance->GetSingleCreatureFromStorage(entry))
                captain->AI()->SendAIEvent(AI_EVENT_CUSTOM_E, m_creature, captain);
    }

    void JustReachedHome() override
    {
        if (!m_instance || m_instance->GetData(TYPE_FROST_WING_ENTRANCE) == DONE)
            return;

        // A wipe restores the complete escort formation, including captains
        // that Svalna had already converted to their undead entries.
        for (uint32 entry : aCaptainEntries)
            if (Creature* captain = m_instance->GetSingleCreatureFromStorage(entry))
                captain->AI()->SendAIEvent(AI_EVENT_CUSTOM_D, m_creature, captain);
        if (Creature* crok = m_instance->GetSingleCreatureFromStorage(NPC_CROK_SCOURGEBANE))
            crok->AI()->SendAIEvent(AI_EVENT_CUSTOM_D, m_creature, crok);
    }

    void SpellHit(Unit*, SpellEntry const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_HURL_SPEAR && m_creature->HasAura(SPELL_AETHER_SHIELD))
            m_creature->RemoveAurasDueToSpell(SPELL_AETHER_SHIELD);
    }

    void UpdateAI(uint32 diff) override
    {
        if (m_eventStartTimer)
        {
            if (m_eventStartTimer <= diff)
            {
                m_eventStartTimer = 0;
                DoBroadcastText(SAY_SVALNA_EVENT_START, m_creature);
            }
            else
                m_eventStartTimer -= diff;
        }

        if (m_reviveTimer)
        {
            if (m_reviveTimer <= diff)
            {
                m_reviveTimer = 0;
                DoBroadcastText(SAY_SVALNA_RESURRECT, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_REVIVE_CHAMPION);
                m_creature->SetLevitate(true);
                m_creature->GetMotionMaster()->MovePoint(POINT_SVALNA_LAND, 4356.88f, 2512.40f, 358.436f);
            }
            else
                m_reviveTimer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_spearTimer <= diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_IMPALING_SPEAR, SELECT_FLAG_PLAYER))
            {
                // Aether Shield and Impaling Spear are one linked mechanic,
                // not two independent timers.
                DoCastSpellIfCan(m_creature, SPELL_AETHER_SHIELD, CAST_TRIGGERED);
                DoCastSpellIfCan(target, SPELL_IMPALING_SPEAR);
            }
            m_spearTimer = urand(20000, 25000);
        }
        else
            m_spearTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct npc_crok_scourgebaneAI : public npc_escortAI
{
    npc_crok_scourgebaneAI(Creature* creature) : npc_escortAI(creature)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
        InitializeEventState();
    }

    void InitializeEventState()
    {
        m_started = false;
        m_introStep = 0;
        m_introTimer = 0;
        m_trashCheckTimer = 1000;
        m_pausedPoint = 0;
        m_deathStrikeTimer = urand(25000, 30000);
    }

    instance_icecrown_citadel* m_instance;
    bool m_started;
    uint8 m_introStep;
    uint32 m_introTimer;
    uint32 m_trashCheckTimer;
    uint8 m_pausedPoint;
    uint32 m_deathStrikeTimer;

    void Reset() override
    {
        m_deathStrikeTimer = urand(25000, 30000);
    }

    void JustRespawned() override
    {
        npc_escortAI::JustRespawned();
        InitializeEventState();
        m_creature->SetActiveObjectState(false);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit*, Unit*, uint32) override
    {
        if (eventType == AI_EVENT_CUSTOM_D)
        {
            End();
            m_creature->SetActiveObjectState(false);
            m_creature->CombatStop(true);
            if (m_creature->IsAlive())
                m_creature->SetDeathState(JUST_DIED);
            m_creature->Respawn();
        }
        else if (eventType == AI_EVENT_CUSTOM_E)
        {
            End();
            m_creature->SetActiveObjectState(false);
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        npc_escortAI::MoveInLineOfSight(who);
        if (m_started || who->GetTypeId() != TYPEID_PLAYER || !m_instance ||
                m_instance->GetData(TYPE_FROST_WING_ENTRANCE) == DONE ||
                !m_creature->IsWithinDistInMap(who, 35.0f))
            return;

        m_started = true;
        m_introStep = 1;
        m_introTimer = 7000;
        // Svalna begins well beyond Crok's current grid. Load her grid before
        // resolving the instance storage entry, then keep the escort owner
        // active until the group reaches her platform.
        m_creature->GetMap()->ForceLoadGrid(4356.71f, 2484.33f);
        m_creature->SetActiveObjectState(true);
        DoBroadcastText(SAY_CROK_INTRO_1, m_creature);

        if (Creature* svalna = m_instance->GetSingleCreatureFromStorage(NPC_SISTER_SVALNA))
            svalna->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, svalna);

        for (uint32 entry : aCaptainEntries)
            if (Creature* captain = m_instance->GetSingleCreatureFromStorage(entry))
                captain->AI()->SendAIEvent(AI_EVENT_CUSTOM_A, m_creature, captain);
    }

    void WaypointReached(uint32 point) override
    {
        if (point == 1 || point == 2 || point == 5)
        {
            m_pausedPoint = point;
            if (HasAliveTrashForPoint(point))
                SetEscortPaused(true);
            else if (point == 5)
                StartSvalnaEncounter();
        }
    }

    void WaypointStart(uint32 point) override
    {
        if ((point == 2 || point == 3) && m_instance)
            if (Creature* svalna = m_instance->GetSingleCreatureFromStorage(NPC_SISTER_SVALNA))
                svalna->AI()->SendAIEvent(AI_EVENT_CUSTOM_C, m_creature, svalna);
    }

    bool HasAliveTrashForPoint(uint8 point)
    {
        float minY = point == 1 ? 2600.0f : point == 2 ? 2550.0f : 2500.0f;
        float maxY = minY + 50.0f;
        uint32 const entries[] = {37127, 37132, 37133, 37134};
        for (uint32 entry : entries)
        {
            CreatureList trash;
            GetCreatureListWithEntryInGrid(trash, m_creature, entry, 100.0f);
            for (Creature* creature : trash)
            {
                float x, y, z;
                creature->GetRespawnCoord(x, y, z);
                if (creature->IsAlive() && y > minY && y < maxY)
                    return true;
            }
        }
        return false;
    }

    void StartSvalnaEncounter()
    {
        m_pausedPoint = 0;
        // End the waypoint path without allowing npc_escortAI to despawn Crok.
        // He and the surviving captains stay present for the Svalna fight.
        End();
        if (Creature* svalna = m_instance->GetSingleCreatureFromStorage(NPC_SISTER_SVALNA))
            svalna->AI()->SendAIEvent(AI_EVENT_CUSTOM_B, m_creature, svalna);
    }

    void UpdateEscortAI(uint32 diff) override
    {
        if (m_pausedPoint && HasEscortState(STATE_ESCORT_PAUSED))
        {
            if (m_trashCheckTimer <= diff)
            {
                m_trashCheckTimer = 1000;
                if (!HasAliveTrashForPoint(m_pausedPoint))
                {
                    uint8 clearedPoint = m_pausedPoint;
                    m_pausedPoint = 0;
                    SetEscortPaused(false);
                    if (clearedPoint == 5)
                        StartSvalnaEncounter();
                }
            }
            else
                m_trashCheckTimer -= diff;
        }

        if (m_introTimer)
        {
            if (m_introTimer <= diff)
            {
                ++m_introStep;
                if (m_introStep == 2)
                {
                    if (Creature* arnath = m_instance->GetSingleCreatureFromStorage(NPC_CAPTAIN_ARNATH))
                        DoBroadcastText(SAY_ARNATH_INTRO_2, arnath);
                    m_introTimer = 7000;
                }
                else if (m_introStep == 3)
                {
                    DoBroadcastText(SAY_CROK_INTRO_3, m_creature);
                    m_introTimer = 21000;
                }
                else
                {
                    m_introTimer = 0;
                    Start(false);
                }
            }
            else
                m_introTimer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Death Strike is conditional in the retail implementation. The
        // routine Scourge Strike remains in the DB spell list.
        if (m_deathStrikeTimer <= diff)
        {
            if (m_creature->GetHealthPercent() < 20.0f)
                DoCastSpellIfCan(m_creature->GetVictim(), SPELL_DEATH_STRIKE);
            m_deathStrikeTimer = urand(5000, 10000);
        }
        else
            m_deathStrikeTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

// The captains keep their existing CMaNGOS EventAI combat rotations. This
// small ScriptDevAI layer owns only the encounter-specific escort lifecycle:
// persistent following, resurrection as hostile champions, and wipe reset.
struct npc_argent_captainAI : public CreatureEventAI
{
    npc_argent_captainAI(Creature* creature) : CreatureEventAI(creature)
    {
        m_instance = static_cast<instance_icecrown_citadel*>(creature->GetInstanceData());
        m_followDistance = 0.0f;
        m_followAngle = 0.0f;
        m_following = false;
        m_undead = IsUndeadCaptain(creature->GetEntry());
    }

    instance_icecrown_citadel* m_instance;
    float m_followDistance;
    float m_followAngle;
    bool m_following;
    bool m_undead;

    void Reset() override
    {
        CreatureEventAI::Reset();
        m_undead = IsUndeadCaptain(m_creature->GetEntry());
    }

    void JustRespawned() override
    {
        CreatureEventAI::JustRespawned();
        m_following = false;
        m_undead = false;
        m_creature->SetActiveObjectState(false);
    }

    void FollowCrok()
    {
        if (!m_instance || m_undead)
            return;
        if (Creature* crok = m_instance->GetSingleCreatureFromStorage(NPC_CROK_SCOURGEBANE))
            m_creature->GetMotionMaster()->MoveFollow(crok, m_followDistance, m_followAngle);
    }

    void ReceiveAIEvent(AIEventType eventType, Unit* sender, Unit* invoker, uint32 miscValue) override
    {
        if (eventType == AI_EVENT_CUSTOM_A)
        {
            if (Creature* crok = m_instance ? m_instance->GetSingleCreatureFromStorage(NPC_CROK_SCOURGEBANE) : nullptr)
            {
                m_followDistance = m_creature->GetDistance(crok);
                m_followAngle = m_creature->GetAngle(crok);
                m_following = true;
                m_creature->SetActiveObjectState(true);
                FollowCrok();
            }
            return;
        }
        if (eventType == AI_EVENT_CUSTOM_D)
        {
            m_following = false;
            m_creature->SetActiveObjectState(false);
            m_creature->CombatStop(true);
            if (m_creature->IsAlive())
                m_creature->SetDeathState(JUST_DIED);
            m_creature->Respawn();
            return;
        }
        if (eventType == AI_EVENT_CUSTOM_E)
        {
            m_following = false;
            m_creature->SetActiveObjectState(false);
            return;
        }

        CreatureEventAI::ReceiveAIEvent(eventType, sender, invoker, miscValue);
    }

    void EnterEvadeMode() override
    {
        bool const resumeFollow = m_following && !m_undead;
        CreatureEventAI::EnterEvadeMode();
        if (resumeFollow)
            FollowCrok();
    }

    void SpellHit(Unit* caster, SpellEntry const* spellInfo) override
    {
        CreatureEventAI::SpellHit(caster, spellInfo);
        if (spellInfo->Id != SPELL_REVIVE_CHAMPION || m_undead)
            return;

        uint32 const undeadEntry = GetUndeadCaptainEntry(m_creature->GetEntry());
        if (!undeadEntry)
            return;

        m_following = false;
        m_creature->SetDeathState(JUST_ALIVED);
        if (!m_creature->UpdateEntry(undeadEntry, nullptr, nullptr, false))
            return;

        m_undead = true;
        m_creature->SetHealth(m_creature->GetMaxHealth());
        m_creature->SetActiveObjectState(true);
        InitAI();
        DoCastSpellIfCan(m_creature, SPELL_UNDEATH, CAST_TRIGGERED);
        m_creature->SetInCombatWithZone();
    }
};

void AddSC_boss_sister_svalna()
{
    Script* script = new Script;
    script->Name = "boss_sister_svalna";
    script->GetAI = &GetNewAIInstance<boss_sister_svalnaAI>;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_crok_scourgebane";
    script->GetAI = &GetNewAIInstance<npc_crok_scourgebaneAI>;
    script->RegisterSelf();

    script = new Script;
    script->Name = "npc_argent_captain";
    script->GetAI = &GetNewAIInstance<npc_argent_captainAI>;
    script->RegisterSelf();
}
