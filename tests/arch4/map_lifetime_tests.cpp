#include "MotionGenerators/MoveMap.h"
#include "MotionGenerators/MoveMapSharedDefines.h"
#include <Detour/Include/DetourNavMeshBuilder.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "Database/DatabaseEnv.h"

// The game library normally obtains these process globals from mangosd.
// This fixture never initializes a database or starts the world.
DatabaseType WorldDatabase;
DatabaseType CharacterDatabase;
DatabaseType LoginDatabase;
DatabaseType LogsDatabase;
uint32 realmID = 0;

#define CHECK(x) do { if (!(x)) { std::cerr << __LINE__ << ": " #x " failed\n"; std::abort(); } } while (false)

int main()
{
    auto directory=std::filesystem::temp_directory_path()/std::filesystem::path("arch4-nav-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(directory/"mmaps");
    std::string base=directory.generic_string()+"/";
    dtNavMeshParams meshParams{};
    meshParams.tileWidth=meshParams.tileHeight=10;meshParams.maxTiles=2;meshParams.maxPolys=16;
    {
        std::ofstream file(directory/"mmaps/033.mmap",std::ios::binary);
        file.write(reinterpret_cast<char*>(&meshParams),sizeof(meshParams));
    }
    unsigned short verts[]{0,0,0, 0,0,10, 10,0,10, 10,0,0};
    unsigned short polygons[]{0,1,2,3,0xffff,0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff,0xffff};
    unsigned short flags[]{1};unsigned char areas[]{0};
    dtNavMeshCreateParams params{};
    params.verts=verts;params.vertCount=4;params.polys=polygons;params.polyFlags=flags;params.polyAreas=areas;params.polyCount=1;params.nvp=6;
    params.bmax[0]=params.bmax[2]=10;params.bmax[1]=2;
    params.walkableHeight=2;params.walkableRadius=0.5;params.walkableClimb=0.5;params.cs=params.ch=1;params.buildBvTree=true;
    unsigned char* data{};int size{};
    CHECK(dtCreateNavMeshData(&params,&data,&size));
    {
        MmapTileHeader header;header.size=size;
        std::ofstream file(directory/"mmaps/0330000.mmtile",std::ios::binary);
        file.write(reinterpret_cast<char*>(&header),sizeof(header));
        file.write(reinterpret_cast<char*>(data),size);
    }
    dtFree(data);
    MMAP::MMapManager manager;
    for(unsigned cycle=0;cycle<20;++cycle)
    {
        CHECK(manager.loadMapInstance(base,33,1001));
        CHECK(manager.loadMapInstance(base,33,1002));
        CHECK(manager.loadMap(base,33,1001,0,0,0));
        CHECK(manager.loadMap(base,33,1002,0,0,0));
        CHECK(manager.getLoadedMapsCount()==2 && manager.getLoadedTilesCount()==2);
        CHECK(ManTech::MemoryLedger::Read(ManTech::MemoryKind::NavShared).count==1);
        auto first=manager.GetNavMeshQuery(33,1001);
        auto second=manager.GetNavMeshQuery(33,1002);
        CHECK(first && second && first!=second);
        std::thread worker([&]{CHECK(manager.GetNavMeshQuery(33,1001)!=first);});worker.join();
        CHECK(manager.getMapThreadQueryCount()==3);
        CHECK(ManTech::MemoryLedger::Read(ManTech::MemoryKind::NavQueries).count==3);
        CHECK(ManTech::MemoryLedger::Read(ManTech::MemoryKind::NavQueries).bytes>0);
        CHECK(manager.unloadMapInstance(33,1001));
        CHECK(manager.GetNavMesh(33,1001)==nullptr);
        CHECK(manager.getLoadedTilesCount()==1 && manager.getMapThreadQueryCount()==1);
        CHECK(manager.GetNavMeshQuery(33,1002)==second);
        CHECK(manager.unloadMapInstance(33,1002));
        CHECK(manager.getLoadedMapsCount()==0 && manager.getLoadedTilesCount()==0 && manager.getMapThreadQueryCount()==0);
        CHECK(ManTech::MemoryLedger::Read(ManTech::MemoryKind::NavShared).bytes==0);
        CHECK(ManTech::MemoryLedger::Read(ManTech::MemoryKind::NavTiles).bytes==0);
        CHECK(ManTech::MemoryLedger::Read(ManTech::MemoryKind::NavQueries).bytes==0);
        CHECK(ManTech::MemoryLedger::Read(ManTech::MemoryKind::NavQueries).count==0);
    }
    CHECK(manager.getQueryAllocationCount()==manager.getQueryFreeCount());
    // Only remove the three files created by this test; no recursive deletion.
    std::filesystem::remove(directory/"mmaps/0330000.mmtile");
    std::filesystem::remove(directory/"mmaps/033.mmap");
    std::filesystem::remove(directory/"mmaps");std::filesystem::remove(directory);
    std::cout << "Actual MMapManager: 20 instance load/unload cycles release tiles, queries and geometry leases\n";
}
