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
#include "input.hpp"

String currentWorkingDirectory;

class VTree : public VStrLst {
public:
  IFiles *directory;
  VPreview *preview;
  String searchText;
  virtual setRect(int x0, int y0, int x1, int y1) {VStrLst::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VStrLst::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VStrLst::setRect(p0,p1);}

  void searchTextUpdated();

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
    preview = new VPreview();
    int previewSize = 50*frameBufferWidth/320;
    int xp = 100*frameBufferWidth/320;
    int yp = 140*frameBufferHeight/200;
    preview->setRect(position.x+xp,position.y+yp,position.x+xp+previewSize,position.y+yp+previewSize);
  }

};

void VTree::searchTextUpdated() {
  String c = toLower(searchText);
  int y = 0;
  int found = -1;
  int ll = -1;
  do {
    String a = directory->getString(0,y);
    String b = directory->getString(1,y);
    if (a==NIL||b==NIL)
      break;
    a = toLower(a+b);
    int l = 0;
    for (int i = 0; i < c.length(); i++) {
      if (i < a.length()) {
        int o = a[i]-c[i];
        l += (o==0)?1:0;
      } else {
        break;
      }
    }
    if (l > ll && l != 0) {found = y; ll = l;}
    y++;
  } while(1==1);
  if (found != -1) selectedLine=found;
}

void VTree::draw() {
  VStrLst::draw();
  if (hasKeyboardFocus(this)) {
    if ((key>='a'&&key<='z')||(key>='0'&&key<='9')||(key>='A'&&key<='Z')||key=='.'||key=='!'||key=='_'||key=='/'||key=='\\') {
      String k=" ";k[0]=key;
      searchText+=k;
      searchTextUpdated();
    } else {
      if (key!=VK_NONE) searchText="";
    }
    pushRect(0,position.y+size.height,frameBufferWidth,frameBufferHeight);
    String siz = directory->getString(2,selectedLine);
    String fileName = directory->getString(4,selectedLine);
    if (siz != "<DIR>")
      preview->setFileName(fileName, key == VK_F3);
    drawString(0,position.y+size.height+1,searchText+">"+fileName,COLOR_PROMPTTEXT);
    int selecteds = 0;
    int selectedBytes = 0;
    for (int i = 0; i < directory->selection.size(); i++) {
      if (directory->selection[i]) {
        selecteds++;
        int y = i;
        if (y<directory->sorting.size()) y = directory->sorting[y];
        selectedBytes += directory->files[y].size;
      }
    }
    String status = "";
    if (selecteds>0) status += "Selected:"+String(selecteds) + (selecteds == 1 ? " File " : " Files ") + sizeString(selectedBytes);
    if (status == "") status = "<F3> View <F4> Edit";
    drawBox(0,position.y+size.height+1+CHAR_HEIGHT,frameBufferWidth,position.y+size.height+1+CHAR_HEIGHT*2,COLOR_STATUSBAR);
    drawString(0,position.y+size.height+1+CHAR_HEIGHT,status,COLOR_STATUSBARTEXT);
    popRect();
  }
  preview->draw();
}

void VTree::update() {
  VStrLst::update();
  if (hasKeyboardFocus(this)) {
    if (key==VK_ENTER) {
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
    treeLeft->init(Rect(0,CHAR_HEIGHT,frameBufferWidth/2-1,frameBufferHeight-16),"c:/");
    treeRight->init(Rect(frameBufferWidth/2,CHAR_HEIGHT,frameBufferWidth-1,frameBufferHeight-16),currentWorkingDirectory);

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
      drawMouse();
      frameBufferToScreen(frameBuffer);
      while(mouseB==1) updateMouse();
    } while(key!=VK_ESCAPE);
}

void graphicsMode320x200x256() {
    setBiosGraphicsMode(0x13);
    allocFrameBuffer(320,200);
    randomPalette();
}

void graphicsMode640x480x16() {
    setBiosGraphicsMode(0x12);
    allocFrameBuffer(640,480);
    palette16Colors();
}

void main()
{
    currentWorkingDirectory = getCWD();
    disableControlBreak();
    int gfxMode = getBiosGraphicsMode();
    graphicsMode320x200x256();
    //graphicsMode640x480x16();

    displayFileSelector();

    freeFrameBuffer();
    setBiosGraphicsMode(gfxMode);
}