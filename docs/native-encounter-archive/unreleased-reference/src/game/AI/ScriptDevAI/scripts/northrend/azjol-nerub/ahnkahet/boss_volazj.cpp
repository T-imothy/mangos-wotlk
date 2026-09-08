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
SDName: Boss_Volazj
SDComment: Insanity clone combat and phase lifecycle implemented; live timing validation pending
SDCategory: Ahn'kahet
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ahnkahet.h"
#include "Entities/TemporarySpawn.h"
#include <array>

enum
{
    SAY_AGGRO                       = -1619033,
    SAY_INSANITY                    = -1619034,
    SAY_SLAY_1                      = -1619035,
    SAY_SLAY_2                      = -1619036,
    SAY_SLAY_3                      = -1619037,
    SAY_DEATH_1                     = -1619038,         // missing text
    SAY_DEATH_2                     = -1619039,

    SPELL_MIND_FLAY                 = 57941,
    SPELL_MIND_FLAY_H               = 59974,
    SPELL_SHADOW_BOLT               = 57942,
    SPELL_SHADOW_BOLT_H             = 59975,
    SPELL_SHIVER                    = 57949,
    SPELL_SHIVER_H                  = 59978,

    SPELL_WHISPER_AGGRO             = 60291,
    SPELL_WHISPER_INSANITY          = 60292,
    SPELL_WHISPER_SLAY_1            = 60293,
    SPELL_WHISPER_SLAY_2            = 60294,
    SPELL_WHISPER_SLAY_3            = 60295,
    SPELL_WHISPER_DEATH_1           = 60296,
    SPELL_WHISPER_DEATH_2           = 60297,

    SPELL_INSANITY                  = 57496,            // start insanity phasing
    SPELL_INSANITY_VISUAL           = 57561,

    SPELL_TWISTED_VISAGE_EFFECT     = 57507,
    SPELL_TWISTED_VISAGE_PASSIVE    = 57551,

    SPELL_SUMMON_VISAGE_1           = 57500,
    SPELL_SUMMON_VISAGE_2           = 57501,
    SPELL_SUMMON_VISAGE_3           = 57502,
    SPELL_SUMMON_VISAGE_4           = 57503,
    SPELL_SUMMON_VISAGE_5           = 57504,

    MAX_INSANITY_SPELLS             = 5,
};

static const uint32 aInsanityPhaseSpells[MAX_INSANITY_SPELLS] = {SPELL_INSANITY_PHASE_16, SPELL_INSANITY_PHASE_32, SPELL_INSANITY_PHASE_64, SPELL_INSANITY_PHASE_128, SPELL_INSANITY_PHASE_256};
static const uint32 aSpawnVisageSpells[MAX_INSANITY_SPELLS] = {SPELL_SUMMON_VISAGE_1, SPELL_SUMMON_VISAGE_2, SPELL_SUMMON_VISAGE_3, SPELL_SUMMON_VISAGE_4, SPELL_SUMMON_VISAGE_5};


// NPC spell choices and cadence follow TrinityCore's 3.3.5 Volazj implementation.
// These are the encounter's clone spells, never a player's learned spell ranks.
// https://github.com/TrinityCore/TrinityCore/blob/3.3.5/src/server/scripts/Northrend/AzjolNerub/Ahnkahet/boss_herald_volazj.cpp
enum VolazjAbilityTarget { VISAGE_VICTIM, VISAGE_SELF, VISAGE_FRIENDLY };
struct VolazjAbility
{
    uint32 spell = 0, initial = 0, repeatMin = 0, repeatMax = 0;
    VolazjAbilityTarget target = VISAGE_VICTIM;
    float minDistance = 0.0f, maxDistance = 0.0f;
};
struct VolazjProfile
{
    std::array<VolazjAbility, 3> abilities{};
    bool ranged = false, dualWield = false;
    uint32 form = 0;
};

