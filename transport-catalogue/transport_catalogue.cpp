#include "transport_catalogue.h"


namespace transport_catalogue {

void TransportCatalogue::AddBus(const std::string& id, const std::vector<std::string>& stops) {
    buses_.push_back({ id, stops });
    for(const auto& stop : stops){
        buses_for_stop_[stop].insert(id);
    }  
}

void TransportCatalogue::AddStop(const std::string& id, geo::Coordinates& coordinates) {
    stops_.push_back({ id, coordinates });
}

void TransportCatalogue::AddDistances(const std::string& id, const std::unordered_map<std::string, unsigned int>& distances) {
    auto start_stop = FindStop(id);
    for (auto i = distances.begin(); i != distances.end(); i++) {
        auto reach_stop = FindStop(i->first);
        auto distance = i->second;
        distances_.insert({ {start_stop, reach_stop}, distance });
    }
}

const Bus* TransportCatalogue::FindBus(const std::string& id) const {    
    auto i = std::find_if(buses_.begin(), buses_.end(), [&] (const auto& bus) {
        return bus.id == id;
    });
    
    if (i == buses_.end()){
        return nullptr;
    }
    else {
        return &(*i);
    }
}

const Stop* TransportCatalogue::FindStop(const std::string& id) const {
    auto i = std::find_if(stops_.begin(), stops_.end(), [&] (const auto& stop) {
        return stop.id == id;
    });
    
    if (i == stops_.end()){
        return nullptr;
    }
    else {
        return &(*i);
    }
}

unsigned int TransportCatalogue::GetDistance(const Stop* start_stop, const Stop* reach_stop) const {
    if (distances_.count({ start_stop, reach_stop })) {
        return distances_.at({ start_stop, reach_stop });
    }
    else if (distances_.count({ reach_stop, start_stop })) {
        return distances_.at({ reach_stop, start_stop });
    }
    return 0;
}

BusInfo TransportCatalogue::GetBusInfo(const Bus* bus) const {
   
    BusInfo info;
    info.stops = bus->stops.size();
    
    std::unordered_set<std::string_view> unique_stops;
    for (const auto& stop : bus->stops) {
        unique_stops.insert(stop);
    }
    info.unique_stops = unique_stops.size();

    unsigned int route_length = 0;
    double geo_length = 0.0;
    for (auto i = bus->stops.begin() + 1; i != bus->stops.end(); i++) {
        auto first = std::find_if(stops_.begin(), stops_.end(), [&] (const auto& stop) {
            return stop.id == *(i - 1);
        });
        auto second = std::find_if(stops_.begin(), stops_.end(), [&] (const auto& stop) {
            return stop.id == *(i);
        });
        route_length += GetDistance(FindStop(first->id), FindStop(second->id));
        geo_length += geo::ComputeDistance(first->coordinates, second->coordinates);
    }
    info.route_length = route_length;
    info.curvature = (double)route_length / geo_length;
    
    return info;
}

std::set<std::string> TransportCatalogue::GetStopInfo(const Stop* stop) const {
    
    if (!buses_for_stop_.count(stop->id) || buses_for_stop_.at(stop->id).empty()) {
        return {};
    }
    
    return buses_for_stop_.at(stop->id);
}
    
} //transport_catalogue
