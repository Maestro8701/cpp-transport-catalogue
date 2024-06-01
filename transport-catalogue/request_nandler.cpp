#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

    BusInfo RequestHandler::GetBusInfo(const std::string_view& bus_name) const {
        return db_.GetBusInfo(bus_name);
    }

    // Возвращает маршруты, проходящие через
    //const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;
    const StopInfo RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
        return std::move(db_.GetStopInfo(stop_name));
    }

    const std::map<std::string_view, Bus*>& RequestHandler::GetRoutes() const {
        return db_.GetRoutes();
    }

    const std::map<std::string_view, Stop*>& RequestHandler::GetStops() const {
        return db_.GetStops();
    }
    
    const std::unordered_map<Stop*, std::set<std::string_view>>& RequestHandler::GetStopToBuses() const {
        return db_.GetStopToBuses();
    }
    // Этот метод будет нужен в следующей части итогового проекта
    //svg::Document RenderMap(std::ostream& out) const {
    void RequestHandler::RenderMap(std::ostream& out) const {
        renderer_.SetProjector(GetRoutes());
        renderer_.CreateDocument(GetRoutes(), GetStops(), GetStopToBuses());
        renderer_.Render(out);
    }
