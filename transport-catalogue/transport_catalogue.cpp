// место для вашего кода
#include "transport_catalogue.h"

namespace transport {

//добавление маршрута в базу, то есть добавляем структуру BUS в деку buses
void Catalogue::AddRoute(const std::string& number, const std::vector<std::string>& bus_stops) {
    buses_.push_back({number, bus_stops});
    buses_info_[buses_.back().number] =  &buses_.back();

    for (const auto& stop : bus_stops) {
        if (stops_info_.count(stop)) buses_on_stops_[stops_info_[stop]->name].insert(number);
    }
}

//добавление остановки в базу, то есть добавляем структуру STOP в деку stops
void Catalogue::AddStop(const std::string& name, const geo::Coordinates& coordinates) {
    stops_.push_back({name, coordinates});
    stops_info_[stops_.back().name] = &stops_.back();
    //buses_on_stops_[name] = {};
}

//поиск маршрута по имени, то есть ищем структуру BUS в справочнике buses_info
const Bus* Catalogue::FindRoute(const std::string& number) const {
    return buses_info_.count(number) ? buses_info_.at(number) : nullptr;
}
    
//поиск остановки по имени, то есть ищем структуру STOP в деке stops
const Stop* Catalogue::FindStop(const std::string& name) const {
    return stops_info_.count(name) ? stops_info_.at(name) : nullptr;
}
    
//получение информации о маршруте, то есть оформляем структуру BusAnswer
const BusAnswer Catalogue::FindInfo (const std::string& number) const {
    BusAnswer busanswer;
    const Bus* bus = FindRoute(number);
        
    //if (!bus) throw std::invalid_argument("bus not found");
        
    busanswer.stops_count = bus->bus_stops.size();
    busanswer.uniq_stops_count = CalcUniqStops(number);
        
    double route_length = 0;
    for (auto iter = bus->bus_stops.begin(); iter+1 != bus->bus_stops.end(); ++iter) {
        route_length += geo::ComputeDistance (stops_info_.at(*iter)->coordinates, stops_info_.at(*(iter+1))->coordinates);
    }
    busanswer.route_length = route_length;
    return busanswer;
}
    
// поиск маршрутов проезжающих через остановку
const std::set<std::string_view> Catalogue::FindBusesOnStop (const std::string& name) const {
    //return stops_info_.at(name)->bus_tranzit;
    if (buses_on_stops_.count(name)) {
        return buses_on_stops_.at(name);
    } else {
        return {};
    }
}
} //namespace transport
