#include "vdisasm.hpp"
#include "array.hpp"
#include "syntax.hpp"
#include "constant.hpp"
#include "colors.hpp"
#include "dos.hpp"
#include "blit.hpp"
#include "vtxtalrt.hpp"

void VDisAsm::draw() {
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,COLOR_TEXTBACKGROUND);

  int i;

  int textLastLine = 0;
  for (i = 0; i < text.length(); i++) if (text[i]=='\n') textLastLine++;
  if (textLastLine>0) {
    scrollBoxY = textPos.y*size.height/(textLastLine*CHAR_HEIGHT);
    drawBox(position.x+size.width-4*frameBufferWidth/320,position.y+scrollBoxY,position.x+size.width,position.y+scrollBoxY+2*frameBufferHeight/200,COLOR_SCROLL);
  }

  Point pos = position - textPos;
  int line = 0;
  while(pos.y < position.y+size.height) {
    String lineNr = String(line+1);
    drawString(position.x+size.width-lineNr.pixelWidth(),pos.y,lineNr, COLOR_LINENUMBERS);
    pos.y += CHAR_HEIGHT;
    line++;
  }
  pos = position - textPos;
  int spx2 = pos.x; 
  pos.x += String("$00000000 $00 $00 $00 ").pixelWidth();
  int spx = pos.x; 
  for (i = 0; i < text.length(); i++) {
    char c = text[i];
    switch(c) {
      case '\n': {pos.y += CHAR_HEIGHT; pos.x = spx;continue;} break;
      case '\t': {pos.x = ((pos.x - (position.x - textPos.x)) / (CHAR_HEIGHT*2)+1)*(CHAR_HEIGHT*2)+(position.x-textPos.x);continue;} break;
    }
    drawChar(pos.x,pos.y,c,getHiLightColor(colors[i]));
    if (pos.y > position.y + size.height) break;
  }

  pos = position - textPos;
  for (i = 0; i < lineNumbers.length(); i++) {
    char c = lineNumbers[i];
    switch(c) {
      case '\n': {pos.y += CHAR_HEIGHT; pos.x = spx2;continue;} break;
    }
    drawChar(pos.x,pos.y,c,COLOR_LINENUMBERS);
    if (pos.y > position.y + size.height) break;
  }


  pos = position - textPos;
  pos.x += String("$00000000 ").pixelWidth();
  spx2 = pos.x;
  for (i = 0; i < datas.length(); i++) {
    char c = datas[i];
    switch(c) {
      case '\n': {pos.y += CHAR_HEIGHT; pos.x = spx2;continue;} break;
    }
    drawChar(pos.x,pos.y,c,COLOR_HILIGHT_COMMENT);
    if (pos.y > position.y + size.height) break;
  }

  if (i == text.length())
    drawStringOutlined(pos.x,pos.y,"<EOF>",COLOR_RED,COLOR_YELLOW);
  String status = String(textPos.x/CHAR_HEIGHT+1)+" "+String(textPos.y/CHAR_HEIGHT+1);
  status += " "+sizeString(text.length());
  status += " " + fileName.last(20);
  drawBox(position.x,position.y+size.height-CHAR_HEIGHT,position.x+size.width,position.y+size.height,COLOR_STATUSBAR);
  drawString(position.x,position.y+size.height-CHAR_HEIGHT,status,COLOR_STATUSBARTEXT);
  popRect();
}

virtual int VDisAsm::update() {
  if (key == VK_G_CTRL) {
    String gotoLine = "";
    int a = VTxtAlrt::box(gotoLine,"Goto Address(Hex):","OK","Don't");
    if (a==1) {
      textPos = Point(0,0);
      int64_t address = gotoLine.fromHex();
      getDisassembly_6502(opcodes, lineNumbers, datas, data,address,data.length());
      text = opcodes;
      colors = text;
      updateSyntaxHiLight(text,colors,"");
    }
    key = VK_NONE;
  }

  return VText::update();
}


void VDisAsm::init(const String &_text, const String &_fileName) {
  data = _text;
  getDisassembly_6502(opcodes, lineNumbers, datas, data,0,data.length());
  text = opcodes;
  colors = text;
  updateSyntaxHiLight(text,colors,"");
}

typedef struct Opcode {
  char *name;
  int cycles;
  int type;
} Opcode;

