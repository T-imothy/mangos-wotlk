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
SDName: boss_sindragosa
SD%Complete: 80%
SDComment: native Ice Tomb chain and model LOS implemented; complete fight validation remains required
SDCategory: Icecrown Citadel
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "icecrown_citadel.h"
#include "Spells/Scripts/SpellScript.h"
#include "Spells/SpellAuras.h"

enum
{
    SAY_AGGRO                   = -1631148,
    SAY_UNCHAINED_MAGIC         = -1631149,
    SAY_BLISTERING_COLD         = -1631150,
    SAY_RESPIRE                 = -1631151,
    SAY_TAKEOFF                 = -1631152,
    SAY_PHASE_3                 = -1631153,
    SAY_SLAY_1                  = -1631154,
    SAY_SLAY_2                  = -1631155,
    SAY_BERSERK                 = -1631156,
    SAY_DEATH                   = -1631157,

    // Spells

    // Sindragosa

    // all phases
    SPELL_BERSERK               = 26662,

    // Phase 1 and 3
    SPELL_TAIL_SMASH            = 71077,
    SPELL_CLEAVE                = 19983,
    SPELL_FROST_AURA            = 70084,
    SPELL_FROST_BREATH          = 69649,
    SPELL_ICY_GRIP              = 70117,
    SPELL_BLISTERING_COLD       = 70123,
    SPELL_PERMEATING_CHILL      = 70109,
    SPELL_UNCHAINED_MAGIC       = 69762,

    // Phase 2
    SPELL_ICE_TOMB              = 69712, // triggers Frost Beacon on random targets, which triggers actual Ice Tomb after 7 sec.
    SPELL_ICE_TOMB_PROTECTION   = 69700, // protects from taking dmg while in Ice Tomb, should be triggered by Ice Tomb stunning spell
    // Frost Bomb related
    SPELL_FROST_BOMB            = 69846, // summons dummy target npc
    SPELL_FROST_BOMB_DMG        = 69845,
    SPELL_FROST_BOMB_VISUAL     = 70022, // circle mark
// SPELL_FROST_BOMB_OTHER      = 70521, // no idea where it is used, wowhead says it is used by some other Sindragosa (37755)

    // Phase 3
    SPELL_MYSTIC_BUFFET         = 70128,
    SPELL_ICE_TOMB_SINGLE       = 69675,

    // Rimefang
    SPELL_RIMEFANG_FROST_AURA   = 71387,
    SPELL_RIMEFANG_FROST_BREATH = 71386,
    SPELL_RIMEFANG_ICY_BLAST    = 71376,

    // Spinestalker
    SPELL_SPINESTALKER_BELLOWING_ROAR   = 36922,
    SPELL_SPINESTALKER_CLEAVE           = 40505,
    SPELL_SPINESTALKER_TAIL_SWEEP       = 71369
};

enum SindragosaPhase
{
    SINDRAGOSA_PHASE_OOC                = 0,
    SINDRAGOSA_PHASE_AGGRO              = 1,
    SINDRAGOSA_PHASE_GROUND             = 2,
    SINDRAGOSA_PHASE_FLYING_TO_AIR      = 3,
    SINDRAGOSA_PHASE_AIR                = 4,
    SINDRAGOSA_PHASE_FLYING_TO_GROUND   = 5,
    SINDRAGOSA_PHASE_THREE              = 6
};

enum SindragosaPoint
{
    SINDRAGOSA_POINT_GROUND_CENTER      = 0,
    SINDRAGOSA_POINT_AIR_CENTER         = 1,
    SINDRAGOSA_POINT_AIR_PHASE_2        = 2,
    SINDRAGOSA_POINT_AIR_EAST           = 3,
    SINDRAGOSA_POINT_AIR_WEST           = 4
};

enum RimefangPhase
{
    RIMEFANG_PHASE_GROUND               = 0,
    RIMEFANG_PHASE_FLYING               = 1,
    RIMEFANG_PHASE_AIR                  = 2
};

enum RimefangPoint
{
    RIMEFANG_POINT_GROUND               = 0,
    RIMEFANG_POINT_AIR                  = 1,
    RIMEFANG_POINT_INITIAL_LAND_AIR     = 2,
    RIMEFANG_POINT_INITIAL_LAND         = 3
};

enum SpinestalkerPoint
{
    SPINESTALKER_POINT_INITIAL_LAND_AIR = 0,
    SPINESTALKER_POINT_INITIAL_LAND     = 1
};

#define FROST_BOMB_MIN_X 4367.0f
#define FROST_BOMB_MAX_X 4424.0f
#define FROST_BOMB_MIN_Y 2437.0f
#define FROST_BOMB_MAX_Y 2527.0f

static const float SindragosaPosition[10][3] =
{
    {4407.44f, 2484.37f, 203.37f},      // 0 center, ground
    {4407.44f, 2484.37f, 235.37f},      // 1 center, air
    {4470.00f, 2484.37f, 235.37f},      // 2 Sindragosa air phase point
    {4414.32f, 2456.94f, 203.37f},      // 3 Rimefang landing point
    {4414.32f, 2456.94f, 228.37f},      // 4 Rimefang above landing point
    {4414.32f, 2512.73f, 203.37f},      // 5 Spinestalker landing point
    {4414.32f, 2512.73f, 228.37f},      // 6 Spinestalker above landing point
    {4505.00f, 2484.37f, 235.37f},      // 7 Sindragosa spawn point
    {4505.00f, 2444.37f, 235.37f},      // 8 Sindragosa east flying point
    {4505.00f, 2524.37f, 235.37f},      // 9 Sindragosa west flying point
};

