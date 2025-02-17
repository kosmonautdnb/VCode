#ifndef __VTEXT_HPP__
#define __VTEXT_HPP__

#include "vbase.hpp"
#include "string.hpp"
#include "constants.hpp"
#include "input.hpp"
#include "blit.hpp"
#include "vga.hpp"

enum {
        HILIGHT_NONE = 0,
        HILIGHT_SPECIAL = 1,
        HILIGHT_MATH = 2,
        HILIGHT_NUMBER = 3,
        HILIGHT_BOOLEAN = 4,
        HILIGHT_SCOPE = 5,
        HILIGHT_STRING = 6,
};

class VText : public VBase {
public:
  String text;
  String colors;
  String message;
  Point textPos;
  int mode;
  enum {
        MODE_TEXT = 1,
        MODE_HEX = 2,
  };
  virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
  virtual void draw();
  virtual void update();

  virtual void doModal() {
    pushRect(0,0,frameBufferWidth,frameBufferHeight);
    clipRect = Rect(0,0,frameBufferWidth,frameBufferHeight);
    do {
      updateKeyboard();
    } while(key != VK_NONE);
    do {
      drawCapturedFrameBuffer();
      updateKeyboard();
      updateMouse();
      draw();
      update();
      drawMouse();
      frameBufferToScreen(frameBuffer);
    } while(key != VK_ESCAPE && key != VK_F3);
    key = VK_NONE;
    popRect();
  }

  void syntaxHiLight(const String &text, String &colors) {
        for (int i = 0; i < text.length(); i++) {
                char c = text[i];
                switch(c) {
                case '\"': {
                        colors[i] = HILIGHT_STRING;
                        i++;
                        for (; i < text.length(); i++) {
                                colors[i]=HILIGHT_STRING;
                                if (text[i]=='\"'||text[i]=='\n') break;
                        }
                } break;
                case '\'': {
                        colors[i] = HILIGHT_STRING;
                        i++;
                        for (; i < text.length(); i++) {
                                colors[i]=HILIGHT_STRING;
                                if (text[i]=='\''||text[i]=='\n') break;
                        }
                } break;
                case 0:
                case '@':
                case '#':
                case '$':
                case '?':
                case '+': colors[i] = HILIGHT_SPECIAL; break;
                case '-':
                case '>':
                case '<':
                case '*':
                case '/':
                case '=': colors[i] = HILIGHT_MATH; break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': colors[i] = HILIGHT_NUMBER; break;
                case '&':
                case '|':
                case '^':
                case '~':
                case '!': colors[i] = HILIGHT_BOOLEAN; break;
                case '(':
                case ')':
                case '[':
                case ']':
                case '{':
                case '}':
                case '.':
                case ',':
                case ';':
                case ':': colors[i] = HILIGHT_SCOPE; break;
                default: colors[i] = HILIGHT_NONE; break;
                }
        }
  }

  void init(const String &_text) {
    text = _text;
    colors = _text;
    mode = MODE_TEXT;
    syntaxHiLight(text,colors);
    textPos = Point(0,0);
  }

  void setMessage(const String &_message) {
        message = _message;
  }
};

#endif //__VTEXT_HPP__