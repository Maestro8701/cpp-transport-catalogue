#include "json_reader.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

    void JsonReader::AddStops(const json::Array& base_requests, TransportCatalogue& catalogue) {
        for (const json::Node& request_node : base_requests) {
            const json::Dict& request = request_node.AsMap();
            if (request.at("type").AsString() == "Stop") {
                catalogue.AddStop(request.at("name").AsString(), {request.at("latitude").AsDouble(), request.at("longitude").AsDouble()});
            }
        }
    }
    
    void JsonReader::AddRouteDistances(const json::Array& base_requests, TransportCatalogue& catalogue) {
        for (const json::Node& request_node : base_requests) {
            const json::Dict& request = request_node.AsMap();
            if (request.at("type").AsString() == "Stop") {
                const json::Dict& distances = request.at("road_distances").AsMap();
                for (auto it = distances.begin(); it != distances.end(); it++) {
                    catalogue.AddRouteDistance(request.at("name").AsString(), {it->second.AsDouble(), it->first});
                }
            }
        }
    }
    
    void JsonReader::AddBuses(const json::Array& base_requests, TransportCatalogue& catalogue) {
        for (const json::Node& request_node : base_requests) {
            const json::Dict& request = request_node.AsMap();
            if (request.at("type").AsString() == "Bus") {
                auto& proxy = request.at("stops").AsArray();
                std::vector<std::string_view> stops_names; //////////// STRING_VIEW ISSUE
                for (auto& stop_name : proxy) {
                    stops_names.push_back(stop_name.AsString());
                }
                if (request.at("is_roundtrip").AsBool() == false) {
                   for (int i = stops_names.size() - 2; i >= 0; --i) {
                       stops_names.push_back(stops_names[i]);            
                    }
                }
                catalogue.AddBus(request.at("name").AsString(), stops_names, request.at("is_roundtrip").AsBool());
            }
        }    
    }
    
    void JsonReader::ApplyBaseCommands(const json::Array& base_requests, TransportCatalogue& catalogue) {
        AddStops(base_requests, catalogue);
        AddRouteDistances(base_requests, catalogue);
        AddBuses(base_requests, catalogue);
    }
    
    void JsonReader::ApplyGetBuses(RequestHandler& request_handler, json::Dict& response, const json::Dict& request) {
        auto bus_info = request_handler.GetBusInfo(request.at("name").AsString());
        if (bus_info.bus_exists) {
            response["curvature"] = json::Node(bus_info.curvature);
            response["route_length"] = json::Node(bus_info.length_route);
            response["stop_count"] = json::Node(static_cast<int>(bus_info.count_stops));
            response["unique_stop_count"] = json::Node(static_cast<int>(bus_info.count_unique_stops));
        } else {
            response["error_message"] = json::Node(std::string("not found"));
        }
    }
    
    void JsonReader::ApplyGetStops(RequestHandler& request_handler, json::Dict& response, const json::Dict& request) {
        json::Array buses;
        StopInfo stop_info = request_handler.GetBusesByStop(request.at("name").AsString());
        for (auto& bus : stop_info.buses) {
            buses.push_back(json::Node(std::string({bus.begin(), bus.end()})));
        }
        if (stop_info.stop_exists) {
            response["buses"] = std::move(json::Node(std::move(buses)));
        } else {
            response["error_message"] = json::Node(std::string("not found"));
        }
    }

    void JsonReader::ApplyGetMap(RequestHandler& request_handler, json::Dict& response) {
        std::stringstream stream;
        request_handler.RenderMap(stream);
        std::string str = std::move(stream.str());
        response["map"] = std::move(json::Node(std::move(str)));
    }

    json::Node JsonReader::ApplyStatCommands(const json::Array& stat_requests, RequestHandler& request_handler) {
        json::Array response_array;
        for (const json::Node& request_node : stat_requests) {
            const json::Dict& request = request_node.AsMap();
            json::Dict response;
            response["request_id"] = request.at("id").AsInt();
            if (request.at("type").AsString() == "Bus") {
                ApplyGetBuses(request_handler, response, request);
            } else if (request.at("type").AsString() == "Stop") {
                ApplyGetStops(request_handler, response, request);
            } else if (request.at("type").AsString() == "Map") {
                ApplyGetMap(request_handler, response); /////////////////////////////// ADD METHOD
            }
            response_array.push_back(std::move(response));
        }
        return json::Node(std::move(response_array));
    }

    svg::Color JsonReader::SetColor(const json::Node& render_setting) {
        svg::Color color;
        if (render_setting.IsArray()) {
            std::vector<double> proxy;
            for (const json::Node& temp : render_setting.AsArray()) {
                proxy.push_back(temp.AsDouble());
            }
            if (proxy.size() == 3) {
                color = svg::Color(svg::Rgb(proxy[0],proxy[1],proxy[2]));
            } else {
                color = svg::Color(svg::Rgba(proxy[0],proxy[1],proxy[2],proxy[3]));
            }
        } else {
            color = render_setting.AsString();
        }
        return color;
    }

    RenderSettings JsonReader::GetRenderSettings() {
        RenderSettings settings;
        const json::Dict& root =  document_.GetRoot().AsMap();
        const json::Dict& render_settings = root.at("render_settings").AsMap();
        settings.width = render_settings.at("width").AsDouble();
        settings.height = render_settings.at("height").AsDouble();
        settings.padding = render_settings.at("padding").AsDouble();
        settings.line_width = render_settings.at("line_width").AsDouble();
        settings.stop_radius = render_settings.at("stop_radius").AsDouble();
        settings.bus_label_font_size = render_settings.at("bus_label_font_size").AsInt();
        for (auto& temp : render_settings.at("bus_label_offset").AsArray()) {
            settings.bus_label_offset.push_back(temp.AsDouble());
        }
        settings.stop_label_font_size = render_settings.at("stop_label_font_size").AsInt();
        for (auto& temp : render_settings.at("stop_label_offset").AsArray()) {
            settings.stop_label_offset.push_back(temp.AsDouble());
        }
        settings.underlayer_width = render_settings.at("underlayer_width").AsDouble();
        
        settings.underlayer_color = SetColor(render_settings.at("underlayer_color"));
        
        for (const json::Node& color : render_settings.at("color_palette").AsArray()) {
            settings.color_palette.push_back(SetColor(color));
        }

        return settings;
    }
    
    json::Document JsonReader::ApplyCommands(RequestHandler& request_handler, TransportCatalogue& catalogue) {
        const json::Dict& root =  document_.GetRoot().AsMap();
        const json::Array& base_requests = root.at("base_requests").AsArray();
        const json::Array& stat_requests = root.at("stat_requests").AsArray();       
        ApplyBaseCommands(base_requests, catalogue);
        return json::Document(std::move(ApplyStatCommands(stat_requests, request_handler)));
    }