struct boss_sindragosaAI : public ScriptedAI
{
    boss_sindragosaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        m_creature->GetCombatManager().SetLeashingCheck([](Unit*, float x, float, float)
        {
            return x < 4314.0f;
        });
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiFrostBreathTimer;
    uint32 m_uiTailSmashTimer;
    uint32 m_uiIcyGripTimer;
    uint32 m_uiBlisteringColdTimer;
    uint32 m_uiUnchainedMagicTimer;
    uint32 m_uiFrostBombTimer;
    uint32 m_uiIceTombSingleTimer;
    bool m_airTombPending;

    void Reset() override
    {
        m_uiPhase                   = SINDRAGOSA_PHASE_OOC;
        m_uiPhaseTimer              = 45000;
        m_uiBerserkTimer            = 10 * MINUTE * IN_MILLISECONDS;
        m_uiCleaveTimer             = urand(5000, 15000);
        m_uiTailSmashTimer          = 20000;
        m_uiFrostBreathTimer        = 5000;
        m_uiIcyGripTimer            = 35000;
        m_uiBlisteringColdTimer     = 0;
        m_uiIceTombSingleTimer      = 15000;
        m_uiUnchainedMagicTimer     = urand(15000, 30000);
        m_airTombPending            = false;
    }

    void SetFlying(bool bIsFlying)
    {
        if (bIsFlying)
            m_creature->SetAnimTier(AnimTier::Hover);
        else
            m_creature->SetAnimTier(AnimTier::Ground);

        m_creature->SetLevitate(bIsFlying);
        m_creature->SetWalk(bIsFlying);
    }

    void EnterEvadeMode() override
    {
        SetFlying(true);
        ScriptedAI::EnterEvadeMode();
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SINDRAGOSA, FAIL);

