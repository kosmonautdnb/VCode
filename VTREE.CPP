#include "vtree.hpp"
#include "input.hpp"
#include "constant.hpp"
#include "blit.hpp"
#include "ifiles.hpp"
#include "vpreview.hpp"
#include <direct.h>
#include "util.hpp"
#include "valert.hpp"
#include "vtxtalrt.hpp"

VTree *treeLeft = NULL;
VTree *treeRight = NULL;
extern bool requestSideSwitch;

void VTree::draw() {

  if (treeMode == TREEMODE_EDIT) {
    if (hasKeyboardFocus(this)) {
      pushRect(position.x,position.y-CHAR_HEIGHT,position.x+size.width,position.y+size.height);
      drawBox(position.x,position.y-CHAR_HEIGHT,position.x+size.width,position.y,COLOR_HEADERLINE);
      popRect();
    }
    String status = "ESC for exit. ALT+Shift instead of TAB.";
    drawBox(0,position.y+size.height+1+CHAR_HEIGHT,frameBufferWidth,position.y+size.height+1+CHAR_HEIGHT*2,COLOR_STATUSBAR);
    drawString(0,position.y+size.height+1+CHAR_HEIGHT,status,COLOR_STATUSBARTEXT);
    vCode->setRect(this->position,this->size);
    vCode->draw();
    return;
  }
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
    vPreview->init(fileName);
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
    if (status == "") {
      if (!ctrlKey)
        status = "F1Hx F2Renam F3Vw F4Edt F5Cpy F6Mve F7Nwdir F8Del F9Nfo";
      else
        status = "F4Edt(Other)";
      if (shiftKey)
        status = "F1Dis F2Pnt F3Cmb F4Edt(This)";
    }
    drawBox(0,position.y+size.height+1+CHAR_HEIGHT,frameBufferWidth,position.y+size.height+1+CHAR_HEIGHT*2,COLOR_STATUSBAR);
    drawString(0,position.y+size.height+1+CHAR_HEIGHT,status,COLOR_STATUSBARTEXT);
    popRect();
  }
  int xp = size.width-52*frameBufferWidth/320;
  vPreview->position.x = position.x+xp;
  vPreview->draw();
}

