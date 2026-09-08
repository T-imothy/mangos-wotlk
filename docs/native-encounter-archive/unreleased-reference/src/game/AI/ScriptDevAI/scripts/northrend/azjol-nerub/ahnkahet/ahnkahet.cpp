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
SDName: instance_ahnkahet
SD%Complete: 75
SDComment:
SDCategory: Ahn'kahet
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "ahnkahet.h"
#include "Entities/TemporarySpawn.h"
#include "Spells/SpellAuras.h"

instance_ahnkahet::instance_ahnkahet(Map* pMap) : ScriptedInstance(pMap),
    m_bRespectElders(false),
    m_bVolunteerWork(false),
    m_uiDevicesActivated(0),
    m_uiInitiatesKilled(0),
    m_uiTwistedVisageCount(0)
{
    Initialize();
}

void instance_ahnkahet::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_ahnkahet::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ELDER_NADOX:
        case NPC_TALDARAM:
        case NPC_JEDOGA_SHADOWSEEKER:
        case NPC_HERALD_VOLAZJ:
            m_npcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_AHNKAHAR_GUARDIAN_EGG:
            m_GuardianEggList.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_AHNKAHAR_SWARM_EGG:
            m_SwarmerEggList.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_JEDOGA_CONTROLLER:
            // Sort the controllers based on their purpose
            if (pCreature->GetPositionZ() > 30.0f)
                // Used for Taldaram visual
                m_lJedogaControllersGuidList.push_back(pCreature->GetObjectGuid());
            else if (pCreature->GetPositionZ() > 20.0f)
                // Used for Jedoga visual
                m_lJedogaEventControllersGuidList.push_back(pCreature->GetObjectGuid());
            else if (pCreature->GetPositionZ() < -16.0f)
                // Used for Jedoga sacrifice
                m_jedogaSacrificeController = pCreature->GetObjectGuid();
            break;
        case NPC_TWISTED_VISAGE_1:
        case NPC_TWISTED_VISAGE_2:
        case NPC_TWISTED_VISAGE_3:
        case NPC_TWISTED_VISAGE_4:
        case NPC_TWISTED_VISAGE_5:
            if (GetData(TYPE_VOLAZJ) == SPECIAL && m_insanityVisages.insert(pCreature->GetObjectGuid()).second)
                m_uiTwistedVisageCount = m_insanityVisages.size();
            break;
    }
}

