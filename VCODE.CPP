#include "vcode.hpp"
#include "blit.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "dos.hpp"
#include "clipbrd.hpp"
#include "util.hpp"
#include "valert.hpp"

void VCode::draw() {
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,COLOR_TEXTBACKGROUND);

  int i;
  int lineNr = 0;
  int x = 0,maxX = 0;
  for (i = 0; i < text.length(); i++) {
    char c = text[i];
    x++;
    if (c=='\n') {if (x>maxX) maxX=x;x=0;lineNr++;}
  }
  textLastLine=lineNr;
  if (maxX>0&&textLastLine>size.height/CHAR_HEIGHT&&(displayOutline>0)) {
    lineNr = 0;
    x = 0;
    int w = 20*frameBufferWidth/320;
    int h = size.height;
    for (i = 0; i < text.length(); i++) {
      char c = text[i];
      if (c>='0') {
        int x0 = position.x+size.width-w+x*w/maxX;
        int y0 = position.y+lineNr*h/textLastLine;
        int x1 = position.x+size.width-w+(x+1)*w/maxX;
        int y1 = position.y+(lineNr+1)*h/textLastLine;
        if (x1==x0)x1=x0+1;
        if (y1==y0)y1=y0+1;
        drawBox(x0,y0,x1,y1,getHiLightColor(colors[i]));
      }
      x++;
      if (c=='\n') {x=0;lineNr++;}
    }
    drawRectangle(position.x+size.width-w-1, position.y+textPos.y*h/(textLastLine*CHAR_HEIGHT)-1,position.x+size.width+1, position.y+(textPos.y+size.height)*h/(textLastLine*CHAR_HEIGHT)+1, COLOR_RECTANGLE);
    displayOutline--;
  }


  if (textLastLine>0) {
    scrollBoxY = textPos.y*size.height/(textLastLine*CHAR_HEIGHT);
    drawBox(position.x+size.width-4*frameBufferWidth/320,position.y+scrollBoxY,position.x+size.width,position.y+scrollBoxY+2*frameBufferHeight/200,COLOR_SCROLL);
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
  int lineX = 0;
  int lineY = 0;
  cursorI = -1;
  topLine = -1;
  bottomLine = -1;
  cursorIX=-1;
  Point eofPos;
  for (i = 0; i < text.length() || (pos.y < position.y + size.height); i++) {
    char c = i < text.length() ? text[i] : '\n';
    if (i == text.length()-1) {eofPos = pos; eofPos.x += String::chr(c).pixelWidth();}
    char l = i < text.length() ? colors[i] : HIGHLIGHT_EMPTY;
    bool selected = (selectionStart>=0&&selectionEnd>=0)&&((i>=selectionStart&&i<selectionEnd)||(i>=selectionEnd&&i<selectionStart));
    if (i == cursorRequiresI) {
      cursorX = lineX;
      cursorY = lineY;
      cursorRequiresI = -1;
    }
    if (lineY == cursorY) {
      if (cursorRequiresX!=-1 && pos.x>=cursorRequiresX) {cursorRequiresX=-1;cursorX = lineX;}
      if (lineX == cursorX) {
        cursorI = i;
        cursorPos = pos;
      }
    }
    if (pos.y < position.y + size.height) bottomLine = lineY;
    if (topLine == -1 && pos.y >= position.y) topLine = lineY;
    switch(c) {
      case '\n': {
        if(selected)
          drawBox(pos.x,pos.y,pos.x+size.width,pos.y+CHAR_HEIGHT,COLOR_SELECTION);
        if (mouseB==1 && mouseX>=pos.x&&mouseY>=pos.y&&mouseY<pos.y+CHAR_HEIGHT&&mouseX<=position.x+size.width-4*frameBufferWidth/320) {
          cursorY = lineY; cursorX=lineX+(mouseX-pos.x)/String::chr(' ').pixelWidth();
        }
        if(lineY==cursorY&&cursorI==-1)
        {
          cursorI=i;cursorPos=pos;cursorIX=lineX;
        }
        lineX=0;
        lineY++;
        pos.y += CHAR_HEIGHT;
        pos.x = position.x - textPos.x;continue;
      } break;
    }
    if (selected) {
      String k; k.resize(1); k[0]=c;
      drawBox(pos.x,pos.y,pos.x + k.pixelWidth(),pos.y+CHAR_HEIGHT,COLOR_SELECTION);
    }
    if (mouseX>=pos.x&&mouseY>=pos.y&&mouseY<pos.y+CHAR_HEIGHT&&mouseX<pos.x+String::chr(c).pixelWidth()) {
      if (mouseB==1) {
        cursorX = lineX; cursorY = lineY;
      }
    }
    drawChar(pos.x, pos.y, c, getHiLightColor(l));
    lineX++;
  }
  if (cursorIX != -1) cursorPos.x += (cursorX - cursorIX)*String(" ").pixelWidth();
  if (cursorRequiresX!=-1) {
    if (cursorPos.x<cursorRequiresX) {
      while(cursorPos.x<cursorRequiresX) {
        cursorPos.x += String(" ").pixelWidth();
        cursorX++;
      }
    } else {
      while(cursorPos.x>cursorRequiresX) {
        cursorPos.x -= String(" ").pixelWidth();
        cursorX--;
      }
    }
  }
  cursorRequiresX=-1;
  cursorRequiresI = -1;
  if (cursorPos.x > position.x + size.width) textPos.x += CHAR_HEIGHT;
  if (cursorPos.x < position.x) {textPos.x -= CHAR_HEIGHT; if (textPos.x < 0) textPos.x = 0;}
  if (selectionEnd==-2) {selectionEnd = cursorI;}
  drawStringOutlined(eofPos.x,eofPos.y,"<EOF>",COLOR_RED,COLOR_YELLOW);
  drawCursor();
  bool updated = originalText != text;
  String status = (updated ? ".." : "") + String("<CTRL-L/S> ") + String(cursorX)+" "+String(cursorY) + fileName.last(20) + " " + sizeString(text.length());
  drawBox(position.x,position.y+size.height-CHAR_HEIGHT,position.x+size.width,position.y+size.height,COLOR_STATUSBAR);
  drawString(position.x,position.y+size.height-CHAR_HEIGHT,status,COLOR_STATUSBARTEXT);
  popRect();
}

