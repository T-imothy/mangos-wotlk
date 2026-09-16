#include "Entities/TransportInstanceRouting.h"
#include <cstdlib>
#include <iostream>

int main()
{
    struct Case { unsigned map, instance, destination, partition, expected; };
    Case const cases[] = {
        {631, 42, 631, 0, 42}, // ICC route wraps inside the same saved raid
        {631, 9001, 631, 0, 9001}, // a second raid must not collapse into instance 0
        {631, 0, 631, 0, 0},
        {609, 67, 609, 0, 67}, // preserve phased non-continent map ownership
        {571, 0, 571, 0, 0},
        {0, 2, 0, 3, 3}, // same continent, different partition
        {0, 2, 0, 2, 2},
        {0, 2, 1, 5, 5}, // cross-continent boat
        {1, 5, 0, 2, 2},
        {0, 2, 530, 0, 0}, // destination is an ordinary world map
        {0, 2, 0, 0, 0}, // partitioning disabled
    };
    for (auto const& c : cases)
    {
        auto destination = MaNGOS::TransportDestinationInstance(c.map, c.instance, c.destination, c.partition);
        if (destination != c.expected)
        {
            std::cerr << "Transport instance routing failed for map " << c.map << '\n';
            return EXIT_FAILURE;
        }
        if (c.map == 631 && (c.map != c.destination || c.instance != destination))
            return EXIT_FAILURE; // must never enter CreateMap with a transport for this route
    }
    std::cout << "ICC ownership, phased maps and continent transport routing passed\n";
}
