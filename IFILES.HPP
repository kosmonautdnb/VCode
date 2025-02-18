#ifndef __IFILES_HPP__
#define __IFILES_HPP__

#include "dos.hpp"
#include "array.hpp"
#include "ifiles.hpp"
#include "istrlst.hpp"

class IFiles : public IStrLst {
public:
  Array<File> files;
  Array<int> sorting;
  Array<int> selection;
  int sortedBy;
  bool sortedAscending;
  virtual void assigned() {selection.clear(); sorting.clear(); sortedBy = -1;}
  virtual String getString(int x, int y);
  virtual void sort(int x, bool ascending);
  virtual void select(int y, bool selected);
  virtual bool selected(int y);
};

#endif //__IFILES_HPP__