void instance_ahnkahet::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_DOOR_TALDARAM:
            if (m_auiEncounter[TYPE_TALDARAM] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_VORTEX:
            if (m_auiEncounter[TYPE_TALDARAM] == SPECIAL)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;

        case GO_ANCIENT_DEVICE_L:
        case GO_ANCIENT_DEVICE_R:
            if (m_auiEncounter[TYPE_NADOX] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;

        default:
            return;
    }
    m_goEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_ahnkahet::SetData(uint32 uiType, uint32 uiData)
{
    debug_log("SD2: Instance Ahn'Kahet: SetData received for type %u with data %u", uiType, uiData);

    switch (uiType)
    {
        case TYPE_NADOX:
            m_auiEncounter[uiType] = uiData;
            if (uiData == IN_PROGRESS)
                m_bRespectElders = true;
            else if (uiData == SPECIAL)
                m_bRespectElders = false;
            else if (uiData == DONE)
            {
                DoToggleGameObjectFlags(GO_ANCIENT_DEVICE_L, GO_FLAG_NO_INTERACT, false);
                DoToggleGameObjectFlags(GO_ANCIENT_DEVICE_R, GO_FLAG_NO_INTERACT, false);
            }
            break;
        case TYPE_TALDARAM:
            if (uiData == SPECIAL)
            {
                ++m_uiDevicesActivated;

                if (m_uiDevicesActivated == 2)
                {
                    m_auiEncounter[uiType] = uiData;
                    DoUseDoorOrButton(GO_VORTEX);

                    // Lower Taldaram
                    if (Creature* pTaldaram = GetSingleCreatureFromStorage(NPC_TALDARAM))
                        pTaldaram->GetMotionMaster()->MovePoint(1, aTaldaramLandingLoc[0], aTaldaramLandingLoc[1], aTaldaramLandingLoc[2]);

                    // Interrupt the channeling
                    for (GuidList::const_iterator itr = m_lJedogaControllersGuidList.begin(); itr != m_lJedogaControllersGuidList.end(); ++itr)
                    {
                        if (Creature* pTemp = instance->GetCreature(*itr))
                            pTemp->InterruptNonMeleeSpells(false);
                    }
                }
            }
            else if (uiData == DONE)
            {
                m_auiEncounter[uiType] = uiData;
                DoUseDoorOrButton(GO_DOOR_TALDARAM);
            }
            break;
        case TYPE_JEDOGA:
            m_auiEncounter[uiType] = uiData;
            if (uiData == IN_PROGRESS)
                m_bVolunteerWork = true;
            else if (uiData == SPECIAL)
                m_bVolunteerWork = false;
            break;
        case TYPE_AMANITAR:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_VOLAZJ:
        {
            const uint32 previous = m_auiEncounter[uiType];
            m_auiEncounter[uiType] = uiData;
            if (uiData != SPECIAL)
            {
                HandleInsanityClear();
                // Detach before despawning: native callbacks can be immediate.
                std::set<ObjectGuid> oldVisages;
                oldVisages.swap(m_insanityVisages);
                m_uiTwistedVisageCount = 0;
                m_lInsanityPlayersGuidList.clear();
                for (ObjectGuid guid : oldVisages)
                    if (Creature* visage = instance->GetCreature(guid))
                        visage->ForcedDespawn();
            }
            // Returning from an Insanity phase is not a fresh timed attempt.
            if (uiData == IN_PROGRESS && previous != SPECIAL && previous != IN_PROGRESS)
                instance->StartEventForAllPlayersInMap(ACHIEV_START_VOLAZJ_ID, nullptr);
            break;
        }

        default:
            script_error_log("Instance Ahn'Kahet: ERROR SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
            break;
    }

    // For some encounters Special data needs to be saved
    if (uiData == DONE || (uiData == SPECIAL && uiType == TYPE_TALDARAM))
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3]
                   << " " << m_auiEncounter[4];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_ahnkahet::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_TWILIGHT_INITIATE:
            ++m_uiInitiatesKilled;

            // If all initiates are killed, then land Jedoga and stop the channeling
            if (m_uiInitiatesKilled == MAX_INITIATES)
            {
                if (Creature* pJedoga = GetSingleCreatureFromStorage(NPC_JEDOGA_SHADOWSEEKER))
                    pJedoga->GetMotionMaster()->MovePoint(1, aJedogaLandingLoc[0], aJedogaLandingLoc[1], aJedogaLandingLoc[2]);

                for (GuidList::const_iterator itr = m_lJedogaEventControllersGuidList.begin(); itr != m_lJedogaEventControllersGuidList.end(); ++itr)
                {
                    if (Creature* pTemp = instance->GetCreature(*itr))
                        pTemp->InterruptNonMeleeSpells(false);
                }
            }

            break;
        case NPC_TWISTED_VISAGE_1:
        case NPC_TWISTED_VISAGE_2:
        case NPC_TWISTED_VISAGE_3:
        case NPC_TWISTED_VISAGE_4:
        case NPC_TWISTED_VISAGE_5:
            FinishInsanityVisage(pCreature, true);
            break;
    }
}

void instance_ahnkahet::OnCreatureEvade(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_TWISTED_VISAGE_1:
        case NPC_TWISTED_VISAGE_2:
        case NPC_TWISTED_VISAGE_3:
        case NPC_TWISTED_VISAGE_4:
        case NPC_TWISTED_VISAGE_5:
            FinishInsanityVisage(pCreature, false);

            pCreature->ForcedDespawn();
            break;
    }
}

