#include "vtxtline.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "input.hpp"
#include "vga.hpp" 

extern VBase *keyboardFocus;

void VTxtLine::draw() {
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
  if (hasKeyboardFocus(this) && (frameCounter & 8)) {
    drawBox(cursorPos.x,cursorPos.y,cursorPos.x+1,cursorPos.y+CHAR_HEIGHT,COLOR_CURSOR);
    drawBox(cursorPos.x+1,cursorPos.y,cursorPos.x+2,cursorPos.y+CHAR_HEIGHT,COLOR_RED);
  }
  popRect();
}

int VTxtLine::update() {
  if (mouseIn() && mouseB==1) setKeyboardFocus(this);
  if (keyboardFocus!=this) return 0;
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
  if (key == VK_LEFT_CTRL) {
    if (!text.empty()) {
      int x = cursorX-1;
      if (x < 0) x = 0;
      if (x >text.length()-1) x = text.length()-1;
      int a = text[x];
      bool full = (a>='0'&&a<='9')||(a>='a'&&a<='z')||(a>='A'&&a<='Z')||a=='_'||a=='.';
      int i;
      for (i = x;i>=0;i--) {
        a = text[i];
        bool full2 = (a>='0'&&a<='9')||(a>='a'&&a<='z')||(a>='A'&&a<='Z')||a=='_'||a=='.';
        if (full2!=full) {cursorX=i+1;break;}
      }
      if (i == -1) cursorX=0;
    }
  }
  if (key == VK_RIGHT_CTRL) {
    int a = cursorX+1<text.length()?text[cursorX+1]:' ';
    bool full = (a>='0'&&a<='9')||(a>='a'&&a<='z')||(a>='A'&&a<='Z')||a=='_'||a=='.';
    int i;
    for (i = cursorX+1;i<text.length();i++) {
      a = text[i];
      bool full2 = (a>='0'&&a<='9')||(a>='a'&&a<='z')||(a>='A'&&a<='Z')||a=='_'||a=='.';
      if (full2!=full) {cursorX=i;break;}
    }
    if (i == text.length()) cursorX=text.length();
  }
  if (key == VK_POS1) {
    cursorX = 0;
  }
  if (key == VK_END) {
    cursorX = text.length();
  }
  if (cursorX>text.length()) cursorX = text.length();
  if (cursorX<0) cursorX = 0;
  return 0;
}

void VTxtLine::init(const String &_text) {
  text = _text;
  restoreKeyboardFocus = keyboardFocus;
  setKeyboardFocus(this);
  cursorX = _text.length();
  scrollX = 0;
}