const int OPC_TYPE_NONE = 0;
const int OPC_TYPE_ABS = 1;
const int OPC_TYPE_REL = 2;
const int OPC_TYPE_IND = 3;
const int OPC_TYPE_ZP = 4;
const int OPC_TYPE_ZPX = 5;
const int OPC_TYPE_ZPY = 6;
const int OPC_TYPE_IZX = 7;
const int OPC_TYPE_IZY = 8;
const int OPC_TYPE_ABX = 9;
const int OPC_TYPE_ABY = 10;
const int OPC_TYPE_IMM = 11;

int opcodeBytes[] = {1,3,2,3,2,2,2,2,2,3,3,2};

#define STAR(__v__) ((__v__)|32)
#define RED(__v__) ((__v__)|64)
#define BLUE(__v__) ((__v__)|128)

Opcode opcodes[] = {
  // 0x00
  {"brk",7,OPC_TYPE_NONE},
  {"ora",7,OPC_TYPE_IZX},
  {"kil",0,OPC_TYPE_NONE},
  {"slo",8,OPC_TYPE_IZX},
  {"nop",3,OPC_TYPE_ZP},
  {"ora",3,OPC_TYPE_ZP},
  {"asl",5,OPC_TYPE_ZP},
  {"slo",5,OPC_TYPE_ZP},
  {"php",3,OPC_TYPE_NONE},
  {"ora",2,OPC_TYPE_IMM},
  {"asl",2,OPC_TYPE_NONE},
  {"anc",2,OPC_TYPE_IMM},
  {"nop",4,OPC_TYPE_ABS},
  {"ora",4,OPC_TYPE_ABS},
  {"asl",6,OPC_TYPE_ABS},
  {"slo",6,OPC_TYPE_ABS},
  // 0x10
  {"bpl",2,OPC_TYPE_REL},
  {"ora",5,OPC_TYPE_IZY},
  {"kil",0,OPC_TYPE_NONE},
  {"slo",8,OPC_TYPE_IZY},
  {"nop",4,OPC_TYPE_ZPX},
  {"ora",4,OPC_TYPE_ZPX},
  {"asl",6,OPC_TYPE_ZPX},
  {"slo",6,OPC_TYPE_ZPX},
  {"clc",2,OPC_TYPE_NONE},
  {"ora",STAR(4),OPC_TYPE_ABY},
  {"nop",2,OPC_TYPE_NONE},
  {"slo",7,OPC_TYPE_ABY},
  {"nop",STAR(4),OPC_TYPE_ABX},
  {"ora",STAR(4),OPC_TYPE_ABX},
  {"asl",7,OPC_TYPE_ABX},
  {"slo",7,OPC_TYPE_ABX},
  // 0x20
  {"jsr",6,OPC_TYPE_ABS},
  {"and",6,OPC_TYPE_IZX},
  {"kil",0,OPC_TYPE_NONE},
  {"rla",8,OPC_TYPE_IZX},
  {"bit",3,OPC_TYPE_ZP},
  {"and",3,OPC_TYPE_ZP},
  {"rol",5,OPC_TYPE_ZP},
  {"rla",5,OPC_TYPE_ZP},
  {"plp",4,OPC_TYPE_NONE},
  {"and",2,OPC_TYPE_IMM},
  {"rol",2,OPC_TYPE_NONE},
  {"anc",2,OPC_TYPE_IMM},
  {"bit",3,OPC_TYPE_ABS},
  {"and",4,OPC_TYPE_ABS},
  {"rol",6,OPC_TYPE_ABS},
  {"rla",6,OPC_TYPE_ABS},
  // 0x30
  {"bmi",2,OPC_TYPE_REL},
  {"and",STAR(5),OPC_TYPE_IZY},
  {"kil",0,OPC_TYPE_NONE},
  {"rla",8,OPC_TYPE_IZY},
  {"nop",4,OPC_TYPE_ZPX},
  {"and",4,OPC_TYPE_ZPX},
  {"rol",6,OPC_TYPE_ZPX},
  {"rla",6,OPC_TYPE_ZPX},
  {"sec",2,OPC_TYPE_NONE},
  {"and",STAR(4),OPC_TYPE_ABY},
  {"nop",2,OPC_TYPE_NONE},
  {"rla",7,OPC_TYPE_ABY},
  {"nop",STAR(4),OPC_TYPE_ABX},
  {"and",STAR(4),OPC_TYPE_ABX},
  {"rol",7,OPC_TYPE_ABX},
  {"rla",7,OPC_TYPE_ABX},
  // 0x40
  {"rti",6,OPC_TYPE_NONE},
  {"eor",6,OPC_TYPE_IZX},
  {"kil",0,OPC_TYPE_NONE},
  {"sre",8,OPC_TYPE_IZX},
  {"nop",3,OPC_TYPE_ZP},
  {"eor",3,OPC_TYPE_ZP},
  {"lsr",5,OPC_TYPE_ZP},
  {"sre",5,OPC_TYPE_ZP},
  {"pha",3,OPC_TYPE_NONE},
  {"eor",2,OPC_TYPE_IMM},
  {"lsr",2,OPC_TYPE_NONE},
  {"alr",2,OPC_TYPE_IMM},
  {"jmp",3,OPC_TYPE_ABS},
  {"eor",4,OPC_TYPE_ABS},
  {"lsr",6,OPC_TYPE_ABS},
  {"sre",6,OPC_TYPE_ABS},
  // 0x50
  {"bvc",STAR(2),OPC_TYPE_REL},
  {"eor",STAR(5),OPC_TYPE_IZY},
  {"kil",0,OPC_TYPE_NONE},
  {"sre",8,OPC_TYPE_IZY},
  {"nop",4,OPC_TYPE_ZPX},
  {"eor",4,OPC_TYPE_ZPX},
  {"lsr",4,OPC_TYPE_ZPX},
  {"sre",4,OPC_TYPE_ZPX},
  {"cli",2,OPC_TYPE_NONE},
  {"eor",STAR(4),OPC_TYPE_ABY},
  {"nop",2,OPC_TYPE_NONE},
  {"sre",7,OPC_TYPE_ABY},
  {"nop",STAR(4),OPC_TYPE_ABX},
  {"eor",STAR(4),OPC_TYPE_ABX},
  {"lsr",7,OPC_TYPE_ABX},
  {"sre",7,OPC_TYPE_ABX},
  // 0x60
  {"rts",6,OPC_TYPE_NONE},
  {"adc",6,OPC_TYPE_IZX},
  {"kil",0,OPC_TYPE_NONE},
  {"rra",6,OPC_TYPE_IZX},
  {"nop",3,OPC_TYPE_ZP},
  {"adc",3,OPC_TYPE_ZP},
  {"ror",5,OPC_TYPE_ZP},
  {"rra",5,OPC_TYPE_ZP},
  {"pla",4,OPC_TYPE_NONE},
  {"adc",2,OPC_TYPE_IMM},
  {"ror",2,OPC_TYPE_NONE},
  {"arr",2,OPC_TYPE_IMM},
  {"jmp",5,OPC_TYPE_IND},
  {"adc",4,OPC_TYPE_ABS},
  {"ror",6,OPC_TYPE_ABX},
  {"rra",6,OPC_TYPE_ABX},
  // 0x70
  {"bvs",STAR(2),OPC_TYPE_REL},
  {"adc",STAR(5),OPC_TYPE_IZY},
  {"kil",0,OPC_TYPE_NONE},
  {"rra",8,OPC_TYPE_IZY},
  {"nop",4,OPC_TYPE_ZPX},
  {"adc",4,OPC_TYPE_ZPX},
  {"ror",6,OPC_TYPE_ZPX},
  {"rra",6,OPC_TYPE_ZPX},
  {"sei",6,OPC_TYPE_NONE},
  {"adc",STAR(4),OPC_TYPE_ABY},
  {"nop",2,OPC_TYPE_NONE},
  {"rra",7,OPC_TYPE_ABY},
  {"nop",STAR(4),OPC_TYPE_ABX},
  {"adc",STAR(4),OPC_TYPE_ABX},
  {"ror",7,OPC_TYPE_ABX},
  {"rra",7,OPC_TYPE_ABX},
  // 0x80
  {"nop",2,OPC_TYPE_IMM},
  {"sta",6,OPC_TYPE_IZX},
  {"nop",2,OPC_TYPE_IMM},
  {"sax",6,OPC_TYPE_IZX},
  {"sty",3,OPC_TYPE_ZP},
  {"sta",3,OPC_TYPE_ZP},
  {"stx",3,OPC_TYPE_ZP},
  {"sax",3,OPC_TYPE_ZP},
  {"dey",2,OPC_TYPE_NONE},
  {"nop",2,OPC_TYPE_IMM},
  {"txa",2,OPC_TYPE_NONE},
  {"xaa",RED(2),OPC_TYPE_NONE},
  {"sty",4,OPC_TYPE_ABS},
  {"sta",4,OPC_TYPE_ABS},
  {"stx",4,OPC_TYPE_ABS},
  {"sax",4,OPC_TYPE_ABS},
  // 0x90
  {"bcc",STAR(2),OPC_TYPE_REL},
  {"sta",6,OPC_TYPE_IZX},
  {"kil",0,OPC_TYPE_NONE},
  {"ahx",BLUE(6),OPC_TYPE_IZY},
  {"sty",4,OPC_TYPE_ZPX},
  {"sta",4,OPC_TYPE_ZPX},
  {"stx",4,OPC_TYPE_ZPY},
  {"sax",4,OPC_TYPE_ZPY},
  {"tya",2,OPC_TYPE_NONE},
  {"sta",5,OPC_TYPE_ABY},
  {"txs",2,OPC_TYPE_NONE},
  {"tas",BLUE(5),OPC_TYPE_ABY},
  {"shy",BLUE(5),OPC_TYPE_ABY},
  {"sta",5,OPC_TYPE_ABX},
  {"shx",BLUE(5),OPC_TYPE_ABY},
  {"ahx",BLUE(5),OPC_TYPE_ABY},
  // 0xa0
  {"ldy",2,OPC_TYPE_IMM},
  {"lda",6,OPC_TYPE_IZX},
  {"ldx",2,OPC_TYPE_IMM},
  {"lax",6,OPC_TYPE_IZX},
  {"ldy",3,OPC_TYPE_ZP},
  {"lda",3,OPC_TYPE_ZP},
  {"ldx",3,OPC_TYPE_ZP},
  {"lax",3,OPC_TYPE_ZP},
  {"tay",2,OPC_TYPE_NONE},
  {"lda",3,OPC_TYPE_IMM},
  {"tax",2,OPC_TYPE_NONE},
  {"lax",RED(2),OPC_TYPE_IMM},
  {"ldy",4,OPC_TYPE_ABS},
  {"lda",4,OPC_TYPE_ABS},
  {"ldx",4,OPC_TYPE_ABS},
  {"lax",4,OPC_TYPE_ABS},
  // 0xb0
  {"bcs",STAR(2),OPC_TYPE_REL},
  {"lda",STAR(5),OPC_TYPE_IZY},
  {"kil",0,OPC_TYPE_NONE},
  {"lax",STAR(5),OPC_TYPE_IZY},
  {"ldy",4,OPC_TYPE_ZPX},
  {"lda",4,OPC_TYPE_ZPX},
  {"ldx",4,OPC_TYPE_ZPX},
  {"lax",4,OPC_TYPE_ZPX},
  {"clv",2,OPC_TYPE_NONE},
  {"lda",STAR(4),OPC_TYPE_ABY},
  {"tsx",2,OPC_TYPE_NONE},
  {"las",STAR(4),OPC_TYPE_ABY},
  {"ldy",STAR(4),OPC_TYPE_ABX},
  {"lda",STAR(4),OPC_TYPE_ABX},
  {"ldx",STAR(4),OPC_TYPE_ABX},
  {"lax",STAR(4),OPC_TYPE_ABS},
  // 0xc0
  {"cpy",2,OPC_TYPE_IMM},
  {"cmp",6,OPC_TYPE_IZX},
  {"nop",2,OPC_TYPE_IMM},
  {"dcp",8,OPC_TYPE_IZX},
  {"cpy",3,OPC_TYPE_ZP},
  {"cmp",3,OPC_TYPE_ZP},
  {"dec",5,OPC_TYPE_ZP},
  {"dcp",5,OPC_TYPE_ZP},
  {"iny",2,OPC_TYPE_NONE},
  {"cmp",2,OPC_TYPE_IMM},
  {"dex",2,OPC_TYPE_NONE},
  {"axs",2,OPC_TYPE_IMM},
  {"cpy",4,OPC_TYPE_ABS},
  {"cmp",4,OPC_TYPE_ABS},
  {"dec",6,OPC_TYPE_ABS},
  {"dcp",6,OPC_TYPE_ABS},
  // 0xd0
  {"bne",STAR(2),OPC_TYPE_REL},
  {"cmp",STAR(5),OPC_TYPE_IZY},
  {"kil",0,OPC_TYPE_NONE},
  {"dcp",8,OPC_TYPE_IZY},
  {"nop",4,OPC_TYPE_ZPX},
  {"cmp",4,OPC_TYPE_ZPX},
  {"dec",6,OPC_TYPE_ZPX},
  {"dcp",6,OPC_TYPE_ZPX},
  {"cld",2,OPC_TYPE_NONE},
  {"cmp",STAR(4),OPC_TYPE_ABY},
  {"nop",2,OPC_TYPE_NONE},
  {"dcp",7,OPC_TYPE_ABY},
  {"nop",STAR(4),OPC_TYPE_ABX},
  {"cmp",STAR(4),OPC_TYPE_ABX},
  {"dec",7,OPC_TYPE_ABX},
  {"dcp",7,OPC_TYPE_ABX},
  // 0xe0
  {"cpx",2,OPC_TYPE_IMM},
  {"sbc",6,OPC_TYPE_IZX},
  {"nop",2,OPC_TYPE_IMM},
  {"isc",8,OPC_TYPE_IZX},
  {"cpx",3,OPC_TYPE_ZP},
  {"sbc",3,OPC_TYPE_ZP},
  {"inc",5,OPC_TYPE_ZP},
  {"isc",5,OPC_TYPE_ZP},
  {"inx",2,OPC_TYPE_NONE},
  {"sbc",2,OPC_TYPE_IMM},
  {"nop",2,OPC_TYPE_NONE},
  {"sbc",2,OPC_TYPE_IMM},
  {"cpx",4,OPC_TYPE_ABS},
  {"sbc",4,OPC_TYPE_ABS},
  {"inc",6,OPC_TYPE_ABS},
  {"isc",6,OPC_TYPE_ABS},
  // 0xf0
  {"beq",STAR(2),OPC_TYPE_REL},
  {"sbc",STAR(5),OPC_TYPE_IZY},
  {"kil",0,OPC_TYPE_NONE},
  {"isc",8,OPC_TYPE_IZY},
  {"nop",4,OPC_TYPE_ZPX},
  {"sbc",4,OPC_TYPE_ZPX},
  {"inc",6,OPC_TYPE_ZPX},
  {"isc",6,OPC_TYPE_ZPX},
  {"sed",2,OPC_TYPE_NONE},
  {"sbc",STAR(6),OPC_TYPE_ABY},
  {"nop",2,OPC_TYPE_NONE},
  {"isc",7,OPC_TYPE_ABY},
  {"nop",STAR(4),OPC_TYPE_ABX},
  {"sbc",STAR(4),OPC_TYPE_ABX},
  {"inc",7,OPC_TYPE_ABX},
  {"isc",7,OPC_TYPE_ABX}
};

