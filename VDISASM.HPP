#ifndef __VDISASM_HPP__
#define __VDISASM_HPP__

#include "string.hpp"
#include "types.hpp"
#include "vtext.hpp"

void getDisassembly_6502(String &opcs, String &adrs, String &dats, const String &text, int64_t startAddress, int64_t endAddress);

class VDisAsm : public VText {
public:
  String data;

  String opcodes;
  String lineNumbers;
  String datas;

  VDisAsm() : VText() {
    opcodes = "";
    lineNumbers = "";
    datas = "";
  }
  virtual ~VDisAsm() {;}

  virtual void draw();
  virtual int update();

  void init(const String &_text, const String &_fileName);
};

#endif //__VDISASM_HPP__