#pragma once

#include "GLMHelper.h"

namespace lava {

// Simple rectangle class.
// If NUM is an integer, contains all points from tl up to but not including br.
template<typename NUM, glm::precision precision=glm::highp>
class Rect
{
public:
    typedef glm::detail::tvec2<NUM, precision> Vec2;
    Vec2 tl;
    Vec2 br;

    Rect() = default;
    Rect(NUM left, NUM top, NUM right, NUM bottom);
    Rect(const Vec2& TL, const Vec2& BR);
    template<typename ORIGNUM, glm::precision origPrecision>
            explicit Rect(const Rect<ORIGNUM, origPrecision>& rc);

    bool operator ==(const Rect<NUM, precision>& rect) const;
    bool operator !=(const Rect<NUM, precision>& rect) const;
    NUM width() const;
    NUM height() const;
    Vec2 center() const;
    void setWidth(NUM width);
    void setHeight(NUM height);
    void setSize(const Vec2& size);
    bool contains(const Vec2& pt) const;
    bool contains(const Rect<NUM, precision>& rect) const;
    bool intersects(const Rect<NUM, precision>& rect) const;
    void expand(const Rect<NUM, precision>& rect);
    void expand(const Vec2& pt);
    void intersect(const Rect<NUM, precision>& rect);
    Vec2 size() const;
};

typedef Rect<float, glm::highp> FRect;
typedef Rect<int, glm::highp> IRect;

template<typename NUM, glm::precision precision>
std::ostream& operator<<( std::ostream& os, const Rect<NUM, precision> &r)
{
    os << "(" << r.tl << "-" << r.br << ")";
    return os;
}

template<typename NUM, glm::precision precision>
Rect<NUM, precision>::Rect(const Vec2& TL, const Vec2& BR)
    : tl(TL), br(BR)
{}

template<typename NUM, glm::precision precision>
Rect<NUM, precision>::Rect(NUM left, NUM top, NUM right, NUM bottom) 
    : tl(left, top), 
      br(right, bottom)
{}

template<typename NUM, glm::precision precision>
template<class ORIGNUM, glm::precision origPrecision>
Rect<NUM, precision>::Rect(const Rect<ORIGNUM, origPrecision>& rc)
    : tl (NUM(rc.tl.x), NUM(rc.tl.y)),
      br (NUM(rc.br.x), NUM(rc.br.y))
{
}

template<typename NUM, glm::precision precision>
bool Rect<NUM, precision>::operator ==(const Rect<NUM, precision> & rect) const
{
  return (tl == rect.tl && br == rect.br);
}

template<typename NUM, glm::precision precision>
bool Rect<NUM, precision>::operator !=(const Rect<NUM, precision> & rect) const
{
  return !(rect==*this);
}

template<typename NUM, glm::precision precision>
NUM Rect<NUM, precision>::width() const
{
  return br.x-tl.x;
}

template<typename NUM, glm::precision precision>
NUM Rect<NUM, precision>::height() const
{
  return br.y-tl.y;
}

template<typename NUM, glm::precision precision>
glm::detail::tvec2<NUM, precision> Rect<NUM, precision>::center() const
{
    return Vec2(tl+br)/(NUM)2;
}

template<typename NUM, glm::precision precision>
void Rect<NUM, precision>::setWidth(NUM width)
{
    br.x = tl.x+width;
}
 
template<typename NUM, glm::precision precision>
void Rect<NUM, precision>::setHeight(NUM height)
{
    br.y = tl.y+height;
}
    
template<typename NUM, glm::precision precision>
void Rect<NUM, precision>::setSize(const Vec2& size)
{
    setWidth(size.x);
    setHeight(size.y);
}

template<typename NUM, glm::precision precision>
bool Rect<NUM, precision>::contains(const Vec2& pt) const
{
    return (pt.x >= tl.x && pt.x < br.x &&
        pt.y >= tl.y && pt.y < br.y);
}

template<typename NUM, glm::precision precision>
bool Rect<NUM, precision>::contains(const Rect<NUM, precision>& rect) const
{
    Vec2 brpt (rect.br.x-1, rect.br.y-1);
    return contains(rect.tl) && contains(brpt);
}

template<typename NUM, glm::precision precision>
bool Rect<NUM, precision>::intersects(const Rect<NUM, precision>& rect) const
{
    return !(rect.br.x <= tl.x || rect.tl.x >= br.x ||
      rect.br.y <= tl.y || rect.tl.y >= br.y);
}

template<typename NUM, glm::precision precision>
void Rect<NUM, precision>::expand(const Rect<NUM, precision>& rect)
{
    if (width() == 0 && height() == 0) {
        *this = rect;
    } else {
        tl.x = std::min(tl.x, rect.tl.x);
        tl.y = std::min(tl.y, rect.tl.y);
        br.x = std::max(br.x, rect.br.x);
        br.y = std::max(br.y, rect.br.y);
    }
}

template<typename NUM, glm::precision precision>
void Rect<NUM, precision>::expand(const Vec2& pt)
{
    tl.x = std::min(tl.x, pt.x);
    tl.y = std::min(tl.y, pt.y);
    br.x = std::max(br.x, pt.x);
    br.y = std::max(br.y, pt.y);
}

template<typename NUM, glm::precision precision>
void Rect<NUM, precision>::intersect(const Rect<NUM, precision>& rect)
{
    tl.x = std::max(tl.x, rect.tl.x);
    tl.y = std::max(tl.y, rect.tl.y);
    br.x = std::min(br.x, rect.br.x);
    br.y = std::min(br.y, rect.br.y);
}

template<typename NUM, glm::precision precision>
glm::detail::tvec2<NUM, precision> Rect<NUM, precision>::size() const
{
    return Vec2(width(), height());
}

}

