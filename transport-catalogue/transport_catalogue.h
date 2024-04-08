#pragma once

#include "geo.h"

#include <algorithm>
#include <deque>
#include <string>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace transport_catalogue {

struct Bus {
    std::string id;
    std::vector<std::string> stops;
};
    
struct BusInfo {
    size_t stops;
    size_t unique_stops;
    unsigned int route_length;
    double curvature;
};

struct Stop {
    std::string id;
    geo::Coordinates coordinates;
};

class TransportCatalogue {
public:

    struct DistancesHasher {
        size_t operator()(const std::pair<const Stop*, const Stop*>& stops) const {
            size_t hash_first = std::hash<const void*>{}(stops.first);
            size_t hash_second = std::hash<const void*>{}(stops.second);
            return hash_first + hash_second * 37;
        }
    };
    
    void AddBus(const std::string& id, const std::vector<std::string>& stops);
    void AddStop(const std::string& id, geo::Coordinates& coordinates);
    void AddDistances(const std::string& id, const std::unordered_map<std::string, unsigned int>& distances);
    const Bus* FindBus(const std::string& id) const;
    const Stop* FindStop(const std::string& id) const;
    unsigned int GetDistance(const Stop* start_stop, const Stop* reach_stop) const;
    BusInfo GetBusInfo(const Bus* bus) const;
    std::set<std::string> GetStopInfo(const Stop* stop) const;

private:
    std::deque<Bus> buses_;
    std::deque<Stop> stops_;
    std::unordered_map<std::string, std::set<std::string>> buses_for_stop_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, unsigned int, DistancesHasher> distances_;
};
    
} //transport_catalogue