        m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_EAST, SindragosaPosition[8][0], SindragosaPosition[8][1], SindragosaPosition[8][2]);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void AttackStart(Unit* pWho) override
    {
        ScriptedAI::AttackStart(pWho);

        // on aggro: land first, then start the encounter
        if (m_uiPhase == SINDRAGOSA_PHASE_OOC)
        {
            m_uiPhase = SINDRAGOSA_PHASE_AGGRO;
            SetCombatMovement(false);
            m_creature->SetWalk(true);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_CENTER, SindragosaPosition[1][0], SindragosaPosition[1][1], SindragosaPosition[1][2]);
        }
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoScriptText(SAY_AGGRO, m_creature);
        // instance data set when sindragosa lands
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SINDRAGOSA, DONE);
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (uiMovementType != POINT_MOTION_TYPE || !m_creature->IsAlive())
            return;

        if (uiPointId == SINDRAGOSA_POINT_AIR_EAST && m_uiPhase == SINDRAGOSA_PHASE_OOC)
        {
            m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_WEST, SindragosaPosition[9][0], SindragosaPosition[9][1], SindragosaPosition[9][2]);
        }
        else if (uiPointId == SINDRAGOSA_POINT_AIR_WEST && m_uiPhase == SINDRAGOSA_PHASE_OOC)
        {
            m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_EAST, SindragosaPosition[8][0], SindragosaPosition[8][1], SindragosaPosition[8][2]);
        }
        else if (uiPointId == SINDRAGOSA_POINT_GROUND_CENTER)
        {
            // fly up
            if (m_uiPhase == SINDRAGOSA_PHASE_FLYING_TO_AIR)
            {
                SetFlying(true);
                m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_CENTER, SindragosaPosition[1][0], SindragosaPosition[1][1], SindragosaPosition[1][2]);
            }
            else if (m_uiPhase == SINDRAGOSA_PHASE_AGGRO || m_uiPhase == SINDRAGOSA_PHASE_FLYING_TO_GROUND) // land and attack
            {
                // on aggro, after landing: set instance data and cast initial spells
                if (m_uiPhase == SINDRAGOSA_PHASE_AGGRO)
                {
                    DoCastSpellIfCan(m_creature, SPELL_FROST_AURA, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_PERMEATING_CHILL, CAST_TRIGGERED);

                    if (m_pInstance)
                        m_pInstance->SetData(TYPE_SINDRAGOSA, IN_PROGRESS);
                }

                m_uiPhase = SINDRAGOSA_PHASE_GROUND;
                SetFlying(false);
                SetCombatMovement(true);

                if (Unit* pVictim = m_creature->GetVictim())
                    m_creature->GetMotionMaster()->MoveChase(pVictim);
            }
        }
        else if (uiPointId == SINDRAGOSA_POINT_AIR_CENTER)
        {
            if (m_uiPhase == SINDRAGOSA_PHASE_AGGRO || m_uiPhase == SINDRAGOSA_PHASE_FLYING_TO_GROUND)
            {
                // land
                m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_GROUND_CENTER, SindragosaPosition[0][0], SindragosaPosition[0][1], SindragosaPosition[0][2]);
            }
            else if (m_uiPhase == SINDRAGOSA_PHASE_FLYING_TO_AIR)
            {
                // fly up (air phase)
                m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_PHASE_2, SindragosaPosition[2][0], SindragosaPosition[2][1], SindragosaPosition[2][2]);
            }
        }
        else if (uiPointId == SINDRAGOSA_POINT_AIR_PHASE_2 && m_uiPhase == SINDRAGOSA_PHASE_FLYING_TO_AIR)
        {
            m_creature->SetOrientation(M_PI_F); // face the platform
            m_uiFrostBombTimer = 10000; // set initial Frost Bomb timer
            m_airTombPending = true;
            m_uiPhase = SINDRAGOSA_PHASE_AIR;
        }
    }

    void DoFrostBomb()
    {
        float x = frand(FROST_BOMB_MIN_X, FROST_BOMB_MAX_X);
        float y = frand(FROST_BOMB_MIN_Y, FROST_BOMB_MAX_Y);
        float z = SindragosaPosition[0][2]; // platform height

        m_creature->CastSpell(x, y, z, SPELL_FROST_BOMB, TRIGGERED_NONE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Berserk
        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        if (m_uiPhase == SINDRAGOSA_PHASE_GROUND || m_uiPhase == SINDRAGOSA_PHASE_THREE)
        {
            // Preserve the native Grip -> one-second pause -> Cold cast order.
            // Rejected Cold casts retry instead of consuming the only attempt.
            if (m_uiBlisteringColdTimer)
            {
                if (m_uiBlisteringColdTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_BLISTERING_COLD) == CAST_OK)
                        m_uiBlisteringColdTimer = 0;
                }
                else m_uiBlisteringColdTimer -= uiDiff;
                return;
            }
            if (const Spell* cast = m_creature->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                if (cast->getState() != SPELL_STATE_FINISHED && cast->m_spellInfo &&
                    (cast->m_spellInfo->Id == 70123 || cast->m_spellInfo->Id == 71047 ||
                     cast->m_spellInfo->Id == 71048 || cast->m_spellInfo->Id == 71049)) return;
        }

        switch (m_uiPhase)
        {
            case SINDRAGOSA_PHASE_THREE:
            {
                // Ice Tomb
                if (m_uiIceTombSingleTimer <= uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, SPELL_ICE_TOMB_SINGLE, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_ICE_TOMB_SINGLE) == CAST_OK)
                            m_uiIceTombSingleTimer = 15000;
                    }
                }
                else
                    m_uiIceTombSingleTimer -= uiDiff;

                // no break
            }
            case SINDRAGOSA_PHASE_GROUND:
            {
                // Phase 1 only
                if (m_uiPhase == SINDRAGOSA_PHASE_GROUND)
                {
                    // Health Check
                    if (m_creature->GetHealthPercent() <= 30.0f)
                    {
                        if (DoCastSpellIfCan(m_creature, SPELL_MYSTIC_BUFFET) == CAST_OK)
                        {
                            m_uiPhase = SINDRAGOSA_PHASE_THREE;
                            DoScriptText(SAY_PHASE_3, m_creature);
                        }
                    }

                    // Phase 2 (air)
                    if (m_uiPhase == SINDRAGOSA_PHASE_GROUND && m_uiPhaseTimer <= uiDiff)
                    {
                        m_uiPhase = SINDRAGOSA_PHASE_FLYING_TO_AIR;
                        m_uiPhaseTimer = 33000;
                        DoScriptText(SAY_TAKEOFF, m_creature);
                        SetCombatMovement(false);
                        m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_GROUND_CENTER, SindragosaPosition[0][0], SindragosaPosition[0][1], SindragosaPosition[0][2]);
                        return;
                    }
                    else if (m_uiPhase == SINDRAGOSA_PHASE_GROUND)
                        m_uiPhaseTimer -= uiDiff;
                }

                // Cleave
                if (m_uiCleaveTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                        m_uiCleaveTimer = urand(5000, 15000);
                }
                else
                    m_uiCleaveTimer -= uiDiff;

                // Tail Smash
                if (m_uiTailSmashTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_TAIL_SMASH) == CAST_OK)
                        m_uiTailSmashTimer = urand(10000, 20000);
                }
                else
                    m_uiTailSmashTimer -= uiDiff;

                // Frost Breath
                if (m_uiFrostBreathTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FROST_BREATH) == CAST_OK)
                        m_uiFrostBreathTimer = urand(15000, 20000);
                }
                else
                    m_uiFrostBreathTimer -= uiDiff;

                // Unchained Magic
                if (m_uiUnchainedMagicTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_UNCHAINED_MAGIC) == CAST_OK)
                    {
                        m_uiUnchainedMagicTimer = urand(40000, 60000);
                        DoScriptText(SAY_UNCHAINED_MAGIC, m_creature);
                    }
                }
                else
                    m_uiUnchainedMagicTimer -= uiDiff;

                // Icy Grip and Blistering Cold
                if (m_uiIcyGripTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_ICY_GRIP) == CAST_OK)
                    {
                        m_uiIcyGripTimer = 70000;
                        m_uiBlisteringColdTimer = 1000;
                        DoScriptText(SAY_BLISTERING_COLD, m_creature);
                    }
                }
                else
                    m_uiIcyGripTimer -= uiDiff;

                DoMeleeAttackIfReady();
                break;
            }
            case SINDRAGOSA_PHASE_FLYING_TO_GROUND:
            case SINDRAGOSA_PHASE_FLYING_TO_AIR:
                break;
            case SINDRAGOSA_PHASE_AIR:
            {
                // Start the cover window only after the air-phase selector is
                // accepted. A rejected cast must not consume the sole attempt
                // or start dropping bombs before the beacon/tomb sequence.
                if (m_airTombPending)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_ICE_TOMB) != CAST_OK)
                        return;
                    m_airTombPending = false;
                }

                // Phase One (ground)
                if (m_uiPhaseTimer <= uiDiff)
                {
                    m_uiPhase = SINDRAGOSA_PHASE_FLYING_TO_GROUND;
                    m_uiPhaseTimer = 42000;
                    m_creature->GetMotionMaster()->MovePoint(SINDRAGOSA_POINT_AIR_CENTER, SindragosaPosition[1][0], SindragosaPosition[1][1], SindragosaPosition[1][2]);
                    return;
                }
                else
                    m_uiPhaseTimer -= uiDiff;

                // Frost Bomb
                if (m_uiFrostBombTimer <= uiDiff)
                {
                    DoFrostBomb();
                    m_uiFrostBombTimer = 6000;
                }
                else
                    m_uiFrostBombTimer -= uiDiff;

                break;
            }
        }
    }
};

