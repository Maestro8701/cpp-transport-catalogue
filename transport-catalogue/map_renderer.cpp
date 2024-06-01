#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

    bool SphereProjector::IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

    void MapRenderer::Render(std::ostream& out) {
        return document_.Render(out);
    }
    
    void MapRenderer::CreateDocument(const std::map<std::string_view, Bus*>& routes,
                        const std::map<std::string_view, Stop*>& stops,
                        const std::unordered_map<Stop*, std::set<std::string_view>>& stop_to_buses) {
        SphereProjector projector(SetProjector(routes));
        unsigned long i = 0;
        for (auto& [name, bus] : routes) {
            if (!bus->stops.empty()) {
                AddRoute(bus, settings_.color_palette[i], projector);
                ++i;
                if (i == settings_.color_palette.size()) {
                    i = 0;
                }
            }
        }
        i = 0;
        for (auto& [name, bus] : routes) {
            if (!bus->stops.empty()) {
                AddRouteName(bus, settings_.color_palette[i], projector);
                ++i;
                if (i == settings_.color_palette.size()) {
                    i = 0;
                }
            }
        }
        
        for (auto& [name, stop] : stops) {
            if (stop_to_buses.count(stop)) {
                AddStopCircle(stop, projector);
            }
        }
        
        for (auto& [name, stop] : stops) {
            if (stop_to_buses.count(stop)) {
                AddStopName(stop, projector);
            }
        }
    }
    
    void MapRenderer::AddRoute(Bus* bus, svg::Color color, SphereProjector& projector) {
        svg::Polyline line;
        for (auto& stop : bus->stops) {
            line.AddPoint(projector(stop->coordinates));
        }
    line.SetFillColor(svg::NoneColor).SetStrokeColor(color).SetStrokeWidth(settings_.line_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        document_.AddPtr(std::make_unique<svg::Polyline>(line));
    }
    
    void MapRenderer::AddRouteName(Bus* bus, svg::Color color,SphereProjector& projector) {
            svg::Text name;
            name.SetPosition(projector(bus->stops[0]->coordinates)).SetOffset({settings_.bus_label_offset[0],settings_.bus_label_offset[1]}).SetFontSize(settings_.bus_label_font_size).SetFontFamily("Verdana").SetFontWeight("bold").SetData(bus->name).SetFillColor(color);
            svg::Text under;
            under.SetPosition(projector(bus->stops[0]->coordinates)).SetOffset({settings_.bus_label_offset[0],settings_.bus_label_offset[1]}).SetFontSize(settings_.bus_label_font_size).SetFontFamily("Verdana").SetFontWeight("bold").SetData(bus->name).SetFillColor(settings_.underlayer_color).SetStrokeColor(settings_.underlayer_color).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeWidth(settings_.underlayer_width);
            document_.AddPtr(std::make_unique<svg::Text>(under));
            document_.AddPtr(std::make_unique<svg::Text>(name));
        
         if (!bus->is_roundtrip && bus->stops[0] != bus->stops[bus->stops.size() / 2]) {
            svg::Text name;
            name.SetPosition(projector(bus->stops[bus->stops.size() / 2]->coordinates)).SetOffset({settings_.bus_label_offset[0],settings_.bus_label_offset[1]}).SetFontSize(settings_.bus_label_font_size).SetFontFamily("Verdana").SetFontWeight("bold").SetData(bus->name).SetFillColor(color);
            svg::Text under;
            under.SetPosition(projector(bus->stops[bus->stops.size() / 2]->coordinates)).SetOffset({settings_.bus_label_offset[0],settings_.bus_label_offset[1]}).SetFontSize(settings_.bus_label_font_size).SetFontFamily("Verdana").SetFontWeight("bold").SetData(bus->name).SetFillColor(settings_.underlayer_color).SetStrokeColor(settings_.underlayer_color).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeWidth(settings_.underlayer_width);
            document_.AddPtr(std::make_unique<svg::Text>(under));
            document_.AddPtr(std::make_unique<svg::Text>(name));
         }
    }
    
    void MapRenderer::AddStopCircle(Stop* stop, SphereProjector& projector) {
        svg::Circle stop_circle;
        stop_circle.SetCenter(projector(stop->coordinates)).SetRadius(settings_.stop_radius).SetFillColor(svg::Color("white"));
        document_.AddPtr(std::make_unique<svg::Circle>(stop_circle));
    }
    
    void MapRenderer::AddStopName(Stop* stop, SphereProjector& projector) {
            svg::Text name;
            name.SetPosition(projector(stop->coordinates)).SetOffset({settings_.stop_label_offset[0],settings_.stop_label_offset[1]}).SetFontSize(settings_.stop_label_font_size).SetFontFamily("Verdana").SetData(stop->name).SetFillColor(svg::Color("black"));
            svg::Text under;
            under.SetPosition(projector(stop->coordinates)).SetOffset({settings_.stop_label_offset[0],settings_.stop_label_offset[1]}).SetFontSize(settings_.stop_label_font_size).SetFontFamily("Verdana").SetData(stop->name).SetFillColor(settings_.underlayer_color).SetStrokeColor(settings_.underlayer_color).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeWidth(settings_.underlayer_width);
            document_.AddPtr(std::make_unique<svg::Text>(under));
            document_.AddPtr(std::make_unique<svg::Text>(name));
    }
    
    void MapRenderer::SetSettings(RenderSettings settings) {
        settings_ = settings;
    }
    
    SphereProjector MapRenderer::SetProjector(const std::map<std::string_view, Bus*>& routes) {
        std::vector<geo::Coordinates> coordinates;
        for (auto it = routes.begin(); it != routes.end(); it++) {
            for (auto& stop : it->second->stops) {
                coordinates.push_back(stop->coordinates);
            }
        }
        return std::move(SphereProjector(coordinates.begin(), coordinates.end(), settings_.width, settings_.height, settings_.padding));
    }
