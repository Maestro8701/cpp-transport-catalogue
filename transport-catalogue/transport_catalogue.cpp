#include <set>
#include "transport_catalogue.h"
#include <iostream>
#include <string>
#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "geo.h"

namespace TC {
 
void TransportCatalogue::AddStop(const std::string& name, geo::Coordinates coordinates) {
    stops_.push_back(Stop(name, coordinates));

    Stop* stop = &(stops_.back());
    std::string_view stop_view(stop->name);
    stopname_to_stop_[stop_view] = stop; /////////// ALERT /////////////
}

void TransportCatalogue::AddRouteDistance(const std::string& name, const std::pair<double, std::string>& stop_pair) {
    route_distances_.insert({{stopname_to_stop_.at(name), stopname_to_stop_.at(stop_pair.second)}, stop_pair.first}); 
}    
    
void TransportCatalogue::AddBus(std::string_view name, const std::vector<std::string_view>& stops_name, bool is_roundtrip) {
    std::string str(name.begin(), name.end());
    std::vector<Stop*> stops; 
    for (const std::string_view& item : stops_name) {
        stops.push_back(stopname_to_stop_.at(item));
    } // все остановки добавлены
 
    auto& ref = buses_.emplace_back(Bus(name, stops, is_roundtrip));
    busname_to_bus_.insert({ std::string_view(ref.name), &ref});
 
    // добавление в индекс stop_to_bus
    for (auto& item : stops) {
        stop_to_buses_[item].insert(busname_to_bus_.at(name)->name);
    }   
}
 
BusInfo TransportCatalogue::GetBusInfo(std::string_view name) const {
    BusInfo result;
 
    auto iter = busname_to_bus_.find(name);
    if (iter == busname_to_bus_.end()) {
        result.bus_exists = false;
        return result;
    }

    const Bus& bus = *iter->second;
    result.bus_exists = true;
    result.count_stops = bus.stops.size();
 
    std::set<const Stop*> unique_stops(bus.stops.begin(), bus.stops.end());
    result.count_unique_stops = unique_stops.size();
 
    double length = 0.0;
    double length_real = 0.0;
    for (auto first = bus.stops.begin(); first != bus.stops.end(); ++first) {
        auto second = std::next(first);
        if (second == bus.stops.end()) {
            break;
        }
        length += ComputeDistance((**first).coordinates, (**second).coordinates);
        if (route_distances_.count({*first, *second})) {
            length_real += route_distances_.at({*first, *second});
        } else if (route_distances_.count({*second, *first})) {
            length_real += route_distances_.at({*second, *first});
        } else {
            length_real += ComputeDistance((**first).coordinates, (**second).coordinates);
        }
    }    
    
    result.length_route = length_real;
    result.curvature = length_real / length;
    return result;
}
 
StopInfo TransportCatalogue::GetStopInfo(std::string_view stop_name) const {
    Stop* stop;
    StopInfo stop_info;
    if (stopname_to_stop_.count(stop_name) > 0) {
        stop = stopname_to_stop_.at(stop_name);        
        stop_info.stop_exists = true;
        if (stop_to_buses_.count(stop) > 0) {
            stop_info.buses = stop_to_buses_.at(stop);

        }
    }
    return stop_info;
}
    
const std::map<std::string_view, Bus*>& TransportCatalogue::GetRoutes() const { //////////////// NEW ISSUE
    return busname_to_bus_;
}
    
const std::map<std::string_view, Stop*>& TransportCatalogue::GetStops() const {
    return stopname_to_stop_;
}
    
const std::unordered_map<Stop*, std::set<std::string_view>>& TransportCatalogue::GetStopToBuses() const {
    return stop_to_buses_;
}

}