UnitAI* GetAI_boss_sindragosa(Creature* pCreature)
{
    return new boss_sindragosaAI(pCreature);
}

struct npc_rimefang_iccAI : public ScriptedAI
{
    npc_rimefang_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();

        // Icy Blast - 3 casts on 10man, 6 on 25man
        m_uiIcyBlastMaxCount = 3;
        if (m_pInstance && m_pInstance->Is25ManDifficulty())
            m_uiIcyBlastMaxCount = 6;

        m_bHasLanded = false;
        m_bIsReady = false;

        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiFrostBreathTimer;
    uint32 m_uiIcyBlastCounter;
    uint32 m_uiIcyBlastMaxCount;
    uint32 m_uiIcyBlastTimer;
    bool m_bHasLanded; // landed after player entered areatrigger
    bool m_bIsReady;

    void Reset() override
    {
        m_uiPhase               = RIMEFANG_PHASE_GROUND;
        m_uiPhaseTimer          = 25000;
        m_uiFrostBreathTimer    = urand(5000, 8000);
        m_uiIcyBlastTimer       = 0;
        m_uiIcyBlastCounter     = 0;

        SetCombatMovement(true);
    }

    void SetFlying(bool bIsFlying)
    {
        if (bIsFlying)
            m_creature->SetAnimTier(AnimTier::Hover);
        else
            m_creature->SetAnimTier(AnimTier::Ground);

        m_creature->SetLevitate(bIsFlying);
        m_creature->SetWalk(bIsFlying);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        DoCastSpellIfCan(m_creature, SPELL_RIMEFANG_FROST_AURA, CAST_TRIGGERED);
    }

    void AttackStart(Unit* pWho) override
    {
        if (!m_bIsReady)
        {
            if (!m_bHasLanded)
            {
                m_bHasLanded = true;
                m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_INITIAL_LAND_AIR, SindragosaPosition[4][0], SindragosaPosition[4][1], SindragosaPosition[4][2]);
            }

            return;
        }

        ScriptedAI::AttackStart(pWho);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_pInstance)
            return;

        Creature* pSpinestalker = m_pInstance->GetSingleCreatureFromStorage(NPC_SPINESTALKER);
        if (!pSpinestalker || !pSpinestalker->IsAlive())
        {
            if (Creature* pSindragosa = m_creature->SummonCreature(NPC_SINDRAGOSA, SindragosaPosition[7][0], SindragosaPosition[7][1], SindragosaPosition[7][2], 0.0f, TEMPSPAWN_MANUAL_DESPAWN, 0))
                pSindragosa->SetInCombatWithZone();
        }
    }

    // evade to point on platform
    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);

        if (m_creature->IsAlive())
            m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_INITIAL_LAND, SindragosaPosition[3][0], SindragosaPosition[3][1], SindragosaPosition[3][2]);

        m_creature->SetLootRecipient(nullptr);

        Reset();
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == RIMEFANG_POINT_INITIAL_LAND_AIR)
        {
            m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_INITIAL_LAND, SindragosaPosition[3][0], SindragosaPosition[3][1], SindragosaPosition[3][2]);
        }
        else if (uiPointId == RIMEFANG_POINT_INITIAL_LAND)
        {
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetFacingTo(M_PI_F);
            m_bIsReady = true;
            SetFlying(false);
        }
        else if (uiPointId == RIMEFANG_POINT_GROUND)
        {
            m_uiPhase = RIMEFANG_PHASE_GROUND;
            SetFlying(false);
            SetCombatMovement(true);

            if (Unit* pVictim = m_creature->GetVictim())
                m_creature->GetMotionMaster()->MoveChase(pVictim);
        }
        else if (uiPointId == RIMEFANG_POINT_AIR)
        {
            m_uiPhase = RIMEFANG_PHASE_AIR;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiPhase == RIMEFANG_PHASE_GROUND)
        {
            // Frost Breath
            if (m_uiFrostBreathTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_RIMEFANG_FROST_BREATH) == CAST_OK)
                    m_uiFrostBreathTimer = urand(5000, 8000);
            }
            else
                m_uiFrostBreathTimer -= uiDiff;

            // Icy Blast - air phase
            if (m_uiPhaseTimer <= uiDiff)
            {
                m_uiPhaseTimer = 40000;
                m_uiPhase = RIMEFANG_PHASE_FLYING;
                SetFlying(true);
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_AIR, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 20.0f);
                return;
            }
            m_uiPhaseTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
        else if (m_uiPhase == RIMEFANG_PHASE_AIR)
        {
            // Icy Blast
            if (m_uiIcyBlastTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_RIMEFANG_ICY_BLAST, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_RIMEFANG_ICY_BLAST) == CAST_OK)
                    {
                        m_uiIcyBlastTimer = 3000;
                        ++m_uiIcyBlastCounter;

                        // phase end
                        if (m_uiIcyBlastCounter >= m_uiIcyBlastMaxCount)
                        {
                            m_uiIcyBlastCounter = 0;
                            m_uiIcyBlastTimer = 0;
                            m_uiPhase = RIMEFANG_PHASE_FLYING;
                            m_creature->GetMotionMaster()->MovePoint(RIMEFANG_POINT_GROUND, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() - 20.0f);
                        }
                    }
                }
            }
            else
                m_uiIcyBlastTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_rimefang_icc(Creature* pCreature)
{
    return new npc_rimefang_iccAI(pCreature);
}


