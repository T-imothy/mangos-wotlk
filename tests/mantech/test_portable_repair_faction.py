"""Compile the actual portable mailbox/repair scripts against boundary fakes.
Run in a Visual Studio developer shell. In-game visual testing is separate.
"""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[2]
source=(root/'src/game/AI/ScriptDevAI/scripts/world/item_scripts.cpp').read_text()
def extract(name):
    start=source.index('    struct '+name+' : public SpellScript')
    brace=source.index('{',start);depth=0
    for i in range(brace,len(source)):
        depth+=(source[i]=='{')-(source[i]=='}')
        if not depth:return source[start:i+1]+';'
prefix=r"""
#include <cassert>
#include <cstdint>
#include <iostream>
using uint32=uint32_t;
enum {SPELL_CAST_OK,SPELL_FAILED_NOT_HERE}; using SpellCastResult=int;
enum {TYPEID_PLAYER=4,GAMEOBJECT_TYPE_MAILBOX=19,HIGHGUID_GAMEOBJECT=5,ALLIANCE=1,TEMPFACTION_NONE=0};
const float DEFAULT_WORLD_OBJECT_SIZE=0.5f;
struct GameObject;struct Map {GameObject* added=nullptr;uint32 GenerateLocalLowGuid(int){return 77;}void Add(GameObject* go){added=go;}};
struct Session{int notices=0;void SendNotification(char const*){++notices;}};
struct SpellEntry{uint32 Id=1206;};
struct WorldObject{
    bool world=true;uint32 type=TYPEID_PLAYER;Map map;
    virtual ~WorldObject()=default;
    uint32 GetTypeId(){return type;}bool IsInWorld(){return world;}Map* GetMap(){return &map;}
    void GetClosePoint(float& x,float& y,float& z,float,float distance){assert(distance==1);x=10;y=20;z=30;}
    float GetOrientation(){return 1.5f;}uint32 GetObjectGuid(){return 91;}
};
struct Player:WorldObject{
    Session session;uint32 team=ALLIANCE;uint32 cleared=0;
    uint32 GetTeam(){return team;}Session* GetSession(){return &session;}
    void RemoveSpellCooldown(SpellEntry const& s){cleared=s.Id;}
};
struct Item{uint32 id;uint32 GetEntry(){return id;}};
struct Creature{uint32 id=24780,faction=35;uint32 GetEntry(){return id;}void SetFactionTemporary(uint32 f,int flag){assert(flag==0);faction=f;}};
struct GameObjectInfo{uint32 type=GAMEOBJECT_TYPE_MAILBOX;};
GameObjectInfo info;bool templateExists=true,createSucceeds=true;int destroyed=0;
struct ObjectMgr{static GameObjectInfo const* GetGameObjectInfo(uint32 id){assert(id==142102);return templateExists?&info:nullptr;}};
struct GameObject{
    uint32 lifetime=0,spell=0,owner=0;bool ai=false;
    ~GameObject(){++destroyed;}
    static GameObject* CreateGameObject(uint32 id){assert(id==142102);return new GameObject;}
    bool Create(uint32 a,uint32 b,uint32 entry,Map*,float x,float y,float z,float o){assert(a==77&&b==77&&entry==142102&&x==10&&y==20&&z==30&&o==1.5f);return createSucceeds;}
    void SetRespawnTime(uint32 s){lifetime=s;}void SetSpellId(uint32 s){spell=s;}void SetSpawnerGuid(uint32 g){owner=g;}void AIM_Initialize(){ai=true;}
};
struct Spell{Item* item;WorldObject* caster;SpellEntry entry;SpellEntry* m_spellInfo=&entry;Item* GetCastItem(){return item;}WorldObject* GetTrueCaster(){return caster;}};
struct SpellScript{virtual SpellCastResult OnCheckCast(Spell*,bool)const{return SPELL_CAST_OK;}virtual void OnCast(Spell*)const{}virtual void OnSummon(Spell*,Creature*)const{}};
"""
# Compile the production summon resolver too: the callback receives its output,
# not the native spell_template entry. This reproduces the original regression.
resolver=(root/'src/game/Entities/PortableRepairVendor.h').read_text()
prefix += '\n'.join(line for line in resolver.splitlines() if not line.startswith('#include')) + '\n'
suffix=r"""
int main(){
    ManTechPortableRepairSpell repair;Player player;Item item{65001};Spell spell{&item,&player};Creature bot;bot.id=PortableRepairVendor::ResolveSummonEntry(65001,44389,24780);assert(bot.id==65001);
    repair.OnSummon(&spell,&bot);assert(bot.faction==12);
    player.team=2;repair.OnSummon(&spell,&bot);assert(bot.faction==29);
    for(auto id:{65000u,65002u,34113u}){item.id=id;bot.faction=35;repair.OnSummon(&spell,&bot);assert(bot.faction==35);}
    item.id=65001;spell.item=nullptr;repair.OnSummon(&spell,&bot);assert(bot.faction==35);spell.item=&item;
    bot.id=123;repair.OnSummon(&spell,&bot);assert(bot.faction==35);bot.id=PortableRepairVendor::CREATURE_ENTRY;
    repair.OnSummon(&spell,nullptr);spell.caster=nullptr;repair.OnSummon(&spell,&bot);assert(bot.faction==35);spell.caster=&player;
    player.type=3;repair.OnSummon(&spell,&bot);assert(bot.faction==35);
    item.id=65001;player.type=TYPEID_PLAYER;spell.caster=&player;spell.item=&item;
    bot.id=24780;bot.faction=190;repair.OnSummon(&spell,&bot);assert(bot.faction==190);
    item.id=34113;bot.id=PortableRepairVendor::ResolveSummonEntry(34113,44389,24780);
    assert(bot.id==24780);repair.OnSummon(&spell,&bot);assert(bot.faction==190);
    item.id=65001;bot.id=PortableRepairVendor::ResolveSummonEntry(65001,44389,24780);player.team=ALLIANCE;
    repair.OnSummon(&spell,&bot);assert(bot.faction==12);
    std::cout<<"PASS actual repair script: Alliance/Horde factions, nonplayer and wrong summon guards, native item/cast isolation\n";
}
"""
with tempfile.TemporaryDirectory(prefix='mantech-portable-services-') as folder:
    folder=Path(folder)
    (folder/'test.cpp').write_text(prefix+extract('ManTechPortableRepairSpell')+suffix)
    subprocess.run(['cl.exe','/nologo','/EHsc','/std:c++17','test.cpp','/Fe:test.exe'],cwd=folder,check=True)
    subprocess.run([str(folder/'test.exe')],check=True)
