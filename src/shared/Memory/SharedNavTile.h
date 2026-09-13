#ifndef MANTECH_SHARED_NAV_TILE_H
#define MANTECH_SHARED_NAV_TILE_H
#include "Memory/MemoryLedger.h"
#include <Detour/Include/DetourNavMesh.h>
#include <Detour/Include/DetourCommon.h>
#include <memory>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>
#include <cstring>
#include <limits>
namespace ManTech {
struct SharedNavTail {
    std::vector<unsigned char> bytes;
    SharedNavTail(unsigned char const* begin, unsigned char const* end):bytes(begin,end) { MemoryLedger::Add(MemoryKind::NavShared,bytes.size()); }
    ~SharedNavTail() { MemoryLedger::Remove(MemoryKind::NavShared,bytes.size()); }
};
class SharedNavTileCache {
    inline static std::mutex mutex;
    inline static std::unordered_map<std::string,std::weak_ptr<SharedNavTail>> entries;
    inline static unsigned loadsSincePrune = 0;
    static void PruneLocked() {
        for (auto i = entries.begin(); i != entries.end();)
            if (i->second.expired()) i = entries.erase(i); else ++i;
    }
public:
    static void Prune() {
        std::lock_guard<std::mutex> guard(mutex);
        PruneLocked();
    }
    // Validate every packed range before Detour follows any of its pointers.
    static std::size_t PrefixSize(unsigned char const* data, std::size_t size) {
        if(size<sizeof(dtMeshHeader)) return 0;
        dtMeshHeader h; std::memcpy(&h,data,sizeof(h));
        if(h.magic!=DT_NAVMESH_MAGIC || h.version!=DT_NAVMESH_VERSION || h.maxLinkCount<=0) return 0;
        auto part=[](int count,std::size_t width)->std::size_t {
            if(count<0 || static_cast<std::size_t>(count)>64*1024*1024/width) return std::numeric_limits<std::size_t>::max();
            return (static_cast<std::size_t>(count)*width+3)&~std::size_t(3);
        };
        std::size_t lengths[]={part(1,sizeof(dtMeshHeader)),part(h.vertCount,3*sizeof(float)),part(h.polyCount,sizeof(dtPoly)),part(h.maxLinkCount,sizeof(dtLink)),part(h.detailMeshCount,sizeof(dtPolyDetail)),part(h.detailVertCount,3*sizeof(float)),part(h.detailTriCount,4),part(h.bvNodeCount,sizeof(dtBVNode)),part(h.offMeshConCount,sizeof(dtOffMeshConnection))};
        std::size_t sum=0,prefix=0;
        for(unsigned i=0;i<9;++i) { if(lengths[i]>size-sum) return 0; sum+=lengths[i]; if(i==3) prefix=sum; }
        return sum==size ? prefix : 0;
    }
    static std::shared_ptr<SharedNavTail> Acquire(std::string const& key,unsigned char const* begin,unsigned char const* end) {
        std::lock_guard<std::mutex> guard(mutex);
        // Reclaim expired metadata in batches, not a full cache scan per tile.
        // Map destruction also explicitly prunes after releasing its leases.
        if (++loadsSincePrune == 256) { PruneLocked(); loadsSincePrune = 0; }
        auto& weak=entries[key];
        if(auto current=weak.lock())
            if(current->bytes.size()==static_cast<std::size_t>(end-begin) && (begin==end || std::memcmp(current->bytes.data(),begin,end-begin)==0)) return current;
        auto result=std::make_shared<SharedNavTail>(begin,end); weak=result; return result;
    }
};
}
#endif