void instance_ahnkahet::OnCreatureDespawn(Creature* creature)
{
    FinishInsanityVisage(creature, false);
}

void instance_ahnkahet::FinishInsanityVisage(Creature* creature, bool died)
{
    if (!creature || GetData(TYPE_VOLAZJ) != SPECIAL ||
        !m_insanityVisages.erase(creature->GetObjectGuid()))
        return;
    m_uiTwistedVisageCount = m_insanityVisages.size();
    if (died)
        creature->CastSpell(creature, SPELL_TWISTED_VISAGE_DEATH, TRIGGERED_OLD_TRIGGERED);
    if (m_insanityVisages.empty())
    {
        if (Creature* boss = GetSingleCreatureFromStorage(NPC_HERALD_VOLAZJ))
        {
            boss->CastSpell(boss, SPELL_INSANITY_CLEAR, TRIGGERED_OLD_TRIGGERED);
            boss->RemoveAurasDueToSpell(57561); // only the Insanity visual/channel
        }
        SetData(TYPE_VOLAZJ, IN_PROGRESS);
    }
    else
    {
        // A phase contains several clones. Their spawner is the copied player,
        // not the phase owner, so phase completion cannot use the spawner GUID.
        const uint32 phase = SPELL_INSANITY_PHASE_16 + creature->GetEntry() - NPC_TWISTED_VISAGE_1;
        if (!HasInsanityVisage(phase))
            for (auto const& reference : instance->GetPlayers())
                if (Player* player = reference.getSource())
                    if (player->HasAura(phase)) { HandleInsanitySwitch(player); break; }
    }
}

void instance_ahnkahet::SetData64(uint32 uiData, uint64 uiGuid)
{
    // Store all the players hit by the insanity spell in order to use them for the phasing switch / clear
    if (uiData == DATA_INSANITY_PLAYER)
    {
        if (Player* pPlayer = instance->GetPlayer(ObjectGuid(uiGuid)))
            m_lInsanityPlayersGuidList.push_back(pPlayer->GetObjectGuid());
    }
}

ObjectGuid instance_ahnkahet::SelectRandomGuardianEggGuid()
{
    if (m_GuardianEggList.empty())
        return ObjectGuid();

    GuidList::iterator iter = m_GuardianEggList.begin();
    advance(iter, urand(0, m_GuardianEggList.size() - 1));

    return *iter;
}

ObjectGuid instance_ahnkahet::SelectRandomSwarmerEggGuid()
{
    if (m_SwarmerEggList.empty())
        return ObjectGuid();

    GuidList::iterator iter = m_SwarmerEggList.begin();
    advance(iter, urand(0, m_SwarmerEggList.size() - 1));

    return *iter;
}

static uint32 GetVolazjInsanityPhase(Player* player)
{
    if (!player) return 0;
    for (uint32 spell : {uint32(SPELL_INSANITY_PHASE_16), uint32(SPELL_INSANITY_PHASE_32),
        uint32(SPELL_INSANITY_PHASE_64), uint32(SPELL_INSANITY_PHASE_128), uint32(SPELL_INSANITY_PHASE_256)})
        if (player->HasAura(spell)) return spell;
    return 0;
}

void instance_ahnkahet::HandleInsanityClear()
{
    for (auto const& reference : instance->GetPlayers())
        if (Player* player = reference.getSource())
            for (uint32 spell : {uint32(SPELL_INSANITY_PHASE_16), uint32(SPELL_INSANITY_PHASE_32),
                uint32(SPELL_INSANITY_PHASE_64), uint32(SPELL_INSANITY_PHASE_128), uint32(SPELL_INSANITY_PHASE_256)})
                player->RemoveAurasDueToSpell(spell);
}

