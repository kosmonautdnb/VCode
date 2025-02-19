#include "vtxtline.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "input.hpp"

void VTxtLine::draw() {
  static int time=0; time++;
  pushRect(position.x,position.y,position.x+size.width,position.y+size.height);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height, COLOR_HEADERLINE);
  Point pos;
  pos.x = position.x-scrollX+1;
  pos.y = position.y+1;
  Point cursorPos;
  cursorPos.x = 0;
  cursorPos.y = 0;
  for (int i = 0; i < text.length()+1; i++) {
    char c = i<text.length()?text[i]:' ';
    if (i==cursorX) cursorPos = pos;
    drawChar(pos.x,pos.y,c,COLOR_HEADERTEXT);
  }
  if (cursorPos.x>position.x+size.width-2) scrollX += cursorPos.x-(position.x+size.width-2);
  if (cursorPos.x<position.x+2) scrollX -= (position.x+2)-cursorPos.x;
  if (scrollX<0) scrollX = 0;
  if (time & 8) {
    drawBox(cursorPos.x,cursorPos.y,cursorPos.x+1,cursorPos.y+CHAR_HEIGHT,COLOR_CURSOR);
    drawBox(cursorPos.x+1,cursorPos.y,cursorPos.x+2,cursorPos.y+CHAR_HEIGHT,COLOR_RED);
  }
  popRect();
}

int VTxtLine::update() {
  bool valid = (key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9');
  switch(key) {
    case '!':
    case '"':
    case '$':
    case '%':
    case '&':
    case '/':
    case '(':
    case ')':
    case '=':
    case '?':
    case '`':
    case 'ø':
    case '^':
    case '{':
    case '}':
    case '[':
    case ']':
    case '\\':
    case 'á':
    case '\'':
    case '*':
    case '+':
    case '~':
    case '#':
    case '-':
    case '_':
    case '.':
    case ':':
    case ';':
    case ',':
    case '<':
    case '>':
    case ' ':
    case '|': valid = true;break;
  }
  if (valid) {
    text.insert(cursorX,String::chr(key));
    cursorX++;
  }
  if (key == VK_DELETE) {
    if (cursorX>0&&(cursorX<=text.length())) {
      text.erase(cursorX-1,1);
      cursorX--;
    }
  }
  if (key == VK_ENTF) {
    if (cursorX < text.length())
      text.erase(cursorX,1);
  }
  if (key == VK_INSERT) {
    if (cursorX < text.length())
      text.insert(cursorX," ");
  }
  if (key == VK_LEFT) {
    cursorX--;
  }
  if (key == VK_RIGHT) {
    cursorX++;
  }
  if (cursorX>text.length()) cursorX = text.length();
  if (cursorX<0) cursorX = 0;
  return 0;
}

void VTxtLine::init(const String &_text) {
  text = _text;
  cursorX = 0;
  scrollX = 0;
}