struct npc_spinestalker_iccAI : public ScriptedAI
{
    npc_spinestalker_iccAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        m_bHasLanded = false;
        m_bIsReady = false;
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;

    uint32 m_uiBellowingRoarTimer;
    uint32 m_uiTailSweepTimer;
    uint32 m_uiCleaveTimer;
    bool m_bHasLanded;
    bool m_bIsReady;

    void Reset() override
    {
        m_uiBellowingRoarTimer  = urand(8000, 24000);
        m_uiTailSweepTimer      = urand(4000, 8000);
        m_uiCleaveTimer         = urand(5000, 8000);
    }

    void SetFlying(bool bIsFlying)
    {
        if (bIsFlying)
            m_creature->SetAnimTier(AnimTier::Hover);
        else
            m_creature->SetAnimTier(AnimTier::Ground);

        m_creature->SetLevitate(bIsFlying);
        m_creature->SetWalk(bIsFlying);
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        if (!m_pInstance)
            return;

        Creature* pRimefang = m_pInstance->GetSingleCreatureFromStorage(NPC_RIMEFANG);
        if (!pRimefang || !pRimefang->IsAlive())
        {
            if (Creature* pSindragosa = m_creature->SummonCreature(NPC_SINDRAGOSA, SindragosaPosition[7][0], SindragosaPosition[7][1], SindragosaPosition[7][2], 0.0f, TEMPSPAWN_MANUAL_DESPAWN, 0))
                pSindragosa->SetInCombatWithZone();
        }
    }

    void AttackStart(Unit* pWho) override
    {
        if (!m_bIsReady)
        {
            if (!m_bHasLanded)
            {
                m_bHasLanded = true;
                m_creature->GetMotionMaster()->MovePoint(SPINESTALKER_POINT_INITIAL_LAND_AIR, SindragosaPosition[6][0], SindragosaPosition[6][1], SindragosaPosition[6][2]);
            }

            return;
        }

        ScriptedAI::AttackStart(pWho);
    }

    void EnterEvadeMode() override
    {
        m_creature->RemoveAllAurasOnEvade();
        m_creature->CombatStop(true);

        if (m_creature->IsAlive())
            m_creature->GetMotionMaster()->MovePoint(SPINESTALKER_POINT_INITIAL_LAND, SindragosaPosition[5][0], SindragosaPosition[5][1], SindragosaPosition[5][2]);

        m_creature->SetLootRecipient(nullptr);

        Reset();
    }

    void MovementInform(uint32 uiMovementType, uint32 uiPointId) override
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == SPINESTALKER_POINT_INITIAL_LAND_AIR)
        {
            m_creature->GetMotionMaster()->MovePoint(SPINESTALKER_POINT_INITIAL_LAND, SindragosaPosition[5][0], SindragosaPosition[5][1], SindragosaPosition[5][2]);
        }
        else if (uiPointId == SPINESTALKER_POINT_INITIAL_LAND)
        {
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->SetFacingTo(M_PI_F);
            m_bIsReady = true;
            SetFlying(false);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_SPINESTALKER_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(5000, 8000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Tail Sweep
        if (m_uiTailSweepTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SPINESTALKER_TAIL_SWEEP) == CAST_OK)
                m_uiTailSweepTimer = urand(4000, 8000);
        }
        else
            m_uiTailSweepTimer -= uiDiff;

        // Bellowing Roar
        if (m_uiBellowingRoarTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SPINESTALKER_BELLOWING_ROAR) == CAST_OK)
                m_uiBellowingRoarTimer = urand(8000, 24000);
        }
        else
            m_uiBellowingRoarTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

UnitAI* GetAI_npc_spinestalker_icc(Creature* pCreature)
{
    return new npc_spinestalker_iccAI(pCreature);
}

/**
 * Frost Bomb - npc marking the target of Frost Bomb
 */
struct mob_frost_bombAI : public ScriptedAI
{
    mob_frost_bombAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_icecrown_citadel*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_citadel* m_pInstance;
    uint32 m_uiFrostBombTimer;

    void Reset() override
    {
        SetCombatMovement(false);
        DoCastSpellIfCan(m_creature, SPELL_FROST_BOMB_VISUAL, CAST_TRIGGERED);
        m_uiFrostBombTimer = 6000;
    }

