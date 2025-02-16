#include "types.hpp"
#include "vga.hpp"
#include "blit.hpp"
#include "pngload.hpp"
#include "array.hpp"
#include "string.hpp"
#include "dos.hpp"
#include "vstrlst.hpp"
#include "vpreview.hpp"
#include "ifiles.hpp"
#include "constant.hpp"

const char *mouseShape=
  "00000."
  "011110"
  "01110."
  "0110.."
  "010..."
  ".0....";

void drawMouse() {
  for (int y = 0; y < 6; y++) {
    for (int x = 0; x < 6; x++) {
      char c = mouseShape[x+y*6];
      if (c == '0') setPixel(mouseX+x,mouseY+y,0xff000000);
      if (c == '1') setPixel(mouseX+x,mouseY+y,0xffffffff);
    }
  }
  drawString(mouseX+7,mouseY,String(key & 255)+":"+String(key>>8),COLOR_WHITE);
}

class VTree : public VStrLst {
public:
        IFiles *directory;
        VPreview *preview;
        virtual setRect(int x0, int y0, int x1, int y1) {VStrLst::setRect(x0,y0,x1,y1);}
        virtual setRect(const Point &p0, const Size &size) {VStrLst::setRect(p0,size);}
        virtual setRect(const Point &p0, const Point &p1) {VStrLst::setRect(p0,p1);}
        virtual void draw();
        virtual void update();
        virtual void setList(IStrLst *list, int listWidth);

        void init(const Rect &r,const String &path) {
          directory = new IFiles();
          directory->files = getFiles(path);
          setRect(r.x0, r.y0, r.x1, r.y1);
          setList(directory,4);
          directory->sort(0,true);
          setKeyboardFocus(this);
        }
};

void VTree::draw() {
  VStrLst::draw();
  if (hasKeyboardFocus(this)) {
    pushRect(0,position.y+size.height,320,200);
    String fileName = directory->getString(4,selectedLine);
    preview->setFileName(fileName, key == 13);
    drawString(0,position.y+size.height+1,fileName,COLOR_PROMPTTEXT);
    popRect();
  }
}

void VTree::update() {
  VStrLst::update();
  if (hasKeyboardFocus(this)) {
    if (key==13) {
      int selected = directory->sorting[selectedLine];
      if (directory->files[selected].directory) {
        String pathName = directory->files[selected].name.path+"/";
        int oneDot = pathName.findLast("/./");
        int twoDots = pathName.findLast("/../");
        if (oneDot != -1) pathName = pathName.substr(0,oneDot)+pathName.substr(oneDot+3)+"/";
        if (twoDots != -1) {
          int i = pathName.findLast(twoDots,"/");
          if (i != -1) pathName = pathName.substr(0,i)+pathName.substr(twoDots+4)+"/";
        }
        directory->files = getFiles(pathName);
        setList(directory,4);
        directory->sort(0,true);
      }
    }
  }
}

void VTree::setList(IStrLst *list, int listWidth) {
  VStrLst::setList(list,listWidth);
}


void displayFileSelector() {
    captureFrameBuffer();
    VTree *treeLeft = new VTree();
    VTree *treeRight = new VTree();
    treeLeft->init(Rect(0,CHAR_HEIGHT,159,192),"c:/");
    treeRight->init(Rect(160,CHAR_HEIGHT,319,192),"c:/");
    VPreview *preview = new VPreview();
    int previewSize = 50;
    preview->setRect(100,140,100+previewSize,140+previewSize);
    treeLeft->preview = preview;
    treeRight->preview = preview;

    do {
      updateMouse();
      updateKeyboard();
      drawCapturedFrameBuffer();
      if (key == 0x09) {
        if (hasKeyboardFocus(treeLeft))
          setKeyboardFocus(treeRight);
        else
          setKeyboardFocus(treeLeft);
      }
      treeLeft->draw();
      treeRight->draw();
      preview->draw();
      drawMouse();
      frameBufferToScreen(frameBuffer);
      while(mouseB==1) updateMouse();
    } while(key!=27);
}

void main()
{
    setBiosGraphicsMode(0x13);
    allocFrameBuffer(320,200);
    randomPalette();

    displayFileSelector();

    freeFrameBuffer();
    setBiosGraphicsMode(2);

}