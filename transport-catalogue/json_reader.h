#pragma once

#include <vector>
#include <string>
#include <map>
#include <utility>
#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"
#include <sstream>
#include "json_builder.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

using namespace TC;
class JsonReader {
    private:
    json::Document document_;
    
    void AddStops(const json::Array& base_requests, TransportCatalogue& catalogue);
    void AddRouteDistances(const json::Array& base_requests, TransportCatalogue& catalogue);
    void AddBuses(const json::Array& base_requests, TransportCatalogue& catalogue);
    void ApplyBaseCommands(const json::Array& base_requests, TransportCatalogue& catalogue);
    void ApplyGetBuses(RequestHandler& request_handler, json::Builder& builder, const json::Dict& request);
    void ApplyGetStops(RequestHandler& request_handler, json::Builder& builder, const json::Dict& request);
    json::Node ApplyStatCommands(const json::Array& stat_requests, RequestHandler& request_handler);
    void ApplyGetMap(RequestHandler& request_handler, json::Builder& builder);
    svg::Color SetColor(const json::Node& render_setting);
        
    public:
    JsonReader(json::Document document)
    : document_(std::move(document)){
    }
    
    RenderSettings GetRenderSettings();
    
    json::Document ApplyCommands(RequestHandler& request_handler, TransportCatalogue& catalogue);
};
