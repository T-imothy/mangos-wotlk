"""Exercise production Follow::Move and Unit spline admission using boundary fakes.
No game client, database, or live process is modified. Run from an MSVC shell.
"""
from pathlib import Path
import subprocess,sys,tempfile,json
HERE=Path(__file__).resolve().parent
ROOT=Path(__file__).resolve().parents[2]
def block(s,marker):
    start=s.index(marker); a=s.index('{',start); depth=0
    for i in range(a,len(s)):
        depth+=(s[i]=='{')-(s[i]=='}')
        if depth==0:return s[start:i+1]
motion=(ROOT/'src/game/MotionGenerators/TargetedMovementGenerator.cpp').read_text()
units=(ROOT/'src/game/Entities/Unit.cpp').read_text()
move=block(motion,'bool FollowMovementGenerator::Move(')
prefix=r'''
#include <cassert>
#include <cstdint>
#include <vector>
#include <iostream>
using uint32=uint32_t;
enum {PATHFIND_NOPATH=1,PATHFIND_SHORTCUT=2,UNIT_FIELD_FLAGS=0,UNIT_FLAG_PLAYER_CONTROLLED=1,TYPEID_PLAYER=4};
struct Point{float x,y,z;};
struct Timer{void Update(uint32){} bool Passed(){return true;} void Reset(uint32){}};
struct Spline{bool done=false; float oldPosition=150; bool Finalized(){return done;} void _Interrupt(){done=true;} void updateState(uint32){oldPosition+=5;}};
struct Creature;
struct Map{bool los=true;int relocations=0;bool IsInLineOfSight(float,float,float,float,float,float,int,bool)const{return los;} void CreatureRelocation(Creature*,float,float,float,float);};
struct GenericTransport{void CalculatePassengerPosition(float&,float&,float&)const{}};
struct Unit{
    Map map;Spline spline;Spline* movespline=&spline;Timer m_movesplineTimer;
    float x=150,y=-20,z=-4,o=0;bool pc=false,followMove=true;int type=3,stopPackets=0,heartbeats=0;int pathType=PATHFIND_NOPATH;
    int GetTypeId(){return type;}Map* GetMap(){return &map;}const GenericTransport* GetTransport(){return nullptr;}
    int GetPhaseMask(){return 1;}bool HasFlag(int,int){return pc;}float GetCollisionHeight(){return 1;}
    bool IsWithinLOS(float,float,float,bool){return map.los;}void GetPosition(float& a,float& b,float& c){a=x;b=y;c=z;}
    bool IsMoving(){return !spline.done;}float GetOrientation(){return o;}
    void UpdateSplinePosition(bool=false){x=spline.oldPosition;}
    void StopMoving(bool send){if(send)++stopPackets;spline.done=true;}void DisableSpline(){spline.done=true;}
    void InterruptMoving(bool forceSendStop=false);
    void UpdateSplineMovement(uint32);
    void NearTeleportTo(float a,float b,float c,float d){InterruptMoving();x=a;y=b;z=c;o=d;}
    void SendHeartBeat(){++heartbeats;}
};
struct Creature:Unit{};
void Map::CreatureRelocation(Creature* c,float x,float y,float z,float o){++relocations;c->x=x;c->y=y;c->z=z;c->o=o;}
struct PathFinder{Unit* unit;std::vector<Point> path{{150,-20,-4},{0,0,0}};PathFinder(Unit* u):unit(u){}void calculate(float,float,float){}auto& getPath(){return path;}int getPathType(){return unit->pathType;}};
namespace Movement{struct MoveSplineInit{Unit& u;MoveSplineInit(Unit& u):u(u){}void MovebyPath(const std::vector<Point>&){}void SetWalk(bool){}void SetVelocity(float){}void SetFacing(float){}void Launch(){u.spline.done=false;u.spline.oldPosition=12;}};}
struct FollowMovementGenerator{
    PathFinder* i_path=nullptr;Unit* i_target;bool allow=true;
    FollowMovementGenerator(Unit* t):i_target(t){}~FollowMovementGenerator(){delete i_path;}
    bool IsUnstuckAllowed(Unit&){return allow;}bool _getOrientation(Unit&,float& o){o=0;return true;}
    bool _getLocation(Unit&,float& x,float& y,float& z,bool){x=1;y=2;z=3;return true;}
    void _addUnitStateMove(Unit& u){u.followMove=true;}void _clearUnitStateMove(Unit& u){u.followMove=false;}
    bool EnableWalking(){return false;}float GetSpeed(Unit&){return 7;}bool Move(Unit&,float,float,float);
};
'''
main=r'''
int main(){
    int failures=0;
    for(int path:{PATHFIND_NOPATH,PATHFIND_SHORTCUT}){
        Unit master;master.pc=true;Creature pet;pet.pathType=path;FollowMovementGenerator follow(&master);
        assert(!follow.Move(pet,1,2,3));assert(pet.map.relocations==1);assert(pet.x==1);
        pet.UpdateSplineMovement(400);
        if(pet.x!=1||!pet.spline.done||pet.followMove){++failures;std::cout<<"FAIL: old spline overwrites recovered pet position, path="<<path<<" x="<<pet.x<<"\n";}
    }
    {Unit master;master.pc=true;master.map.los=false;Creature pet;pet.map.los=false;pet.pathType=0;FollowMovementGenerator follow(&master);
     assert(!follow.Move(pet,1,2,3));assert(pet.map.relocations==1);float restored=pet.x;pet.UpdateSplineMovement(400);
     if(pet.x!=restored||!pet.spline.done){++failures;std::cout<<"FAIL: LOS recovery resumes old spline\n";}}
    {Unit master;Creature pet;FollowMovementGenerator follow(&master);follow.allow=false;
     assert(!follow.Move(pet,1,2,3));assert(pet.map.relocations==0&&pet.stopPackets==0);}
    {Unit master;Creature pet;pet.pathType=0;FollowMovementGenerator follow(&master);
     assert(follow.Move(pet,1,2,3));assert(pet.map.relocations==0&&pet.stopPackets==0&&!pet.spline.done);}
    {Unit master;Unit bot;bot.type=TYPEID_PLAYER;FollowMovementGenerator follow(&master);
     assert(!follow.Move(bot,1,2,3));bot.UpdateSplineMovement(400);assert(bot.x==1&&bot.spline.done);}
    if(!failures)std::cout<<"PASS: recovery holds after next tick; normal paths, denied recovery and player teleport preserved\n";
    return failures?1:0;
}
'''
source=prefix+'\n'+block(units,'void Unit::InterruptMoving(')+'\n'+block(units,'void Unit::UpdateSplineMovement(')+'\n'+move+'\n'+main
with tempfile.TemporaryDirectory(prefix='pet-follow-') as tmp:
    d=Path(tmp);(d/'test.cpp').write_text(source)
    p=subprocess.run(['cl','/nologo','/std:c++17','/EHsc',str(d/'test.cpp'),'/Fe:'+str(d/'test.exe'),'/Fo:'+str(d/'test.obj')],capture_output=True,text=True)
    if p.returncode:print(p.stdout+p.stderr);sys.exit(p.returncode)
    p=subprocess.run([str(d/'test.exe')],capture_output=True,text=True)
    print(p.stdout+p.stderr);sys.exit(p.returncode)