    void AttackStart(Unit* /*pWho*/) override {}

    void UpdateAI(const uint32 uiDiff) override
    {
        // Frost Bomb (dmg)
        if (m_uiFrostBombTimer)
        {
            if (m_uiFrostBombTimer <= uiDiff)
            {
                Creature* sindragosa = m_pInstance ? m_pInstance->GetSingleCreatureFromStorage(NPC_SINDRAGOSA) : nullptr;
                if (!sindragosa || !sindragosa->IsAlive() || m_pInstance->GetData(TYPE_SINDRAGOSA) != IN_PROGRESS)
                {
                    m_uiFrostBombTimer = 0;
                    m_creature->ForcedDespawn();
                    return;
                }

                // The payload is caster-centered. Its marker must supply the origin
                // and line of sight, while Sindragosa retains damage attribution.
                if (m_creature->CastSpell(m_creature, SPELL_FROST_BOMB_DMG, TRIGGERED_OLD_TRIGGERED,
                        nullptr, nullptr, sindragosa->GetObjectGuid()) == SPELL_CAST_OK)
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_FROST_BOMB_VISUAL);
                    m_creature->ForcedDespawn(2000);
                    m_uiFrostBombTimer = 0;
                }
            }
            else
                m_uiFrostBombTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_mob_frost_bomb(Creature* pCreature)
{
    return new mob_frost_bombAI(pCreature);
}

// 36980 - Ice Tomb. The creature is attackable; the closed door supplies native LOS.
struct npc_sindragosa_ice_tombAI : public Scripted_NoMovementAI
{
    npc_sindragosa_ice_tombAI(Creature* creature) : Scripted_NoMovementAI(creature), m_checkTimer(1000) {}

    ObjectGuid m_prisonerGuid;
    ObjectGuid m_blockGuid;
    uint32 m_checkTimer;

    void Reset() override {}
    void AttackStart(Unit* /*target*/) override {}

    bool Initialize(Player* prisoner)
    {
        Map* map = m_creature->GetMap();
        GameObject* block = new GameObject;
        if (!block->Create(0, map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), 201722, map,
                prisoner->GetPhaseMask(), prisoner->GetPositionX(), prisoner->GetPositionY(),
                prisoner->GetPositionZ(), prisoner->GetOrientation()))
        {
            delete block;
            return false;
        }

        map->Add(block);
        block->AIM_Initialize();
        m_blockGuid = block->GetObjectGuid();
        m_prisonerGuid = prisoner->GetObjectGuid();
        return true;
    }

    void Release()
    {
        // Clear ownership before removing auras, which may invoke their own callbacks.
        ObjectGuid prisonerGuid = m_prisonerGuid;
        m_prisonerGuid.Clear();
        if (Player* prisoner = m_creature->GetMap()->GetPlayer(prisonerGuid))
        {
            prisoner->RemoveAurasDueToSpell(70157);
            prisoner->RemoveAurasDueToSpell(SPELL_ICE_TOMB_PROTECTION);
            prisoner->RemoveAurasDueToSpell(71665);
        }
        if (GameObject* block = m_creature->GetMap()->GetGameObject(m_blockGuid))
            block->Delete();
        m_blockGuid.Clear();
    }

    void JustDied(Unit* /*killer*/) override { Release(); }
    void SummonedCreatureDespawn(Creature* summon) override
    {
        if (summon == m_creature)
            Release();
    }

    void UpdateAI(uint32 diff) override
    {
        if (m_checkTimer > diff)
        {
            m_checkTimer -= diff;
            return;
        }
        m_checkTimer = 1000;

        Player* prisoner = m_creature->GetMap()->GetPlayer(m_prisonerGuid);
        instance_icecrown_citadel* instance = dynamic_cast<instance_icecrown_citadel*>(m_creature->GetInstanceData());
        if (!prisoner || !prisoner->IsAlive() || !prisoner->HasAura(70157) ||
                !instance || instance->GetData(TYPE_SINDRAGOSA) != IN_PROGRESS)
        {
            Release();
            m_creature->ForcedDespawn();
            return;
        }

        // Air-phase prisoners start suffocating after the boss lands.
        Creature* boss = instance->GetSingleCreatureFromStorage(NPC_SINDRAGOSA);
        boss_sindragosaAI* ai = boss ? dynamic_cast<boss_sindragosaAI*>(boss->AI()) : nullptr;
        if (ai && ai->m_uiPhase == SINDRAGOSA_PHASE_GROUND && !prisoner->HasAura(71665))
            prisoner->CastSpell(prisoner, 71665, TRIGGERED_OLD_TRIGGERED);
    }
};

UnitAI* GetAI_npc_sindragosa_ice_tomb(Creature* creature)
{
    return new npc_sindragosa_ice_tombAI(creature);
}