static VolazjProfile GetVolazjProfile(uint32 playerClass, uint32 spec)
{
    VolazjProfile profile;
    uint32 next = 0;
    auto add = [&](uint32 spell, uint32 initial, uint32 low, uint32 high,
        VolazjAbilityTarget target = VISAGE_VICTIM, float minimum = 0.0f, float maximum = 0.0f)
    {
        if (next < profile.abilities.size())
            profile.abilities[next++] = {spell, initial, low, high, target, minimum, maximum};
    };
    switch (playerClass)
    {
        case CLASS_WARRIOR:
            if (spec == 0) { add(57789,3000,3000,5000); add(9080,5000,5000,10000); }
            else if (spec == 2) { add(57832,5000,5000,10000,VISAGE_SELF); add(57795,3000,3000,5000); }
            else { add(61490,2000,12000,12000,VISAGE_VICTIM,8.0f); add(57790,3000,3000,5000); }
            break;
        case CLASS_PALADIN:
            add(57798,5000,5000,10000,VISAGE_SELF);
            if (spec == 1) add(57799,2000,5000,10000);
            else { add(57769,2000,0,0,VISAGE_SELF); add(57774,3000,3000,5000); }
            break;
        case CLASS_HUNTER:
            profile.ranged = true;
            add(57589,2000,1000,4000); add(57635,5000,10000,20000,VISAGE_VICTIM,0.0f,4.0f);
            break;
        case CLASS_ROGUE:
            profile.dualWield = true;
            add(57641,5000,5000,10000); add(57640,2000,3000,5000);
            break;
        case CLASS_PRIEST:
            profile.ranged = true;
            if (spec == 2) { add(57778,5000,5000,10000); add(57779,2000,3000,5000); }
            else { add(57777,2000,2000,5000,VISAGE_FRIENDLY); add(57775,4000,4000,6000,VISAGE_FRIENDLY); }
            break;
        case CLASS_DEATH_KNIGHT:
            add(57602,5000,12000,12000,VISAGE_VICTIM,3.0f); add(57599,2000,3000,5000);
            break;
        case CLASS_SHAMAN:
            profile.ranged = spec != 1;
            if (spec == 1) add(57783,2000,3000,5000);
            else if (spec == 2) { add(57802,2000,4000,6000,VISAGE_FRIENDLY); add(57785,4000,4000,6000,VISAGE_FRIENDLY); }
            else { add(57784,5000,5000,10000,VISAGE_SELF); add(57781,2000,3000,5000); }
            break;
        case CLASS_MAGE:
            profile.ranged = true;
            add(57629,5000,5000,10000,VISAGE_SELF); add(57628,2000,3000,5000);
            break;
        case CLASS_WARLOCK:
            profile.ranged = true;
            add(57645,2000,6000,10000); add(57644,3000,3000,5000);
            break;
        case CLASS_DRUID:
            profile.ranged = spec != 1;
            if (spec == 0) { add(57647,2000,3000,5000); add(57648,3000,3000,5000); }
            else if (spec == 1) { profile.form=57655; add(57657,2000,3000,5000); add(57661,3000,3000,5000); }
            else { add(57762,2000,4000,6000,VISAGE_FRIENDLY); add(57765,4000,4000,6000,VISAGE_FRIENDLY); }
            break;
    }
    return profile;
}

static uint32 GetVolazjPlayerSpec(Player* player)
{
    uint32 points[3] = {};
    uint32 const* tabs = GetTalentTabPages(player->getClass());
    for (auto const& entry : player->GetActiveTalents())
    {
        PlayerTalent const& talent = entry.second;
        if (talent.state == PLAYERSPELL_REMOVED || !talent.talentEntry) continue;
        for (uint32 i = 0; i < 3; ++i)
            if (talent.talentEntry->TalentTab == tabs[i]) points[i] += talent.currentRank + 1;
    }
    uint32 spec = 0;
    for (uint32 i = 1; i < 3; ++i) if (points[i] > points[spec]) spec = i;
    return spec;
}

struct npc_volazj_visageAI : public ScriptedAI
{
    npc_volazj_visageAI(Creature* creature) : ScriptedAI(creature) {}
    VolazjProfile m_profile;
    std::array<uint32, 3> m_timers{};
    bool m_configured = false;

    Unit* SelectHealingTarget()
    {
        // Volazj is visible in all Insanity phases and shares their faction.
        // Ordinary friendly selection would let healer clones heal the boss.
        std::list<Creature*> candidates;
        for (uint32 entry = NPC_TWISTED_VISAGE_1; entry <= NPC_TWISTED_VISAGE_5; ++entry)
            GetCreatureListWithEntryInGrid(candidates,m_creature,entry,40.0f);
        Creature* selected = nullptr;
        for (Creature* candidate : candidates)
            if (candidate->IsAlive() && candidate->IsInMap(m_creature) &&
                candidate->GetHealthPercent() < 100.0f &&
                (!selected || candidate->GetHealthPercent() < selected->GetHealthPercent()))
                selected = candidate;
        return selected;
    }

