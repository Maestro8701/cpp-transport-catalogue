#pragma once

#define _USE_MATH_DEFINES 
#include <cmath>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <sstream>

namespace svg {

struct Rgb {
    Rgb() = default;
    Rgb(uint8_t red, uint8_t green, uint8_t blue)
    : red(red)
    , green(green)
    , blue(blue) {
    }
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};
    
struct Rgba {
    Rgba() = default;
    Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity)
    : red(red)
    , green(green)
    , blue(blue)
    , opacity(opacity) {
    }
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
};   
    
using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

// Объявив в заголовочном файле константу со спецификатором inline,
// мы сделаем так, что она будет одной на все единицы трансляции,
// которые подключают этот заголовок.
// В противном случае каждая единица трансляции будет использовать свою копию этой константы
inline const Color NoneColor{"none"};  

struct ColorVisitorOut {
    std::ostream &out;
    
    void operator()(std::monostate) {
        out << "none";
    }
    
    void operator()(std::string str) {
        out << str;
    }
    
    void operator()(Rgb rgb) {
        std::string red = std::to_string(rgb.red);
        std::string green = std::to_string(rgb.green);
        std::string blue = std::to_string(rgb.blue);
        out << "rgb(" << red << "," << green << "," << blue << ")";
    }
    
    void operator()(Rgba rgba) {
        std::string red = std::to_string(rgba.red);
        std::string green = std::to_string(rgba.green);
        std::string blue = std::to_string(rgba.blue);
        //std::string opacity = std::to_string(rgba.opacity);
        out << "rgba(" << red << "," << green << "," << blue << "," << rgba.opacity << ")";
    }
};
    
inline std::ostream& operator<<(std::ostream& out, const Color& color) {
    std::ostringstream strm;
    std::visit(ColorVisitorOut{strm}, color);
    out << strm.str();
    return out;
}
    
struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};
    
enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

inline std::ostream& operator<< (std::ostream& out, const StrokeLineCap& str) {
  switch(str)
  {
    case StrokeLineCap::BUTT: out << "butt"; break;
    case StrokeLineCap::ROUND: out << "round"; break;
    case StrokeLineCap::SQUARE: out << "square"; break;
  }
  return out;
}    
    
enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

inline std::ostream& operator<< (std::ostream& out, const StrokeLineJoin& str) {
  switch(str)
  {
    case StrokeLineJoin::ARCS: out << "arcs"; break;
    case StrokeLineJoin::BEVEL: out << "bevel"; break;
    case StrokeLineJoin::MITER: out << "miter"; break;
    case StrokeLineJoin::MITER_CLIP: out << "miter-clip"; break;
    case StrokeLineJoin::ROUND: out << "round"; break;
  }
  return out;
}    
    
/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeWidth(double width) {
        width_ = std::move(width);
        return AsOwner();
    }
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_ = std::move(line_cap);
        return AsOwner();
    }
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_ = std::move(line_join);
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    // Метод RenderAttrs выводит в поток общие для всех путей атрибуты fill и stroke
    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_) out << " fill=\""sv << *fill_color_ << "\""sv;
        if (stroke_color_) out << " stroke=\""sv << *stroke_color_ << "\""sv;
        if (width_) out << " stroke-width=\""sv << *width_ << "\""sv;
        if (line_cap_) out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
        if (line_join_) out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
    }

private:
    Owner& AsOwner() {
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;
};    
    
/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;
    
    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline : public Object, public PathProps<Polyline> {
private:
    std::vector<Point> points_;
    
    void RenderObject(const RenderContext& context) const override;
    
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);

    /*
     * Прочие методы и данные, необходимые для реализации элемента <polyline>
     */
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text : public Object, public PathProps<Text> {
private:
    Point pos_;
    Point offset_;
    uint32_t size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;
    
    void RenderObject(const RenderContext& context) const override;
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

    // Прочие данные и методы, необходимые для реализации элемента <text>
};

class ObjectContainer {
protected:
    std::vector<std::unique_ptr<Object>> objects_;  
public:
    template <typename T>
    void Add(T obj) {
       objects_.emplace_back(std::make_unique<T>(std::move(obj))); 
    }
    
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    
    virtual ~ObjectContainer() = default;
};   
    
class Document : public ObjectContainer {
private:
        //std::vector<std::unique_ptr<Object>> objects_;
public:
    /*
     Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
     Пример использования:
     Document doc;
     doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
    */

    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

    // Прочие методы и данные, необходимые для реализации класса Document
}; 
    
class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;
    
    virtual ~Drawable() = default;
    
};
    
}  // namespace svg

namespace shapes { 
    
class Triangle : public svg::Drawable {
public:
    Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
        : p1_(p1)
        , p2_(p2)
        , p3_(p3) {
    }

    // Реализует метод Draw интерфейса svg::Drawable
    void Draw(svg::ObjectContainer& container) const override {
        container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }

private:
    svg::Point p1_, p2_, p3_;
};

class Star : public svg::Drawable {
private:
    svg::Point center_;
    double outer_rad_;
    double inner_rad_;
    int num_rays_;
    
public:
    Star(svg::Point center, double outer_rad, double inner_rad, int num_rays) 
    : center_(center)
    , outer_rad_(outer_rad)
    , inner_rad_(inner_rad)
    , num_rays_(num_rays) {
    }
    
    void Draw(svg::ObjectContainer& container) const override {
        container.Add(CreateStar(center_, outer_rad_, inner_rad_, num_rays_).SetFillColor("red").SetStrokeColor("black"));
    }
    
    static svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
    using namespace svg;
    Polyline polyline;
    for (int i = 0; i <= num_rays; ++i) {
        double angle = 2 * M_PI * (i % num_rays) / num_rays;
        polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
        if (i == num_rays) {
            break;
        }
        angle += M_PI / num_rays;
        polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
    }
    return polyline;
}   
    
};
class Snowman : public svg::Drawable {
private:
    svg::Point center_;
    double rad_;
public:
    Snowman(svg::Point center, double rad)
    : center_(center)
    , rad_(rad) {   
    }
    
    void Draw(svg::ObjectContainer& container) const override {
        svg::Point center_bottom(center_.x, center_.y + rad_ * 5); 
        container.Add(svg::Circle().SetCenter(center_bottom).SetRadius(rad_ * 2).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        svg::Point center_middle(center_.x, center_.y + rad_ * 2); 
        container.Add(svg::Circle().SetCenter(center_middle).SetRadius(rad_ * 1.5).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        container.Add(svg::Circle().SetCenter(center_).SetRadius(rad_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
    }    
};

} // namespace shapes     
