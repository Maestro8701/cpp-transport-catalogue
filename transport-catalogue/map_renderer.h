#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <array>
#include <vector>
#include "transport_catalogue.h"
#include "geo.h"
#include "svg.h"
/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

using namespace TC;

inline const double EPSILON = 1e-6;

class SphereProjector {
public:
    bool IsZero(double value);
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const;

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

struct RenderSettings {
    double width;
    double height;
    double padding;
    double line_width;
    double stop_radius;
    int bus_label_font_size;
    std::vector<double> bus_label_offset;
    int stop_label_font_size;
    std::vector<double> stop_label_offset;
    svg::Color underlayer_color;
    double underlayer_width;
    std::vector<svg::Color> color_palette;
    
    /*RenderSettings(const RenderSettings& settings)
    : width(settings.width)
    , height(settings.height)
    , padding(settings.padding)
    , line_width(settings.line_width)
    , stop_radius(settings.stop_radius)
    , bus_label_font_size(settings.bus_label_font_size)
    , bus_label_offset(settings.bus_label_offset)
    , stop_label_font_size(settings.stop_label_font_size)
    , stop_label_offset(settings.stop_label_offset)
    , underlayer_color(settings.underlayer_color)
    , underlayer_width(settings.underlayer_width)
    , color_palette(settings.color_palette) {
    }*/
};

class MapRenderer {
    private:
    RenderSettings settings_;
    svg::Document document_;
    
    public:
    
    /*MapRenderer(RenderSettings settings)
    : settings_(settings) {
    }*/
    
    void Render(std::ostream& out);
    
    void CreateDocument(const std::map<std::string_view, Bus*>& routes,
                        const std::map<std::string_view, Stop*>& stops,
                        const std::unordered_map<Stop*, std::set<std::string_view>>& stop_to_buses);
    
    void AddRoute(Bus* bus, svg::Color color, SphereProjector& projector);
    
    void AddRouteName(Bus* bus, svg::Color color,SphereProjector& projector);
    
    void AddStopCircle(Stop* stop, SphereProjector& projector);
    
    void AddStopName(Stop* stop, SphereProjector& projector);
    
    void SetSettings(RenderSettings settings);
    
    SphereProjector SetProjector(const std::map<std::string_view, Bus*>& routes);
};a