// 69712 (air selector), 69675 (explicit final-phase target).
struct spell_sindragosa_ice_tomb_selector : public SpellScript
{
    void OnInit(Spell* spell) const override
    {
        if (spell->m_spellInfo->Id != SPELL_ICE_TOMB || !spell->GetCaster())
            return;
        Difficulty difficulty = spell->GetCaster()->GetMap()->GetDifficulty();
        spell->SetMaxAffectedTargets(difficulty == RAID_DIFFICULTY_25MAN_HEROIC ? 6 :
            difficulty == RAID_DIFFICULTY_25MAN_NORMAL ? 5 : 2);
    }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*effect*/) const override
    {
        Unit* caster = spell->GetCaster();
        return caster && target && target->IsPlayer() && target->IsAlive() && target != caster->GetVictim() &&
            !target->HasAura(70126) && !target->HasAura(70157);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effect) const override
    {
        if (effect == EFFECT_INDEX_0 && spell->GetCaster() && spell->GetUnitTarget())
            spell->GetCaster()->CastSpell(spell->GetUnitTarget(), 70126, TRIGGERED_OLD_TRIGGERED);
    }
};

// 70126 - The DBC's server-side trigger 70159 is supplied by this native callback.
struct spell_sindragosa_frost_beacon : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        data.spellInfo = nullptr;
        Unit* caster = aura->GetCaster();
        Unit* target = aura->GetTarget();
        instance_icecrown_citadel* instance = caster ? dynamic_cast<instance_icecrown_citadel*>(caster->GetInstanceData()) : nullptr;
        if (!caster || !caster->IsAlive() || !target || !target->IsAlive() ||
                !instance || instance->GetData(TYPE_SINDRAGOSA) != IN_PROGRESS)
            return;
        data.trueCaster = caster;
        data.caster = caster;
        data.target = target;
        data.targetObject = target;
        data.spellInfo = sSpellTemplate.LookupEntry<SpellEntry>(70157);
    }
};

// 70157 - One second after the native stun, create the attackable prison and LOS object.
struct spell_sindragosa_ice_tomb_trap : public AuraScript, public SpellScript
{
    bool OnCheckTarget(const Spell* /*spell*/, Unit* target, SpellEffectIndex /*effect*/) const override
    {
        // Overlapping beacons must not refresh a prison and create a second
        // tomb whose death could release another tomb's prisoner.
        return target && target->IsPlayer() && target->IsAlive() && !target->HasAura(70157);
    }

    void OnPeriodicTrigger(Aura* aura, PeriodicTriggerData& data) const override
    {
        data.spellInfo = nullptr;
        Unit* target = aura->GetTarget();
        Creature* caster = dynamic_cast<Creature*>(aura->GetCaster());
        instance_icecrown_citadel* instance = caster ? dynamic_cast<instance_icecrown_citadel*>(caster->GetInstanceData()) : nullptr;
        if (!target || !target->IsPlayer())
            return;

        if (caster && caster->IsAlive() && target->IsAlive() && caster->GetMap() == target->GetMap() &&
                instance && instance->GetData(TYPE_SINDRAGOSA) == IN_PROGRESS)
        {
            if (aura->GetAuraTicks() != 1)
                return;
            if (Creature* tomb = caster->SummonCreature(36980, target->GetPositionX(), target->GetPositionY(),
                    target->GetPositionZ(), target->GetOrientation(), TEMPSPAWN_DEAD_DESPAWN, 0))
            {
                npc_sindragosa_ice_tombAI* ai = dynamic_cast<npc_sindragosa_ice_tombAI*>(tomb->AI());
                if (ai && ai->Initialize(static_cast<Player*>(target)))
                {
                    target->CastSpell(target, SPELL_ICE_TOMB_PROTECTION, TRIGGERED_OLD_TRIGGERED);
                    return;
                }
                tomb->ForcedDespawn();
            }
        }
        // A failed summon must never leave an unbreakable permanent stun.
        target->RemoveAurasDueToSpell(70157);
    }

    void OnApply(Aura* aura, bool apply) const override
    {
        if (!apply && aura->GetEffIndex() == EFFECT_INDEX_2)
        {
            aura->GetTarget()->RemoveAurasDueToSpell(SPELL_ICE_TOMB_PROTECTION);
            aura->GetTarget()->RemoveAurasDueToSpell(71665);
        }
    }
};

// Frost Bomb and Mystic Buffet must include the Ice Block model in LOS tests.
// Ordinary spell targeting ignores M2 models, including this temporary cover.
struct spell_sindragosa_ice_block_los : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*effect*/) const override
    {
        Unit* caster = spell->GetCaster();
        return caster && target && caster->IsWithinLOSInMap(target, false);
    }
};

// 70117 - Icy Grip uses the native player jump spell, preserving its movement checks.
struct spell_sindragosa_icy_grip : public SpellScript
{
    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*effect*/) const override
    {
        return spell->GetCaster() && target && target->IsPlayer() && target->IsAlive() &&
            target != spell->GetCaster()->GetVictim() && !target->HasAura(70126) && !target->HasAura(70157);
    }

    void OnEffectExecute(Spell* spell, SpellEffectIndex effect) const override
    {
        if (effect == EFFECT_INDEX_0 && spell->GetCaster() && spell->GetUnitTarget())
            spell->GetUnitTarget()->CastSpell(spell->GetCaster(), 70122, TRIGGERED_OLD_TRIGGERED);
    }
};

