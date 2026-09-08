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
SDName: boss_emalon
SD%Complete: 0
SDComment: EventAI boss; native 25-player Lightning Nova damage falloff
SDCategory: Vault of Archavon
EndScriptData */

#include "AI/ScriptDevAI/include/sc_common.h"
#include "Spells/Scripts/SpellScript.h"

// 65279 - 25-player Lightning Nova. The 100-yard targeting radius is distinct
// from the damage falloff. Match the reference spell_voa_lightning_nova model.
struct spell_emalon_lightning_nova : public SpellScript
{
    void OnEffectExecute(Spell* spell, SpellEffectIndex effect) const override
    {
        if (effect != EFFECT_INDEX_0 || !spell->GetCaster() || !spell->GetUnitTarget()) return;
        constexpr float falloffDistance = 70.0f;
        const float distance = spell->GetCaster()->GetDistance(spell->GetUnitTarget());
        const float fraction = std::max(0.0f, std::min(1.0f, (falloffDistance - distance) / falloffDistance));
        spell->SetDamage(int32(spell->GetDamage() * fraction));
    }
};

void AddSC_boss_emalon()
{
    RegisterSpellScript<spell_emalon_lightning_nova>("spell_emalon_lightning_nova");
}
