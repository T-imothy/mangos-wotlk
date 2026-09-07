"""Execute actual Wrath roster-packet and member-status functions with core doubles."""
from pathlib import Path
import subprocess, sys, tempfile
root=Path(__file__).resolve().parents[1]
before='--before' in sys.argv
if before:
    source=subprocess.check_output(['C:/Program Files/Git/cmd/git.exe','-c','safe.directory='+root.as_posix(),'-C',str(root),'show','5f30cd46e1:src/game/Groups/Group.cpp'],text=True)
else:
    source=(root/'src/game/Groups/Group.cpp').read_text()
def function(signature):
    start=source.index(signature);brace=source.index('{',start);depth=1;end=brace+1
    while depth:
        depth+=(source[end]=='{')-(source[end]=='}');end+=1
    return source[start:end]
fixture=r'''
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using uint8=uint8_t;using uint32=uint32_t;
struct ObjectGuid {uint64_t id=0; ObjectGuid(uint64_t v=0):id(v){} bool operator==(ObjectGuid v)const{return id==v.id;} };
enum GroupMemberStatus {MEMBER_STATUS_OFFLINE=0,MEMBER_STATUS_ONLINE=1,MEMBER_STATUS_PVP=2,MEMBER_STATUS_DEAD=4,MEMBER_STATUS_GHOST=8,MEMBER_STATUS_PVP_FFA=16,MEMBER_STATUS_ZONE_OUT=32,MEMBER_STATUS_AFK=64,MEMBER_STATUS_DND=128};
constexpr int PLAYER_FLAGS=1,PLAYER_FLAGS_GHOST=2,SMSG_GROUP_LIST=3,MASTER_LOOT=2,GROUP_FLAG_LFG=8,LFG_STATE_FINISHED_DUNGEON=10;
struct WorldPacket {
 std::vector<uint64_t> values;
 WorldPacket(int=0,int=0){}
 template<class T> WorldPacket& operator<<(T v){values.push_back(uint64_t(v));return *this;}
 WorldPacket& operator<<(ObjectGuid v){values.push_back(v.id);return *this;}
 WorldPacket& operator<<(std::string const&){values.push_back(0);return *this;}
};
struct WorldSession {bool logout=false; WorldPacket sent; bool PlayerLogout()const{return logout;} void SendPacket(WorldPacket const& p){sent=p;} };
struct Lfg {uint8 roles=4;int state=0;uint32 dungeon=42;uint8 GetPlayerRoles()const{return roles;}int GetState()const{return state;}uint32 GetDungeon()const{return dungeon;} };
struct MapEntry {bool IsDynamicDifficultyMap(){return false;}};
struct Map {MapEntry entry;MapEntry* GetEntry(){return &entry;}bool IsHeroic(){return false;}};
struct Group;
struct Player {
 ObjectGuid guid;Group* group=nullptr;WorldSession* session=nullptr;Lfg lfg;Map map;
 bool pvp=false,dead=false,ghost=false,ffa=false,inworld=true,afk=false,dnd=false;
 Group* GetGroup(){return group;}WorldSession* GetSession()const{return session;}
 ObjectGuid GetObjectGuid(){return guid;} Lfg& GetLfgData(){return lfg;}Map* GetMap(){return &map;}
 bool IsPvP()const{return pvp;}bool IsDead()const{return dead;}bool HasFlag(int,int)const{return ghost;}
 bool IsPvPFreeForAll()const{return ffa;}bool IsInWorld()const{return inworld;}bool isAFK()const{return afk;}bool isDND()const{return dnd;}
};
struct Manager {std::map<uint64_t,Player*> players;Player* GetPlayer(ObjectGuid g){auto i=players.find(g.id);return i==players.end()?nullptr:i->second;}} sObjectMgr;
struct Group {
 struct Slot {ObjectGuid guid;std::string name;uint8 group=0;};
 using member_citerator=std::vector<Slot>::const_iterator;
 std::vector<Slot> m_memberSlots;uint8 m_groupFlags=0,m_lootMethod=0,m_lootThreshold=2,m_dungeonDifficulty=0,m_raidDifficulty=0;
 uint32 m_counter=0;ObjectGuid m_masterLooterGuid,m_leaderGuid{1};Lfg lfg;
 member_citerator _getMemberCSlot(ObjectGuid g){for(auto i=m_memberSlots.cbegin();i!=m_memberSlots.cend();++i)if(i->guid==g)return i;return m_memberSlots.end();}
 uint8 GetFlags(Slot const&){return 0;}uint32 GetMembersCount(){return uint32(m_memberSlots.size());}ObjectGuid GetObjectGuid(){return ObjectGuid(99);}Lfg& GetLfgData(){return lfg;}
 void SendUpdateTo(Player*);
};
void check(bool okay,const char* msg){if(!okay){std::cerr<<"FAIL: "<<msg<<"\n";std::exit(2);}}
'''
test=r'''
int main(){
 Group group;WorldSession leaderSession,humanSession,botSession,leavingSession;
 Player leader,human,bot,leaving;leader.guid=1;human.guid=2;bot.guid=3;leaving.guid=5;
 leader.session=&leaderSession;human.session=&humanSession;bot.session=&botSession;leaving.session=&leavingSession;
 for(auto p:{&leader,&human,&bot,&leaving}){p->group=&group;sObjectMgr.players[p->guid.id]=p;}
 for(int i=1;i<=5;++i)group.m_memberSlots.push_back({ObjectGuid(i),"member"});
 leavingSession.logout=true;
 auto verify=[&](bool lfg){
   group.m_groupFlags=lfg?GROUP_FLAG_LFG:0;group.SendUpdateTo(&leader);
   auto const& v=leaderSession.sent.values;size_t header=lfg?9:7;
   check(v[header-1]==4,"recipient excluded from roster");
   check(v[header+2]==MEMBER_STATUS_ONLINE,"connected human must be online in roster");
   check(v[header+8]==MEMBER_STATUS_ONLINE,"connected bot must be online in roster");
   check(v[header+14]==MEMBER_STATUS_OFFLINE,"missing member must remain offline");
   check(v[header+20]==MEMBER_STATUS_OFFLINE,"logging-out member must remain offline");
   check(v[header+2]==GetGroupMemberStatus(&human),"roster and individual status agree");
 };
 verify(false);verify(true);verify(false);
 bot.session=nullptr;group.SendUpdateTo(&leader);check(leaderSession.sent.values[15]==0,"sessionless member offline");bot.session=&botSession;
 bot.pvp=bot.dead=bot.ghost=bot.ffa=bot.afk=bot.dnd=true;
 group.SendUpdateTo(&leader);check(leaderSession.sent.values[15]==223,"status flags preserved");
 bot.inworld=false;check(GetGroupMemberStatus(&bot)==255,"zone-out flag preserved");
 check(GetGroupMemberStatus(nullptr)==0,"null member offline");
 std::cout<<"PASS: actual normal/LFG roster serialization; humans, bots, missing/logout/sessionless members, repeated updates, all status flags\n";
}
'''
with tempfile.TemporaryDirectory(prefix='group-roster-') as temp:
    path=Path(temp);cpp=path/'test.cpp';exe=path/'test.exe'
    cpp.write_text(fixture+function('GroupMemberStatus GetGroupMemberStatus(')+function('void Group::SendUpdateTo(')+test)
    subprocess.run(['cl','/nologo','/std:c++17','/EHsc',str(cpp),'/Fe:'+str(exe)],cwd=path,check=True)
    result=subprocess.run([str(exe)],capture_output=True,text=True)
    print(result.stdout+result.stderr)
    if before:
        assert result.returncode==2 and 'connected human must be online' in result.stderr
        print('PASS: deployed source reproduces erroneous offline roster status')
    else:assert result.returncode==0,result.returncode
