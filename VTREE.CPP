#include "vtree.hpp"
#include "input.hpp"
#include "constant.hpp"
#include "blit.hpp"
#include "ifiles.hpp"
#include "vpreview.hpp"

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
    preview->init(fileName);
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
    if (status == "") status = "<F1>Hexed <F3>View <F4>Edit <F10>Exit";
    drawBox(0,position.y+size.height+1+CHAR_HEIGHT,frameBufferWidth,position.y+size.height+1+CHAR_HEIGHT*2,COLOR_STATUSBAR);
    drawString(0,position.y+size.height+1+CHAR_HEIGHT,status,COLOR_STATUSBARTEXT);
    popRect();
  }
  preview->draw();
}

void VTree::update() {
  VStrLst::update();
  int sortedBy = directory->sortedBy;
  bool sortedAscending = directory->sortedAscending;
  if (reload) {
    directory->files = getFiles(lastPathName);
    directory->sort(-1,false);
    directory->sort(sortedBy,sortedAscending);
    reload = false;
  }
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
        lastPathName=pathName;
        setList(directory,4);
        directory->sort(-1,false);
        directory->sort(sortedBy,sortedAscending);
      }
    }
    String fileName = directory->getString(4,selectedLine);
    if (key == VK_F1) {
      vHex->init(stringFromFile(fileName),fileName);
      vHex->doModal();
    }
    if (key == VK_F3) {
      vText->init(stringFromFile(fileName),fileName);
      vText->doModal();
    }
    if (key == VK_F4) {
      vCode->init(stringFromFile(fileName),fileName);
      vCode->doModal();
    }
  }
}

void VTree::setList(IStrLst *list, int listWidth) {
  VStrLst::setList(list,listWidth);
}

void VTree::init(const Rect &r,const String &path) {
  lastPathName = path;
  reload = false;
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
  vText = new VText();
  vText->setRect(0,0,frameBufferWidth,frameBufferHeight);
  vCode = new VCode();
  vCode->setRect(0,0,frameBufferWidth,frameBufferHeight);
  vHex = new VHex();
  vHex->setRect(0,0,frameBufferWidth,frameBufferHeight);
}

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