int VTree::update() {
  if (treeMode == TREEMODE_EDIT) {
    if (!hasKeyboardFocus(this)) return 0;
    int r = vCode->update();
    if (key == VK_ESCAPE) {
      treeMode = TREEMODE_FILETREE;
      key = VK_NONE;
    }
    return r;
  }
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
    String extension = toLower(getExtension(fileName));
    if (key == VK_F1) {
      vHex->init(stringFromFile(fileName,false),fileName);
      vHex->setRect(0,0,frameBufferWidth,frameBufferHeight);
      vHex->doModal();
    }
    if (key == VK_F1_SHIFT) {
      vDisAsm->init(stringFromFile(fileName,true),fileName);
      vDisAsm->setRect(0,0,frameBufferWidth,frameBufferHeight);
      vDisAsm->doModal();
    }
    if (key == VK_F2_SHIFT) {
      treeLeft->cleanUp();
      treeRight->cleanUp();
      vClippy->init(lastPathName,frameBufferWidth,frameBufferHeight);
      vClippy->setRect(0,0,frameBufferWidth,frameBufferHeight);
      vClippy->load(fileName,-1);
      vClippy->doModal();
      setKeyboardFocus(this);
    }
    if (key == VK_F3_SHIFT) {
      String combinedFileName;
      int a = VTxtAlrt::box(combinedFileName,"Combined filename:","Create","Don't");
      if (a == 1) {
        combineFiles(combinedFileName);
      }
    }
    if (key == VK_F3) {
      vText->init(stringFromFile(fileName,true),fileName);
      vText->setRect(0,0,frameBufferWidth,frameBufferHeight);
      vText->doModal();
    }
    if (key == VK_F4) {
      if (extension != ".exe" && extension != ".com"&& extension != ".sys") {
        vCode->init(stringFromFile(fileName,true),fileName);
        vCode->setRect(0,0,frameBufferWidth,frameBufferHeight);
        vCode->doModal();
      }
    }
    if (key == VK_F4_CTRL || key == VK_F4_SHIFT) {
      if (extension != ".exe" && extension != ".com"&& extension != ".sys") {
        VTree *toLoadOn = this;
        if (key == VK_F4_CTRL) {
          toLoadOn = this == treeLeft ? treeRight : treeLeft;
          requestSideSwitch = true;
        }
        if (toLoadOn != NULL) {
          toLoadOn->vCode->init(stringFromFile(fileName,true),fileName);
          toLoadOn->vCode->setRect(this->position,this->size);
          toLoadOn->vCode->reset();
          toLoadOn->treeMode = TREEMODE_EDIT;
        }
        return 0;
      }
    }
    if (key == VK_F2) {
      String fullFileName = directory->getString(4,selectedLine);
      String filePath = getPath(fullFileName);
      String fileName = getName(fullFileName)+getExtension(fullFileName);
      int a = VTxtAlrt::box(fileName,"Rename from:" + fileName.last(30),"Rename","Don't");
      if (a == 1) {
        renameFile(fullFileName, filePath+fileName);
      }
    }
    if (key == VK_F5||key == VK_F6) {
      Array<String> files;
      int i;
      for (i = 0; i < directory->selection.size(); i++) {
        if (directory->selected(i)&&directory->getString(2,i)!="<DIR>")
          files.push_back(directory->getString(4,i));
      }
      if (files.size()==0&&directory->getString(2,selectedLine)!="<DIR>")
        files.push_back(directory->getString(4,selectedLine));
      if (!files.empty()) {
        bool b = false;
        if (key == VK_F5)
          if (copyFiles(this==treeLeft?treeRight->lastPathName:treeLeft->lastPathName,files)) b = true;
        if (key == VK_F6)
          if (moveFiles(this==treeLeft?treeRight->lastPathName:treeLeft->lastPathName,files)) b = true;
        if (b)
          for (i = 0; i < directory->selection.size(); i++) directory->select(i,false);
      }
    }
    if (key == VK_F7) {
      String folderName="";
      int a = VTxtAlrt::box(folderName,"Create new Folder","Create","Don't");
      if (a == 1) {
        createFolder(lastPathName+folderName);
        fileWasWritten = true;
      }
    }
    if (key == VK_F8) {
      Array<String> files;
      int i;
      for (i = 0; i < directory->selection.size(); i++) {
        if (directory->selected(i)&&directory->getString(2,i)!="<DIR>")
          files.push_back(directory->getString(4,i));
      }
      if (files.size()==0&&directory->getString(2,selectedLine)!="<DIR>")
        files.push_back(directory->getString(4,selectedLine));
      bool b = false;
      if (files.size()==0&&directory->getString(2,selectedLine)=="<DIR>") {
        int a = VAlert::box("Delete this directory?","YES","NO");
        if (a == 1) {
          deleteFolder(directory->getString(4,selectedLine));
          b = true;    
        }
      }
      if (!files.empty()) {
        if (removeFiles(files)) b = true;
      }
      if (b)
        for (i = 0; i < directory->selection.size(); i++) directory->select(i,false);
    }
    if (key == VK_F9) {
      vCode->init(getInfoString(),"");
      vCode->setRect(0,0,frameBufferWidth,frameBufferHeight);
      vCode->doModal();
    }
    if (key == '+') {
      int a = VTxtAlrt::box(lastMask,"        Select        ","OK");
      if (a == 1) {
        updateSelection(lastMask,true);
      }
    }
    if (key == '-') {
      int a = VTxtAlrt::box(lastMask,"        Unselect        ","OK");
      if (a == 1) {
        updateSelection(lastMask,false);
      }
    }
  }
  return 0;
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
  int previewSize = 50*frameBufferWidth/320;
  int xp = size.width-52*frameBufferWidth/320;
  int yp = 140*frameBufferHeight/200;
  vPreview->setRect(position.x+xp,position.y+yp,position.x+xp+previewSize,position.y+yp+previewSize);
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

void VTree::overlayByFileName(const String &fileName) {
  vCode->init(stringFromFile(fileName,true),fileName);
  vCode->setRect(this->position,this->size);
  treeMode = TREEMODE_EDIT;
}

bool hasMask(const String &line, const String &mask) {
  if (mask.empty()||line.empty())
    return false;
  int j = 0;
  int i = 0;
  for (; i < line.length(); i++) {
    char c = line[i];
    if (j >= mask.length()) return false;
    if (mask[j]=='*') {
      if (j+1 < mask.length()) if (c == mask[j+1]) j+=2;
      continue;
    }
    if (mask[j]=='?') {j++;continue;}
    if (c != mask[j]) return false;
    j++;
  }
  if (j == mask.length()) return true;
  if (j == mask.length()-1 && mask[j]=='*') return true;
  return false;
}

void VTree::updateSelection(const String &mask, bool doSelect) {
  int y = 0;
  String search = toLower(mask);
  do {
    String a = directory->getString(0,y);
    String b = directory->getString(1,y);
    String c = directory->getString(2,y);
    if (a==NIL||b==NIL||c==NIL)
      break;
    if (c != "<DIR>") {
      a = toLower(a+b);
      if (hasMask(a,search)) {
        directory->select(y,doSelect);
      }
    }
    y++;
  } while(1==1);
}

void VTree::combineFiles(const String &fileName) {
  String result;
  String a;
  int y = 0;
  a=directory->getString(4,y);
  while(a!=NIL) {
    if (directory->selection[y])
      result += stringFromFile(a,false);
    y++;
    a=directory->getString(4,y);
  }
  stringToFile(result,fileName,false);
  vCode->init(stringFromFile(fileName,true),fileName);
  vCode->setRect(0,0,frameBufferWidth,frameBufferHeight);
  vCode->doModal();
}
