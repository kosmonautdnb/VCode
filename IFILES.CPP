#include "ifiles.hpp"

String IFiles::getString(int x, int y) {
  if (y == -1) {
  String sort = (sortedBy == x) ? sortedAscending ? " +" : " -" : "  ";
  if (x == 0) return sort + "Name";
  if (x == 1) return sort + "Ext";
  if (x == 2) return sort + "Size";
  if (x == 3) return sort + "Date";
  }
  if (y >= 0 && y<sorting.size()) y = sorting[y];
  if (y < 0 || y >= files.size()) return NIL;
  if (x == 0) return files[y].name.name;
  if (x == 1) return files[y].name.extension;
  if (x == 2) return files[y].directory ? "<DIR>" : String(files[y].size);
  if (x == 3) return dateString(files[y].date);
  if (x == 4) return files[y].name.path + files[y].name.extension;
  return "";
}

void IFiles::sort(int x, bool ascending) {
  if (sortedBy==x&&sortedAscending) ascending=false;
  sortedBy = x;
  sortedAscending = ascending;
  if (x==-1) return;
  sorting.resize(files.size());
  for (int i = 0; i < sorting.size(); i++) sorting[i]=i;

  for (int j = 0; j < sorting.size(); j++) {
    for (int k = j + 1; k < sorting.size(); k++) {
      String a1 = getString(x,j);
      String a2 = getString(x,k);
      int i1 = atoi(a1.c_str());
      int i2 = atoi(a2.c_str());
      bool a = a1 < a2;
      if (i1 > 0 && i2 > 0)
        a = i1 > i2;
      if (!ascending) a = !a;
      if (files[sorting[j]].directory&&!files[sorting[k]].directory) a = false;
      if (!files[sorting[j]].directory&&files[sorting[k]].directory) a = true;
      if (a) {
        int t = sorting[j];
        sorting[j] = sorting[k];
        sorting[k] = t;
      }
    }
  }
}

void IFiles::select(int y, bool selected) {
  if (selection.size() != files.size()) {
    selection.resize(files.size());
    for (int i = 0; i < selection.size(); i++) selection[i]=false;
  }
  if (y >= 0 && y < selection.size())
    selection[y] = selected;
}

bool IFiles::selected(int y) {
  if (selection.size() != files.size()) {
    selection.resize(files.size());
    for (int i = 0; i < selection.size(); i++) selection[i]=false;
  }
  if (y >= 0 && y < selection.size())
    return selection[y];
  return false;
}