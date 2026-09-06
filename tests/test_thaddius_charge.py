"""Compile the native periodic charge callback with live/dead/missing casters."""
from pathlib import Path
import subprocess
import tempfile

repo=Path(__file__).resolve().parents[1]
source=(repo/'src/game/AI/ScriptDevAI/scripts/northrend/naxxramas/boss_thaddius.cpp').read_text()
source=source.split('struct ThaddiusCharge :')[1]
begin=source.index('void OnPeriodicTrigger(')
brace=source.index('{',begin);depth=1;end=brace+1
while depth:
    depth+=(source[end]=='{')-(source[end]=='}');end+=1
method=source[begin:end]
code=r'''
#include <cassert>
#include <vector>
#include <iostream>
using uint32=unsigned;
enum{SPELL_POSITIVE_CHARGE=28059,SPELL_NEGATIVE_CHARGE=28084,
 SPELL_POSITIVE_CHARGE_BUFF=29659,SPELL_NEGATIVE_CHARGE_BUFF=29660,TRIGGERED_OLD_TRIGGERED=1};
struct Unit{bool alive=true;unsigned charge=28059,removed=0,casts=0,buff=0;
 bool IsAlive(){return alive;}bool HasAura(unsigned id){return charge==id;}
 void RemoveAurasDueToSpell(unsigned id){removed=id;casts=0;}
 void CastSpell(Unit* target,unsigned id,unsigned flags){assert(target==this&&flags==TRIGGERED_OLD_TRIGGERED);buff=id;++casts;}};
using Player=Unit;using PlayerList=std::vector<Player*>;PlayerList players;
void GetPlayerListWithEntryInWorld(PlayerList& result,Unit*,float range){assert(range==13.f);result=players;}
struct Aura{Unit* target=nullptr;Unit* caster=nullptr;unsigned id=28059;
 Unit* GetTarget(){return target;}Unit* GetCaster(){return caster;}unsigned GetId(){return id;}};
struct PeriodicTriggerData{};struct AuraScript{virtual void OnPeriodicTrigger(Aura*,PeriodicTriggerData&) const{}};
struct Handler:AuraScript{__METHOD__};
int main(){Handler handler;PeriodicTriggerData data;Unit target,caster,other,opposite;opposite.charge=28084;
 Aura aura{&target,&caster};players={&target,&other,&opposite};handler.OnPeriodicTrigger(&aura,data);
 assert(target.removed==29659&&target.buff==29659&&target.casts==1);
 caster.alive=false;handler.OnPeriodicTrigger(&aura,data);assert(target.removed==29659&&target.casts==0);
 aura.caster=nullptr;target.casts=10;handler.OnPeriodicTrigger(&aura,data);assert(target.removed==29659&&target.casts==0);
 aura.id=28084;handler.OnPeriodicTrigger(&aura,data);assert(target.removed==29660&&target.casts==0);
 aura.target=nullptr;handler.OnPeriodicTrigger(&aura,data);
 std::cout<<"PASS: native charge buffs retain live stacking and safely clean dead/missing caster state\n";}
'''.replace('__METHOD__',method)
with tempfile.TemporaryDirectory(prefix='mantech-native-charge-') as directory:
    tmp=Path(directory);(tmp/'test.cpp').write_text(code)
    subprocess.run(['cl','/nologo','/std:c++17','/EHsc','test.cpp','/Fe:test.exe'],cwd=tmp,check=True)
    subprocess.run([str(tmp/'test.exe')],cwd=tmp,check=True)
