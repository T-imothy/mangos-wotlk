#ifndef MANGOS_TRANSPORT_INSTANCE_ROUTING_H
#define MANGOS_TRANSPORT_INSTANCE_ROUTING_H

#include <cstdint>

namespace MaNGOS
{
// Maps 0/1 use continent partitions. Other same-map routes must retain
// their owner's instance (notably the ICC gunships and phased map 609).
constexpr std::uint32_t TransportDestinationInstance(std::uint32_t currentMap,
    std::uint32_t currentInstance, std::uint32_t destinationMap,
    std::uint32_t continentInstance)
{
    return destinationMap <= 1 ? continentInstance
        : (currentMap == destinationMap ? currentInstance : 0);
}

// Only maps 0 and 1 are split into continent partitions. Instance-owned
// transports (ICC maps 672/673, phased maps, dungeons and raids) must continue
// normal movement inside their current instance even when the generic continent
// lookup returns zero.
constexpr bool TransportNeedsPartitionTransfer(std::uint32_t map,
    std::uint32_t currentInstance, std::uint32_t continentInstance)
{
    return map <= 1 && currentInstance != continentInstance;
}
}

#endif