    void Configure(Player* model)
    {
        m_profile = GetVolazjProfile(model->getClass(), GetVolazjPlayerSpec(model));
        for (uint32 i = 0; i < m_timers.size(); ++i) m_timers[i] = m_profile.abilities[i].initial;
        SetRangedMode(m_profile.ranged, m_profile.ranged ? 25.0f : 0.0f,
            m_profile.ranged ? TYPE_FULL_CASTER : TYPE_NONE);
        m_creature->SetCanDualWield(m_profile.dualWield);
        if (m_profile.form) DoCastSpellIfCan(m_creature,m_profile.form,CAST_TRIGGERED);
        m_configured = true;
    }

    void EnterEvadeMode() override
    {
        // A player's death/disconnect may temporarily leave a phase empty.
        // Survivors can join it after clearing their own clones; the boss owns
        // wipe cleanup, so a missing victim must not retire living clones.
        if (InstanceData* instance = m_creature->GetInstanceData())
            if (instance->GetData(TYPE_VOLAZJ) == SPECIAL) return;
        m_creature->ForcedDespawn();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!m_configured) return;
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim()) return;
        for (uint32 i = 0; i < m_timers.size(); ++i)
        {
            VolazjAbility& action = m_profile.abilities[i];
            if (!action.spell) continue;
            if (m_timers[i] > diff) { m_timers[i] -= diff; continue; }
            m_timers[i] = 0;
            if (m_creature->IsNonMeleeSpellCasted(false)) continue;
            Unit* target = action.target == VISAGE_SELF ? m_creature :
                action.target == VISAGE_FRIENDLY ? SelectHealingTarget() : m_creature->GetVictim();
            if (!target || (action.minDistance && m_creature->IsWithinCombatDist(target, action.minDistance)) ||
                (action.maxDistance && !m_creature->IsWithinCombatDist(target, action.maxDistance)))
            { m_timers[i] = 1000; continue; }
            if (DoCastSpellIfCan(target, action.spell) == CAST_OK)
            {
                if (action.repeatMax) m_timers[i] = urand(action.repeatMin,action.repeatMax);
                else action.spell = 0;
            }
            else m_timers[i] = 1000;
        }
        DoMeleeAttackIfReady();
    }
};

/*######
## boss_volazj
######*/

struct boss_volazjAI : public ScriptedAI
{
    boss_volazjAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_ahnkahet*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ahnkahet* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiCombatPhase;
    uint32 m_uiMindFlayTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiShiverTimer;

    uint8 m_uiInsanityIndex;
    bool m_bIsInsanityInProgress;
    std::array<ObjectGuid, MAX_INSANITY_SPELLS> m_insanityPlayers{};
    uint32 m_spawnedVisages = 0;
    uint32 m_insanityCheckTimer = 0;
    bool m_insanityBuilt = false;

