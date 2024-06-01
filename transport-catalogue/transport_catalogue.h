#pragma once

#include <set>
#include <iostream>
#include <string>
#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <map>
#include <vector>
#include "geo.h"

namespace TC {

struct Stop {
	std::string name;
	geo::Coordinates coordinates;
 
	explicit Stop(std::string name, geo::Coordinates coordinates) :
		name(name), coordinates(coordinates) {}
};
 
struct Bus {
	std::string name;
	std::vector<Stop*> stops;
    bool is_roundtrip;
 
	explicit Bus(std::string_view name, std::vector<Stop*> stops, bool is_roundtrip) :
		name(name), stops(stops), is_roundtrip(is_roundtrip) {}
};
 
struct BusInfo {
	bool bus_exists = false;
	size_t count_stops = 0;
	size_t count_unique_stops = 0;
	double length_route = 0;
    double curvature = 1;
};
 
struct StopInfo {
	bool stop_exists = false;
	std::set<std::string_view> buses{};
};

class StopPairHasher {
    public:
    
    std::size_t operator()(const std::pair<TC::Stop*, TC::Stop*>& stop_pair) const {
        std::size_t s1 = std::hash<const void*>{}(stop_pair.first);
        std::size_t s2 = std::hash<const void*>{}(stop_pair.second);
        return s1 + s2 * 7;
    }
};       
    
class TransportCatalogue {
	// Реализуйте класс самостоятельно
	public:
		void AddStop(const std::string& name, geo::Coordinates coordinates);
 
        void AddRouteDistance(const std::string& name, const std::pair<double, std::string>& stop_pair);    

		void AddBus(std::string_view name, const std::vector<std::string_view>& stops_name, bool is_roundtrip);
 
		BusInfo GetBusInfo(std::string_view name) const;
 
		StopInfo GetStopInfo(std::string_view stop_name) const;
         
        const std::map<std::string_view, Bus*>& GetRoutes() const;
    
        const std::map<std::string_view, Stop*>& GetStops() const;
    
        const std::unordered_map<Stop*, std::set<std::string_view>>& GetStopToBuses() const;
    
        
            
	private:
		std::deque<Stop> stops_;
		std::map<std::string_view, Stop*> stopname_to_stop_;
 
		std::deque<Bus> buses_;
		std::map<std::string_view, Bus*> busname_to_bus_;
 
		// индекс указатель на остановки - множество автобусов
		std::unordered_map<Stop*, std::set<std::string_view>> stop_to_buses_;
    
        std::unordered_map<std::pair<Stop*, Stop*>, double, StopPairHasher> route_distances_;
}; 
    
}
