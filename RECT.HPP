#ifndef __RECT_HPP__
#define __RECT_HPP__

#include "point.hpp"

class Rect {
public:
        int x0,y0,x1,y1;
        Rect() {x0 = 0; y0 = 0; x1 = 0; y1 = 0;}
        Rect(int _x0, int _y0, int _x1, int _y1) {x0 = _x0; y0 = _y0; x1 = _x1; y1 = _y1;}
        Rect(const Point &p0, const Point &p1) {x0 = p0.x; y0 = p0.y; x1 =p1.x; y1 = p1.y;}
        Rect(const Point &p0, const Size &s) {x0 = p0.x; y0 = p0.y; x1 =p0.x+s.width; y1 = p0.y+s.height;}
};

#endif //__RECT_HPP__