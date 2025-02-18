#ifndef __ISTRLST_HPP__
#define __ISTRLST_HPP__

#include "array.hpp"
#include "string.hpp"

class IStrLst {
public:
  virtual void assigned() = 0;
  virtual String getString(int x, int y) = 0;
  virtual void sort(int x, bool ascending) = 0;
  virtual void select(int y, bool selected) = 0;
  virtual bool selected(int y) = 0;
};

#endif //__ISTRLST_HPP__