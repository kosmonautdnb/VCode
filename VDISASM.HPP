#ifndef __VDISASM_HPP__
#define __VDISASM_HPP__

#include "string.hpp"
#include "types.hpp"
#include "vtext.hpp"

void getDisassembly_6502(String &opcs, int32_t baseAddress, String &adrs, String &dats, const String &text, int32_t startAddress, int32_t endAddress);

class VDisAsm : public VText {
public:
  String data;

  int32_t baseAddress;
  String opcodes;
  String lineNumbers;
  String datas;

  VDisAsm() : VText() {
    baseAddress = 0;
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