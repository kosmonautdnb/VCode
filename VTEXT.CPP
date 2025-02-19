#include "vtext.hpp"
#include "blit.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "dos.hpp"
#include "syntax.hpp"

void VText::draw() {
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,COLOR_TEXTBACKGROUND);

  int i;

  int textLastLine = 0;
  for (i = 0; i < text.length(); i++) if (text[i]=='\n') textLastLine++;
  if (textLastLine>0) {
    scrollBoxY = textPos.y*size.height/(textLastLine*CHAR_HEIGHT);
    drawBox(position.x+size.width-4*frameBufferWidth/320,position.y+scrollBoxY,position.x+size.width,scrollBoxY+2*frameBufferHeight/200,COLOR_SCROLL);
  }

  Point pos = position - textPos;
  int line = 0;
  while(pos.y < position.y+size.height) {
    String lineNr = String(line);
    drawString(position.x+size.width-lineNr.pixelWidth(),pos.y,lineNr, COLOR_LINENUMBERS);
    pos.y += CHAR_HEIGHT;
    line++;
  }
  pos = position - textPos;
  for (i = 0; i < text.length(); i++) {
    char c = text[i];
    switch(c) {
      case '\n': {pos.y += CHAR_HEIGHT; pos.x = position.x - textPos.x;continue;} break;
      case '\t': {pos.x = ((pos.x - (position.x - textPos.x)) / (CHAR_HEIGHT*2)+1)*(CHAR_HEIGHT*2)+(position.x-textPos.x);continue;} break;
    }
    drawChar(pos.x,pos.y,c,getHiLightColor(colors[i]));
    if (pos.y > position.y + size.height) break;
  }
  if (i == text.length())
    drawStringOutlined(pos.x,pos.y,"<EOF>",COLOR_RED,COLOR_YELLOW);
  String status = String(textPos.x/CHAR_HEIGHT)+" "+String(textPos.y/CHAR_HEIGHT);
  status += " "+sizeString(text.length());
  status += " " + fileName;
  drawBox(position.x,position.y+size.height-CHAR_HEIGHT,position.x+size.width,position.y+size.height,COLOR_STATUSBAR);
  drawString(position.x,position.y+size.height-CHAR_HEIGHT,status,COLOR_STATUSBARTEXT);
  popRect();
}

int VText::update() {
  if (mouseB==1&&mouseX>position.x+size.width-4*frameBufferWidth/320) {
    int textLastLine = 0;
    for (int i = 0; i < text.length(); i++) if (text[i]=='\n') textLastLine++;
    textPos.y = (mouseY-position.y)*textLastLine*CHAR_HEIGHT/size.height;
  }

  if (key == VK_UP) {textPos.y -= CHAR_HEIGHT; if (textPos.y < 0) textPos.y = 0;}
  if (key == VK_DOWN) {textPos.y += CHAR_HEIGHT;}
  if (key == VK_PAGEUP) {textPos.y -= CHAR_HEIGHT*16; if (textPos.y < 0) textPos.y = 0;}
  if (key == VK_PAGEDOWN) {textPos.y += CHAR_HEIGHT*16;}
  if (key == VK_LEFT) {textPos.x -= CHAR_HEIGHT; if (textPos.x < 0) textPos.x = 0;}
  if (key == VK_RIGHT) {textPos.x += CHAR_HEIGHT;}
  return 0;
}

void VText::init(const String &_text, const String &_fileName) {
  text = _text;
  colors = _text;
  fileName = _fileName;
  updateSyntaxHiLight(text,colors);
}