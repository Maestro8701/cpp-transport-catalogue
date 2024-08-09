#include "transport_router.h"

namespace transport_router {   
    
TransportRouter::TransportRouter(const RoutingSettings& settings, const TransportCatalogue& db)
: settings_(settings), tc_(db) {
}

RoutingSettings TransportRouter::GetRoutingSettings() {
    return settings_;
}

RouteData TransportRouter::CalculateRoute(std::string_view from, std::string_view to) {
    if (!router_) {
        BuildGraph();
    }

    RouteData result; 
    auto calculated_route = router_->BuildRoute(vertexes_.at(from).wait, vertexes_.at(to).wait); 

    if (calculated_route) { 
        result.founded = true; 
        for (const auto& element_id : calculated_route->edges) { 
            auto edge_details = graph_.GetEdge(element_id); 
            result.total_time += edge_details.weight; 
            result.items.emplace_back(RouteItem{ 
                edges_info_.at(element_id).edge_name, 
                edges_info_.at(element_id).type == EdgeType::TRAVEL ? edges_info_.at(element_id).span_count : 0, 
                edge_details.weight, 
                edges_info_.at(element_id).type 
            }); 
        } 
    } 
    return result; 
}

void TransportRouter::BuildGraph() {
    const size_t total_stops = tc_.GetAllStopsCount();
    const double velocity_factor = settings_.bus_velocity * METERS_PER_KM / MIN_PER_HOUR;
    size_t vertex_id = 0;
    Graph graph(total_stops * 2);
    graph_ = std::move(graph);
    vertexes_.reserve(total_stops);

    // Добавляем остановки в граф
    AddStopsToGraph(vertex_id);

    // Добавляем автобусы в граф
    AddBusesToGraph(velocity_factor);
    
    router_ = std::make_unique<graph::Router<double>>(graph_); 
}

void TransportRouter::AddStopsToGraph(size_t& vertex_id) {
    for (const auto& [stopname, stop] : tc_.GetAllStops()) { 
        vertexes_.insert({ stopname, {vertex_id, vertex_id + 1} });
        ++vertex_id;

        auto edge_id = graph_.AddEdge({
            vertexes_.at(stopname).wait,
            vertexes_.at(stopname).travel,
            settings_.bus_wait_time
        });
        
        edges_info_.insert({ edge_id,
            { stop->stop_name,
              0, // span == 0 для ребра ожидания
              0.0,
              EdgeType::WAIT
            } 
        });

        ++vertex_id; 
    } 
}

void TransportRouter::AddBusesToGraph(double velocity_factor) {
    for (const auto& bus : tc_.GetAllBuses()) { 
        const size_t bus_stop_count = bus.stops.size(); 
        std::vector<double> distances(bus_stop_count, 0.0); 

        // Вычисляем расстояния между каждой парой остановок 
        for (size_t i = 1; i < bus_stop_count; ++i) { 
            distances[i] = distances[i - 1] + static_cast<double>(tc_.GetDistance(bus.stops[i - 1], bus.stops[i]).meters); 
        } 

        for (size_t it_from = 0; it_from < bus_stop_count - 1; ++it_from) { 
            int span_count = 0; 
            for (size_t it_to = it_from + 1; it_to < bus_stop_count; ++it_to) { 
                double road_distance = distances[it_to] - distances[it_from]; 
                auto edge_id = graph_.AddEdge({ 
                    vertexes_.at(bus.stops[it_from]->stop_name).travel, 
                    vertexes_.at(bus.stops[it_to]->stop_name).wait, 
                    road_distance / velocity_factor 
                }); 
                
                edges_info_.insert({ edge_id, 
                    { bus.bus_name, 
                      ++span_count, 
                      0.0, 
                      EdgeType::TRAVEL 
                    } 
                }); 
            } 
        } 
    } 
}
} // namespace transport_router
