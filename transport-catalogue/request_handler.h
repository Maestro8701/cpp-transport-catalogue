#pragma once

#include <optional>
#include <set>
#include <string>
#include <string_view>
#include "transport_catalogue.h"
#include "map_renderer.h"


/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
using namespace TC;
class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    //RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);
    RequestHandler(const TransportCatalogue& db, MapRenderer& renderer) 
    : db_(db)
    , renderer_(renderer) {
    }

    // Возвращает информацию о маршруте (запрос Bus)
    BusInfo GetBusInfo(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    //const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;
    const StopInfo GetBusesByStop(const std::string_view& stop_name) const;
    
    const std::map<std::string_view, Bus*>& GetRoutes() const;
    
    const std::map<std::string_view, Stop*>& GetStops() const;
    
    const std::unordered_map<Stop*, std::set<std::string_view>>& GetStopToBuses() const;
    // Этот метод будет нужен в следующей части итогового проекта
    //svg::Document RenderMap(std::ostream& out) const {
    void RenderMap(std::ostream& out) const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& db_;
    MapRenderer& renderer_;
};
