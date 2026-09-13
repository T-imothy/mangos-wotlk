#include "Memory/SharedNavTile.h"
#include <Detour/Include/DetourNavMeshBuilder.h>
#include <Detour/Include/DetourNavMeshQuery.h>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <thread>

#define CHECK(x) do { if (!(x)) { std::cerr << __LINE__ << ": " #x " failed\n"; std::abort(); } } while (false)

struct Instance
{
    // The tail must outlive the mesh; query workspace is per instance.
    std::shared_ptr<ManTech::SharedNavTail> tail;
    dtNavMesh mesh;
    dtNavMeshQuery query;
    dtTileRef tile{};
    void Load(unsigned char* data, int size)
    {
        auto prefix = ManTech::SharedNavTileCache::PrefixSize(data, size);
        CHECK(prefix && prefix < static_cast<unsigned>(size));
        tail = ManTech::SharedNavTileCache::Acquire("test-tile", data + prefix, data + size);
        auto privateData = static_cast<unsigned char*>(dtAlloc(prefix, DT_ALLOC_PERM));
        CHECK(privateData);
        std::memcpy(privateData, data, prefix);
        dtNavMeshParams params{};
        params.tileWidth = params.tileHeight = 10;
        params.maxTiles = 2; params.maxPolys = 16;
        CHECK(dtStatusSucceed(mesh.init(&params)));
        CHECK(dtStatusSucceed(mesh.addTileShared(privateData, static_cast<int>(prefix), DT_TILE_FREE_DATA, 0, &tile, tail->bytes.data())));
        CHECK(dtStatusSucceed(query.init(&mesh, 64)));
    }
    void Path()
    {
        float from[]{2,0,2}, to[]{8,0,8}, extent[]{2,2,2}, closest[3];
        dtQueryFilter filter;
        dtPolyRef a{}, b{}, path[8]; int count{};
        CHECK(dtStatusSucceed(query.findNearestPoly(from, extent, &filter, &a, closest)) && a);
        CHECK(dtStatusSucceed(query.findNearestPoly(to, extent, &filter, &b, closest)) && b);
        CHECK(dtStatusSucceed(query.findPath(a, b, from, to, &filter, path, &count, 8)) && count == 1);
    }
};

int main()
{
    unsigned short verts[]{0,0,0, 0,0,10, 10,0,10, 10,0,0};
    unsigned short polygons[]{0,1,2,3,0xffff,0xffff, 0xffff,0xffff,0xffff,0xffff,0xffff,0xffff};
    unsigned short flags[]{1}; unsigned char areas[]{0};
    dtNavMeshCreateParams params{};
    params.verts=verts; params.vertCount=4; params.polys=polygons;
    params.polyFlags=flags; params.polyAreas=areas; params.polyCount=1; params.nvp=6;
    params.bmax[0]=params.bmax[2]=10; params.bmax[1]=2;
    params.walkableHeight=2; params.walkableRadius=0.5; params.walkableClimb=0.5;
    params.cs=params.ch=1; params.buildBvTree=true;
    unsigned char* data{}; int size{};
    CHECK(dtCreateNavMeshData(&params, &data, &size));
    CHECK(ManTech::SharedNavTileCache::PrefixSize(data, size-1)==0);
    auto header = reinterpret_cast<dtMeshHeader*>(data);
    int original = header->polyCount; header->polyCount=-1;
    CHECK(ManTech::SharedNavTileCache::PrefixSize(data, size)==0);
    header->polyCount=original;
    const auto before=ManTech::MemoryLedger::Read(ManTech::MemoryKind::NavShared).bytes;
    {
        Instance first; first.Load(data,size);
        {
            Instance second; second.Load(data,size);
            CHECK(first.tail == second.tail);
            auto a=first.mesh.getTileByRef(first.tile), b=second.mesh.getTileByRef(second.tile);
            CHECK(a->polys != b->polys && a->verts != b->verts && a->links != b->links);
            CHECK(a->detailMeshes == b->detailMeshes && a->bvTree == b->bvTree);
            auto ref=first.mesh.getPolyRefBase(a);
            CHECK(dtStatusSucceed(first.mesh.setPolyFlags(ref, 2)));
            CHECK(b->polys[0].flags==1);
            CHECK(dtStatusSucceed(first.mesh.setPolyArea(ref, 7)));
            CHECK(b->polys[0].getArea()==0);
            CHECK(dtStatusSucceed(first.mesh.setPolyFlags(ref, 1)));
            std::thread one([&]{for(int i=0;i<1000;++i)first.Path();});
            std::thread two([&]{for(int i=0;i<1000;++i)second.Path();});
            one.join(); two.join();
        }
        first.Path(); // Destroying a sibling instance cannot invalidate this query.
        auto changed=first.tail->bytes;
        changed.back() ^= 1;
        auto generation=ManTech::SharedNavTileCache::Acquire("test-tile",changed.data(),changed.data()+changed.size());
        CHECK(generation != first.tail && generation->bytes != first.tail->bytes);
        first.Path();
    }
    dtFree(data);
    CHECK(ManTech::MemoryLedger::Read(ManTech::MemoryKind::NavShared).bytes==before);
    std::cout << "Shared navigation: parser, private mutable state, independent concurrent queries, generation replacement and unload passed\n";
}
