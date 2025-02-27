#ifndef __UNDO_HPP__
#define __UNDO_HPP__

#include "types.hpp"
#include "array.hpp"
#include "string.hpp"

class Undo {
public:
  Array<unsigned char> undoMemory;
  Array<int64_t> undoStart;
  Array<int64_t> undoEnd;
  Array<String> undoDescs;
  int undoPos;
  int64_t undoMemoryPosition;

  Undo(int64_t undoMemorySize) {
    undoMemoryPosition = 0;
    undoPos=0;
    undoMemory.resize(undoMemorySize);
  }

  virtual ~Undo() {
  }

  virtual void reset() {
    undoMemoryPosition = 0;
    undoPos=0;
    undoDescs.clear();
    undoStart.clear();
    undoEnd.clear();
  }

  void addByte(unsigned char r) {
    undoMemory[undoMemoryPosition % undoMemory.size()] = r;
    undoMemoryPosition++;
  }

  bool addUndo(const String &s, const String &desc) {
    return addUndo(&(s[0]),s.length(),desc);
  }

  unsigned char getByte(int64_t pos) {
    return undoMemory[pos % undoMemory.size()];
  }

  String doUndo(String &s) {
    if (undoPos>0&&undoPos<=undoStart.size()) {
      String desc = undoDescs[undoPos-1];
      int64_t start = undoStart[undoPos-1];
      int64_t end = undoEnd[undoPos-1];
      s.resize(end-start);
      for (int64_t i = 0; i < end-start; i++) {
        s[i]=getByte(i+start);
      }
      undoPos--;
      return desc;
    }
    return NIL;
  }

  String doRedo(String &s) {
    if (undoPos<undoStart.size()-1) {
      String desc = undoDescs[undoPos+1];
      int64_t start = undoStart[undoPos+1];
      int64_t end = undoEnd[undoPos+1];
      s.resize(end-start);
      for (int64_t i = 0; i < end-start; i++) {
        s[i]=getByte(i+start);
      }
      undoPos++;
      return desc;
    }
    return NIL;
  }

  bool addUndo(void *data, int64_t memorySize, const String &desc) {
    if (undoPos>=undoStart.size()) {
      undoStart.resize(undoPos+1);
      undoEnd.resize(undoPos+1);
      undoDescs.resize(undoPos+1);
    }
    undoStart[undoPos] = undoMemoryPosition;
    unsigned char *r = (unsigned char*)data;
    for(int x = 0; x < memorySize; x++) {
      addByte(*r++);
    }
    undoEnd[undoPos] = undoMemoryPosition;
    undoDescs[undoPos] = desc;
    undoPos++;
    return true;
  }
};

#endif //__UNDO_HPP__