    void Reset() override
    {
        m_uiCombatPhase         = 1;
        m_uiMindFlayTimer       = 10000;
        m_uiShadowBoltTimer     = 5000;
        m_uiShiverTimer         = 18000;

        m_uiInsanityIndex       = 0;
        m_bIsInsanityInProgress = false;
        m_insanityPlayers.fill(ObjectGuid());
        m_spawnedVisages = 0;
        m_insanityCheckTimer = 0;
        m_insanityBuilt = false;
        m_creature->SetPhaseMask(1 | 16 | 32 | 64 | 128 | 256, true);

        SetCombatMovement(true);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_WHISPER_AGGRO);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_VOLAZJ, IN_PROGRESS);

        }
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        switch (urand(0, 2))
        {
            case 0:
                DoScriptText(SAY_SLAY_1, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_WHISPER_SLAY_1);
                break;
            case 1:
                DoScriptText(SAY_SLAY_2, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_WHISPER_SLAY_2);
                break;
            case 2:
                DoScriptText(SAY_SLAY_3, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_WHISPER_SLAY_3);
                break;
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (urand(0, 1))
        {
            DoScriptText(SAY_DEATH_1, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_WHISPER_DEATH_1, CAST_TRIGGERED);
        }
        else
        {
            DoScriptText(SAY_DEATH_2, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_WHISPER_DEATH_2, CAST_TRIGGERED);
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLAZJ, DONE);
    }

    void EnterEvadeMode() override
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_VOLAZJ) == SPECIAL)
            return;

        ScriptedAI::EnterEvadeMode();
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLAZJ, FAIL);
    }

    void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellEntry const*) override
    {
        if (m_bIsInsanityInProgress) damage = 0;
    }

    void JustSummoned(Creature* summon) override
    {
        if (!m_bIsInsanityInProgress || summon->GetEntry() < NPC_TWISTED_VISAGE_1 ||
            summon->GetEntry() > NPC_TWISTED_VISAGE_5) return;
        Player* model = m_creature->GetMap()->GetPlayer(summon->GetSpawnerGuid());
        Player* owner = m_creature->GetMap()->GetPlayer(m_insanityPlayers[summon->GetEntry()-NPC_TWISTED_VISAGE_1]);
        npc_volazj_visageAI* ai = dynamic_cast<npc_volazj_visageAI*>(summon->AI());
        if (!model || !owner || !owner->IsAlive() || !ai) { summon->ForcedDespawn(); return; }
        model->CastSpell(summon,SPELL_TWISTED_VISAGE_EFFECT,TRIGGERED_OLD_TRIGGERED);
        summon->CastSpell(summon,SPELL_TWISTED_VISAGE_PASSIVE,TRIGGERED_OLD_TRIGGERED);
        ai->Configure(model);
        summon->SetImmuneToPlayer(false);
        summon->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_UNINTERACTIBLE);
        ai->AttackStart(owner);
        ++m_spawnedVisages;
    }

    void BuildInsanityVisages()
    {
        if (m_insanityBuilt || !m_bIsInsanityInProgress || !m_pInstance || m_pInstance->GetData(TYPE_VOLAZJ) != SPECIAL) return;
        m_insanityBuilt = true;
        for (uint32 phase = 0; phase < m_uiInsanityIndex; ++phase)
        {
            Player* owner = m_creature->GetMap()->GetPlayer(m_insanityPlayers[phase]);
            if (!owner || !owner->IsAlive()) continue;
            if (!owner->HasAura(aInsanityPhaseSpells[phase]))
            {
                m_pInstance->SetData(TYPE_VOLAZJ,FAIL);
                ScriptedAI::EnterEvadeMode();
                return;
            }
            for (uint32 donor = 0; donor < m_uiInsanityIndex; ++donor)
            {
                if (phase == donor) continue; // fight the other party members, not a self-clone
                Player* model = m_creature->GetMap()->GetPlayer(m_insanityPlayers[donor]);
                if (!model || !model->IsAlive()) continue;
                const uint32 before = m_spawnedVisages;
                model->CastSpell(model,aSpawnVisageSpells[phase],TRIGGERED_OLD_TRIGGERED,
                    nullptr,nullptr,m_creature->GetObjectGuid());
                if (m_spawnedVisages == before)
                {
                    // Never strand the party in a partially constructed phase.
                    m_pInstance->SetData(TYPE_VOLAZJ,FAIL);
                    ScriptedAI::EnterEvadeMode();
                    return;
                }
            }
        }
        m_pInstance->UpdateInsanityPhases();
        if (!m_spawnedVisages)
        {
            m_pInstance->SetData(TYPE_VOLAZJ,IN_PROGRESS);
            m_creature->RemoveAurasDueToSpell(SPELL_INSANITY_VISUAL);
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell) override
    {
        if (pSpell->Id == SPELL_INSANITY && pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
        {
            if (m_uiInsanityIndex >= MAX_INSANITY_SPELLS || !pTarget->IsAlive())
                return;
            // Apply this only for the first target hit
            if (!m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))
            {
                DoCastSpellIfCan(m_creature, SPELL_INSANITY_VISUAL, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_WHISPER_INSANITY, CAST_TRIGGERED);

                DoScriptText(SAY_INSANITY, m_creature);

                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                SetCombatMovement(false);

                if (m_pInstance)
                    m_pInstance->SetData(TYPE_VOLAZJ, SPECIAL);

                m_bIsInsanityInProgress = true;
            }

            // Store the players in the instance, in order to better handle phasing
            if (m_pInstance)
                m_pInstance->SetData64(DATA_INSANITY_PLAYER, pTarget->GetObjectGuid());

            // Collect every hit before constructing each phase's clone group.
            m_insanityPlayers[m_uiInsanityIndex] = pTarget->GetObjectGuid();
            pTarget->CastSpell(pTarget, aInsanityPhaseSpells[m_uiInsanityIndex], TRIGGERED_OLD_TRIGGERED, nullptr, nullptr, m_creature->GetObjectGuid());
            ++m_uiInsanityIndex;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Insanity and wipe recovery must progress without a current victim.
        // Check for Insanity
        if (m_bIsInsanityInProgress)
        {
            bool livingParticipant = false;
            for (ObjectGuid guid : m_insanityPlayers)
                if (Player* player = m_creature->GetMap()->GetPlayer(guid))
                    if (player->IsAlive() && !player->IsBeingTeleported()) livingParticipant = true;
            if (!livingParticipant)
            {
                if (m_pInstance) m_pInstance->SetData(TYPE_VOLAZJ,FAIL);
                ScriptedAI::EnterEvadeMode();
                return;
            }
            if (m_insanityCheckTimer <= uiDiff)
            {
                if (m_pInstance) m_pInstance->UpdateInsanityPhases();
                m_insanityCheckTimer = 1000;
            }
            else m_insanityCheckTimer -= uiDiff;
            if (!m_creature->HasAura(SPELL_INSANITY_VISUAL))
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE);
                SetCombatMovement(true);
                m_bIsInsanityInProgress = false;
            }

            // No other actions during insanity
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim()) return;

        if (m_uiCombatPhase <= 2 && m_creature->GetHealthPercent() < (m_uiCombatPhase == 1 ? 66.0f : 33.0f))
        {
            if (DoCastSpellIfCan(m_creature, SPELL_INSANITY) == CAST_OK)
            {
                m_uiInsanityIndex = 0;
                m_spawnedVisages = 0;
                m_insanityBuilt = false;
                m_insanityPlayers.fill(ObjectGuid());
                ++m_uiCombatPhase;
                return;
            }
        }

        if (m_uiMindFlayTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), m_bIsRegularMode ? SPELL_MIND_FLAY : SPELL_MIND_FLAY_H) == CAST_OK)
                m_uiMindFlayTimer = urand(10000, 20000);
        }
        else
            m_uiMindFlayTimer -= uiDiff;

        if (m_uiShadowBoltTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H) == CAST_OK)
                m_uiShadowBoltTimer = urand(8000, 13000);
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        if (m_uiShiverTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SHIVER : SPELL_SHIVER_H) == CAST_OK)
                    m_uiShiverTimer = 30000;
            }
        }
        else
            m_uiShiverTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_boss_volazj(Creature* pCreature)
{
    return new boss_volazjAI(pCreature);
}