// 69766 - Instability: only natural expiration releases the accumulated Backlash.
struct spell_sindragosa_instability : public AuraScript
{
    void OnApply(Aura* aura, bool apply) const override
    {
        if (apply || aura->GetEffIndex() != EFFECT_INDEX_0 || aura->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            return;
        Unit* target = aura->GetTarget();
        Unit* caster = aura->GetCaster();
        if (!target || !target->IsAlive() || !caster || !caster->IsAlive() || target->GetMap() != caster->GetMap())
            return;
        instance_icecrown_citadel* instance = dynamic_cast<instance_icecrown_citadel*>(target->GetInstanceData());
        Creature* boss = instance ? instance->GetSingleCreatureFromStorage(NPC_SINDRAGOSA) : nullptr;
        if (!boss || !boss->IsAlive() || !boss->IsInCombat() || instance->GetData(TYPE_SINDRAGOSA) != IN_PROGRESS)
            return;
        int32 damage = aura->GetAmount();
        if (damage > 0)
            target->CastCustomSpell(target, 69770, &damage, nullptr, nullptr, TRIGGERED_OLD_TRIGGERED,
                nullptr, aura, aura->GetCasterGuid());
    }
};

// Classify the active native talent allocation without depending on either bot module.
// 1 = healer, 2 = spell damage, 0 = an ineligible melee/ranged-weapon specialization.
static uint32 SindragosaUnchainedRole(Player* player)
{
    if (player->getClass() == CLASS_MAGE || player->getClass() == CLASS_WARLOCK) return 2;
    std::map<uint32, uint32> points;
    for (const auto& entry : player->GetActiveTalents())
    {
        const PlayerTalent& talent = entry.second;
        if (talent.state != PLAYERSPELL_REMOVED && talent.talentEntry)
            points[talent.talentEntry->TalentTab] += talent.currentRank + 1;
    }
    uint32 tab = 0, most = 0;
    for (const auto& tree : points)
        if (tree.second > most) { tab = tree.first; most = tree.second; }
    if (tab == 201 || tab == 202 || tab == 382 || tab == 262 || tab == 282) return 1;
    if (player->getClass() == CLASS_PRIEST ||
        (player->getClass() == CLASS_SHAMAN && tab != 263) ||
        (player->getClass() == CLASS_DRUID && tab != 281)) return 2;
    return 0;
}

// 69762 - Select up to 1/3 healers and fill the 2/6 total from spell damage.
// Rank candidates using a per-cast seed, so repeated effect/target checks are stable.
struct spell_sindragosa_unchained_magic : public SpellScript
{
    void OnInit(Spell* spell) const override { spell->SetScriptValue(urand(0, UINT32_MAX)); }

    bool OnCheckTarget(const Spell* spell, Unit* target, SpellEffectIndex /*effect*/) const override
    {
        Unit* caster = spell->GetCaster();
        if (!caster || !target || !target->IsPlayer()) return false;
        auto eligible = [caster](Player* player)
        {
            return player && player->IsAlive() && !player->IsGameMaster() && !player->IsBeingTeleported() &&
                caster->IsWithinDistInMap(player, 200.0f) && !player->HasAura(70157);
        };
        Player* player = static_cast<Player*>(target);
        if (!eligible(player)) return false;
        const uint32 role = SindragosaUnchainedRole(player);
        if (!role) return false;
        auto rank = [spell](Player* candidate)
        {
            uint64 value = candidate->GetObjectGuid().GetRawValue() ^ spell->GetScriptValue();
            value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
            value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
            return value ^ (value >> 31);
        };
        uint32 healers = 0, ahead = 0;
        const uint64 priority = rank(player);
        for (const auto& reference : caster->GetMap()->GetPlayers())
        {
            Player* candidate = reference.getSource();
            if (!eligible(candidate)) continue;
            const uint32 candidateRole = SindragosaUnchainedRole(candidate);
            if (candidateRole == 1) ++healers;
            if (candidateRole == role && rank(candidate) < priority) ++ahead;
        }
        const Difficulty difficulty = caster->GetMap()->GetDifficulty();
        const bool large = difficulty == RAID_DIFFICULTY_25MAN_NORMAL || difficulty == RAID_DIFFICULTY_25MAN_HEROIC;
        const uint32 healerLimit = large ? 3 : 1;
        return ahead < (role == 1 ? healerLimit : (large ? 6u : 2u) - std::min(healers, healerLimit));
    }
};

void AddSC_boss_sindragosa()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_sindragosa";
    pNewScript->GetAI = &GetAI_boss_sindragosa;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rimefang_icc";
    pNewScript->GetAI = &GetAI_npc_rimefang_icc;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spinestalker_icc";
    pNewScript->GetAI = &GetAI_npc_spinestalker_icc;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_frost_bomb";
    pNewScript->GetAI = &GetAI_mob_frost_bomb;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_sindragosa_ice_tomb";
    pNewScript->GetAI = &GetAI_npc_sindragosa_ice_tomb;
    pNewScript->RegisterSelf();

    RegisterSpellScript<spell_sindragosa_icy_grip>("spell_sindragosa_icy_grip");
    RegisterSpellScript<spell_sindragosa_instability>("spell_sindragosa_instability");
    RegisterSpellScript<spell_sindragosa_unchained_magic>("spell_sindragosa_unchained_magic");
    RegisterSpellScript<spell_sindragosa_ice_tomb_selector>("spell_sindragosa_ice_tomb_selector");
    RegisterSpellScript<spell_sindragosa_frost_beacon>("spell_sindragosa_frost_beacon");
    RegisterSpellScript<spell_sindragosa_ice_tomb_trap>("spell_sindragosa_ice_tomb_trap");
    RegisterSpellScript<spell_sindragosa_ice_block_los>("spell_sindragosa_ice_block_los");
}