void VCode::expand() {
  if (cursorI>text.length()) {
    for (int i = cursorI-text.length()-1; i >= 0; i--) {
      text += "\n";
      colors += "\n";
    }
  }
  if (cursorIX != -1) {
    for (int i = cursorX-cursorIX-1; i >= 0; i--) {
      text.insert(cursorI,' ');
      colors.insert(cursorI,' ');
      cursorI++;
    }
  }
}

int VCode::update() {
  int lastY = textPos.y;
  if (cursorY < topLine) textPos.y -= (topLine-cursorY)*CHAR_HEIGHT;
  if (cursorY > bottomLine) textPos.y+=(cursorY-bottomLine)*CHAR_HEIGHT;
  if (mouseY>=position.y&&mouseY<position.y+size.height&&mouseX>position.x+size.width-4*frameBufferWidth/320&&mouseX<position.x+size.width) {
    displayOutline = true;
    if (mouseB == 1) {
      textPos.y = (mouseY-position.y)*textLastLine*CHAR_HEIGHT/size.height;
      cursorY=textPos.y/CHAR_HEIGHT+1;
    }
  }

  if (strgKey) {
    if (key==VK_L_CTRL && (!fileName.empty())) {
      cursorRequiresI=cursorI;
      text = stringFromFile(fileName,true);
      originalText = text;
      colors = text;
      updateSyntaxHiLight(text,colors);
      return 0;
    }
    if (key==VK_S_CTRL && (!fileName.empty())) {
      originalText = text;
      stringToFile(text,fileName,true);
      return 0;
    }
  }

  if (shiftKey) {
    if (key == VK_UP||key == VK_DOWN||key == VK_LEFT||key==VK_RIGHT||key==VK_PAGEUP||key==VK_PAGEDOWN) {
      if (shiftReleased) selectionStart = cursorI;
      selectionEnd = -2;
      shiftReleased = false;
    }
  } else {
    shiftReleased = true;
    if (key == VK_UP||key == VK_DOWN||key == VK_LEFT||key==VK_RIGHT||key==VK_PAGEUP||key==VK_PAGEDOWN) {
      selectionStart=-1;
      selectionEnd=-1;
    }
  }
  if (selectionStart>=0&&selectionEnd>=0) {
    if (key == VK_ENTF || key == VK_DELETE) {
      if (selectionStart>selectionEnd) {
        int t = selectionStart;
        selectionStart = selectionEnd;
        selectionEnd = t;
      }
      if (selectionEnd>text.length()) selectionEnd = text.length();
      if (selectionStart>text.length()) selectionStart = text.length();
      text.erase(selectionStart,selectionEnd-selectionStart);
      colors.erase(selectionStart,selectionEnd-selectionStart);
      updateSyntaxHiLight(text,colors);
      cursorRequiresI=selectionStart;
      selectionStart = -1;
      selectionEnd = -1;
      key = VK_NONE;
    }
    if (key == VK_C_CTRL && strgKey) {
      if (selectionStart>selectionEnd) {
        int t = selectionStart;
        selectionStart = selectionEnd;
        selectionEnd = t;
      }
      if (selectionEnd>text.length()) selectionEnd = text.length();
      if (selectionStart>text.length()) selectionStart = text.length();
      clipBoard_string=text.substr(selectionStart,selectionEnd);
      selectionStart = -1;
      selectionEnd = -1;
      key = VK_NONE;
    }
  }
  if (key == VK_V_CTRL && strgKey && cursorI>=0) {
    expand();
    text.insert(cursorI,clipBoard_string);
    colors.insert(cursorI,clipBoard_string);
    updateSyntaxHiLight(text,colors);
    key = VK_NONE;
  }

  if (key == VK_ESCAPE) {}
  else
  if (key == VK_POS1) {
    if (cursorI != -1) {
      if (cursorI>text.length()) {
        cursorRequiresI = cursorI;
      } else
        for (int i = cursorI-1; i >= -1; i--) {
          if (i==-1||text[i]=='\n') {cursorRequiresI = i+1;break;}
        }
    }
  } else
  if (key == VK_END) {
    if (cursorI != -1) {
      if (cursorI>=text.length()) {
        cursorRequiresI = cursorI;
      } else
      if (text[cursorI]=='\n') {
        cursorRequiresI = cursorI;
      } else
        for (int i = cursorI; i < text.length(); i++) {
          if (text[i]=='\n') break;
          cursorX++;
        }
    }
  } else
  if (key == VK_UP) {cursorRequiresX=cursorPos.x;cursorY--; if (cursorY < 0) cursorY = 0;}
  else
  if (key == VK_DOWN) {cursorRequiresX=cursorPos.x;cursorY++;}
  else
  if (key == VK_PAGEUP) {cursorY-=16;if(cursorY<0)cursorY=0; }
  else
  if (key == VK_PAGEDOWN) {cursorRequiresX=cursorPos.x;cursorY+=16; }
  else
  if (key == VK_LEFT || key == VK_LEFT_STRG) {int d = strgKey?4:1; cursorX -=d; if (cursorX<0) {cursorX=0;cursorRequiresI = cursorI-1;}}
  else
  if (key == VK_RIGHT || key == VK_RIGHT_STRG) {cursorX+=strgKey?4:1;}
  else {
    if (cursorI != -1) {
      if (key == VK_ENTF) {
          expand();
          text.erase(cursorI,1);
          colors.erase(cursorI,1);
          updateSyntaxHiLight(text,colors);
      }
      else
      if (key == VK_DELETE) {
        if (cursorI>0) {
          expand();
          text.erase(cursorI-1,1);
          colors.erase(cursorI-1,1);
          cursorRequiresI = cursorI-1;
          updateSyntaxHiLight(text,colors);
        }
      }
      else
      if (key == VK_TAB||key == VK_TAB_SHIFT) {
        expand();
        if (selectionStart>=0&&selectionEnd>=0) {
          int searchStart = selectionStart;
          int searchEnd = selectionEnd;
          if (searchStart>searchEnd) {
            int t = searchStart;
            searchStart=searchEnd;
            searchEnd=t;
          }
          searchStart--;
          for(int i = searchStart; i < searchEnd-1; i++) {
            for (int j = 0; j < 2; j++) {
              if ((i==-1)||(text[i]=='\n')) {
                if (!shiftKey) {
                  text.insert(i+1," ");
                  colors.insert(i+1," ");
                  searchEnd++;
                } else {
                  if (text[i+1]==' ') {
                    text.erase(i+1,1);
                    colors.erase(i+1,1);
                    searchEnd--;
                  }
                }
              }
            }
          }
          if (selectionStart>selectionEnd)
            selectionStart=searchEnd;
          else
            selectionEnd=searchEnd;
          updateSyntaxHiLight(text,colors);

        } else {
          for (int x = 4-(cursorX & 3); x >= 0; x--) {
            text.insert(cursorI,' ');
            colors.insert(cursorI,' ');
            cursorX++;
          }
          updateSyntaxHiLight(text,colors);
        }
      }
      else
      if (key == VK_INSERT) {
        overwriteMode = !overwriteMode;
        key = VK_NONE;
      }
      else
      if (key < 256 && key >= ' ') {
        if (key == VK_ENTER) {
          expand();
          text.insert(cursorI,'\n');
          colors.insert(cursorI,'\n');
          updateSyntaxHiLight(text,colors);
          cursorY++;
          cursorX=0;
        } else {
          expand();
          if (overwriteMode&&text[cursorI]!='\n') {
            text[cursorI] = key;
          } else {
            text.insert(cursorI,key);
            colors.insert(cursorI,key);
          }
          updateSyntaxHiLight(text,colors);
          cursorX++;
        }
      }
    }
  }
  if (lastY!=textPos.y) displayOutline = DISPLAYOUTLINE_DURATION;
  if (key == VK_ESCAPE) {
    if (originalText != text) {
      int a = VAlert::box("Do you want to save the file?","YES","NO");
      if (a==1) {
        stringToFile(text,fileName,true);
      }
      if (a != 0)
        key = VK_ESCAPE;
    }
  }
  return 0;
}

void VCode::init(const String &_text, const String &_fileName) {
  fileName = _fileName;
  text = _text;
  originalText = _text;
  colors = _text;
  updateSyntaxHiLight(text,colors);
  textPos = Point(0,0);
}

void VCode::drawCursor() {
  if (cursorI != -1) {
    static int i = 0;i++;
    if (i & 8) {
      if (!overwriteMode) {
        drawBox(cursorPos.x,cursorPos.y,cursorPos.x+1,cursorPos.y+CHAR_HEIGHT,COLOR_CURSOR);
        drawBox(cursorPos.x+1,cursorPos.y,cursorPos.x+2,cursorPos.y+CHAR_HEIGHT,COLOR_RED);
      } else {
        drawBox(cursorPos.x,cursorPos.y,cursorPos.x+CHAR_HEIGHT,cursorPos.y+CHAR_HEIGHT,COLOR_CURSOR);
      }
    }
  }
}