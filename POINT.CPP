#include "point.hpp"

Point operator+(const Point &a, const Point &b) {
  Point r;
  r.x = a.x + b.x;
  r.y = a.y + b.y;
  return r;
}

Point operator-(const Point &a, const Point &b) {
  Point r;
  r.x = a.x - b.x;
  r.y = a.y - b.y;
  return r;
}
