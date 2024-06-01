#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Document ----------------
    
void Document::Render(std::ostream& out) const {
    RenderContext ctx(out, 2, 2);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
    for (auto& obj : objects_) {
        obj->Render(ctx);
    }
    out << "</svg>"sv;
}
    
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(std::move(obj));
}
    
// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    // Выводим атрибуты, унаследованные от PathProps
    RenderAttrs(context.out);
    out << "/>"sv;
}
    
// ---------- PolyLine ----------------
    
void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    bool first = true;
    for (auto point : points_) {
        if (first) {
            out << point.x << "," << point.y;
            first = false;
        } else {
            out << " " << point.x << "," << point.y; /////// space issue!!!
        }
    }
    out << "\"";
    RenderAttrs(context.out);
    out << "/>"sv;
}
    
Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}

// ---------- Text --------------------   

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text";
    RenderAttrs(context.out);
    out << " x=\"" << pos_.x << "\" " << "y=\"" << pos_.y << "\" ";
    out << "dx=\"" << offset_.x << "\" " << "dy=\"" << offset_.y << "\" ";
    out << "font-size=\"" << size_ << "\"";
    if (font_family_ != "") out << " font-family=\"" << font_family_ << "\" ";
    if (font_weight_ != "") out << "font-weight=\"" << font_weight_ << "\">";
    else out << ">";
    out << data_ << "</text>";
    
}    
    
Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}

Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

Text& Text::SetFontSize(uint32_t size) {
    size_ = size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

Text& Text::SetData(std::string data) {
    data_ = data;
    return *this;
}
    
}  // namespace svg