bool instance_ahnkahet::HasInsanityVisage(uint32 phaseSpell) const
{
    if (phaseSpell < SPELL_INSANITY_PHASE_16 || phaseSpell > SPELL_INSANITY_PHASE_256) return false;
    const uint32 entry = NPC_TWISTED_VISAGE_1 + phaseSpell - SPELL_INSANITY_PHASE_16;
    for (ObjectGuid guid : m_insanityVisages)
        if (Creature* visage = instance->GetCreature(guid))
            if (visage->IsAlive() && visage->GetEntry() == entry) return true;
    return false;
}

void instance_ahnkahet::HandleInsanitySwitch(Player* source)
{
    const uint32 previous = GetVolazjInsanityPhase(source);
    if (!previous || HasInsanityVisage(previous)) return;
    // Choose a phase with living clones, even if its original player died or
    // disconnected. Otherwise surviving players cannot finish that clone group.
    std::vector<uint32> destinations;
    for (uint32 spell = SPELL_INSANITY_PHASE_16; spell <= SPELL_INSANITY_PHASE_256; ++spell)
        if (spell != previous && HasInsanityVisage(spell)) destinations.push_back(spell);
    if (destinations.empty()) return;
    const uint32 next = destinations[urand(0,destinations.size()-1)];
    for (auto const& reference : instance->GetPlayers())
    {
        Player* player = reference.getSource();
        if (!player || !player->IsAlive() || !player->HasAura(previous)) continue;
        player->CastSpell(player,next,TRIGGERED_OLD_TRIGGERED);
        if (player->HasAura(next))
            for (uint32 spell = SPELL_INSANITY_PHASE_16; spell <= SPELL_INSANITY_PHASE_256; ++spell)
                if (spell != next) player->RemoveAurasDueToSpell(spell);
    }
}

void instance_ahnkahet::UpdateInsanityPhases()
{
    if (GetData(TYPE_VOLAZJ) != SPECIAL) return;
    // Recheck failed phase casts and players whose original phase became empty.
    // A copy permits terminal cleanup/despawn callbacks to mutate the live set.
    std::set<ObjectGuid> current = m_insanityVisages;
    for (ObjectGuid guid : current)
        if (Creature* visage = instance->GetCreature(guid))
        {
            if (!visage->IsAlive()) FinishInsanityVisage(visage,false);
        }
        else m_insanityVisages.erase(guid);
    if (GetData(TYPE_VOLAZJ) != SPECIAL) return;
    m_uiTwistedVisageCount = m_insanityVisages.size();
    if (m_insanityVisages.empty())
    {
        if (Creature* boss = GetSingleCreatureFromStorage(NPC_HERALD_VOLAZJ))
            boss->RemoveAurasDueToSpell(57561);
        SetData(TYPE_VOLAZJ,IN_PROGRESS);
        return;
    }
    for (auto const& reference : instance->GetPlayers())
        if (Player* player = reference.getSource())
            if (player->IsAlive()) HandleInsanitySwitch(player);
}

bool instance_ahnkahet::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* /*pSource*/, Unit const* /*pTarget*/, uint32 /*uiMiscValue1 = 0*/) const
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRIT_RESPECT_ELDERS:
            return m_bRespectElders;
        case ACHIEV_CRIT_VOLUNTEER_WORK:
            return m_bVolunteerWork;

        default:
            return false;
    }
}

void instance_ahnkahet::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4];

    for (uint32& i : m_auiEncounter)
    {
        if (i == IN_PROGRESS)
            i = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_ahnkahet::GetData(uint32 uiType) const
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

InstanceData* GetInstanceData_instance_ahnkahet(Map* pMap)
{
    return new instance_ahnkahet(pMap);
}

void AddSC_instance_ahnkahet()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "instance_ahnkahet";
    pNewScript->GetInstanceData = &GetInstanceData_instance_ahnkahet;
    pNewScript->RegisterSelf();
}