String getInstructionLine_6502(int64_t &address, const String &text) {
  const Opcode &opc = opcodes[text[address]];
  if (text.length()<address + opcodeBytes[opc.type]) {
    address++;
    return "";
  }
  String instruction = String(opc.name) + " ";
  switch(opc.type) {
    case OPC_TYPE_NONE: break;
    case OPC_TYPE_ABS: instruction += "$" + String::hex(text[address+1]+text[address+2]*256,4); break;
    case OPC_TYPE_REL: instruction += "$" + String::hex(address+text[address+1]-128,4); break;
    case OPC_TYPE_IND: instruction += "($" + String::hex(text[address+1]+text[address+2]*256,4)+")"; break;
    case OPC_TYPE_ZP: instruction += "$" + String::hex(text[address+1],2); break;
    case OPC_TYPE_ZPX: instruction += "$" + String::hex(text[address+1]+text[address+2]*256,2)+",x"; break;
    case OPC_TYPE_ZPY: instruction += "$" + String::hex(text[address+1]+text[address+2]*256,2)+",y"; break;
    case OPC_TYPE_IZX: instruction += "($" + String::hex(text[address+1]+text[address+2]*256,2)+",x)"; break;
    case OPC_TYPE_IZY: instruction += "($" + String::hex(text[address+1]+text[address+2]*256,2)+"),y"; break;
    case OPC_TYPE_ABX: instruction += "$" + String::hex(text[address+1]+text[address+2]*256,4)+",x"; break;
    case OPC_TYPE_ABY: instruction += "$" + String::hex(text[address+1]+text[address+2]*256,4)+",y"; break;
    case OPC_TYPE_IMM: instruction += "#$" + String::hex(text[address+1],2); break;
  }
  address += opcodeBytes[opc.type];
  return instruction;
}

void getDisassembly_6502(String &opcs, String &adrs, String &dats, const String &text, int64_t startAddress, int64_t endAddress) {
  opcs.clear();
  adrs.clear();
  dats.clear();
  for (int64_t i = startAddress; i < endAddress;) {
    int iBefore = i;
    opcs += getInstructionLine_6502(i,text) + "\n";
    adrs += "$"+String::hex(iBefore,8)+"\n";
    for (int l = iBefore; l < i; l++) {if (l!=iBefore) dats += " "; dats += "$" + String::hex(text[l],2);}
    dats += "\n";
  }
}