struct SummonVolazjVisage : public SpellScript
{
    uint32 GetPhaseMaskOverride(Spell* spell) const override
    {
        const uint32 id = spell->m_spellInfo->Id;
        // Native summon property 1881 ignores the summoner's phase. Without
        // this hook each visage is created in phase 1, outside its player's phase.
        return id >= SPELL_SUMMON_VISAGE_1 && id <= SPELL_SUMMON_VISAGE_5 ?
            (1u << (4 + id - SPELL_SUMMON_VISAGE_1)) : 1u;
    }
};

struct VolazjInsanity : public SpellScript
{
    void OnSuccessfulFinish(Spell* spell) const override
    {
        if (Creature* boss = dynamic_cast<Creature*>(spell->GetCaster()))
            if (boss_volazjAI* ai = dynamic_cast<boss_volazjAI*>(boss->AI())) ai->BuildInsanityVisages();
    }
};

void AddSC_boss_volazj()
{
    RegisterSpellScript<VolazjInsanity>("spell_volazj_insanity");
    Script* visage = new Script;
    visage->Name = "npc_volazj_visage";
    visage->GetAI = &GetNewAIInstance<npc_volazj_visageAI>;
    visage->RegisterSelf();
    RegisterSpellScript<SummonVolazjVisage>("spell_summon_volazj_visage");

    Script* pNewScript = new Script;
    pNewScript->Name = "boss_volazj";
    pNewScript->GetAI = &GetAI_boss_volazj;
    pNewScript->RegisterSelf();
}
