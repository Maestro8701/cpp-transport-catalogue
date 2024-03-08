// место для вашего кода
#pragma once

# include "geo.h"

#include<vector>
#include<string>
#include<string_view>
#include<deque>
#include<unordered_map>
#include<unordered_set>
#include<optional>
#include<stdexcept>
#include<iostream>
#include<set>

namespace transport {

//информация об остановке
    struct Stop {
        std::string name; // название остановки
        geo::Coordinates coordinates; // координаты остановки
    };
    
    //информация о маршруте
    struct Bus {
        std::string number; //номер маршрута
        std::vector<std::string> bus_stops; // маршрут, список остановок
    };
    
    //информация для ответа на запрос о маршруте
    struct BusAnswer {
        int stops_count; // колличество остановок в запрашиваемом маршруте
        int uniq_stops_count; //колличество уникальных остановок в запрашиваемом маршруте
        double route_length; // протяженность маршрута
    };

class Catalogue {
    
// Реализуйте класс самостоятельно
    public:
    
//добавление маршрута в базу, то есть добавляем структуру BUS в деку buses
    void AddRoute(const std::string& number, const std::vector<std::string>& bus_stops);
    
//добавление остановки в базу, то есть добавляем структуру STOP в деку stops
    void AddStop(const std::string& name, const geo::Coordinates& coordinates);
    
//поиск маршрута по имени, то есть ищем структуру BUS в справочнике buses_info
    const Bus* FindRoute(const std::string& number) const;
    
//поиск остановки по имени, то есть ищем структуру STOP в справочнике stops_info
    const Stop* FindStop(const std::string& name) const;
    
//получение информации о маршруте, то есть оформляем структуру BusAnswer
    const BusAnswer FindInfo (const std::string& number) const;
    
//расчет количества уникальных остановок, то есть считаем количество неповторяющихся остановок на маршруте
   // int CalcUniqStops(const std::string& number) const;
    
// поиск маршрутов проезжающих через остановку
    const std::set<std::string_view> FindBusesOnStop (const std::string& name) const;
    
    private:
        std::deque<Bus> buses_; // маршруты
        std::deque<Stop> stops_; // остановки
        std::unordered_map<std::string_view, const Bus*> buses_info_; // справочник маршрутов
        std::unordered_map<std::string_view, const Stop*> stops_info_; // справочник остановок
        std::unordered_map<std::string_view, std::set<std::string_view>> buses_on_stops_;
    
        //расчет количества уникальных остановок, то есть считаем количество неповторяющихся остановок на маршруте
        int CalcUniqStops(const std::string& number) const {
            std::unordered_set<std::string> uniq_stops;
            for (const auto& stop : buses_info_.at(number)->bus_stops) {
                uniq_stops.insert(stop);
            }
            return uniq_stops.size();
        }
};
} // namespace transport
