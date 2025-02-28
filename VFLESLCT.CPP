#include "vfleslct.hpp"
#include "input.hpp"
#include "constant.hpp"
#include "blit.hpp"
#include "ifiles.hpp"
#include "valert.hpp"
#include "vtxtalrt.hpp"

void VFleSlct::draw() {
  VStrLst::draw();
}

int VFleSlct::update() {
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
        int sortedBy = directory->sortedBy;
        int sortedAscending = directory->sortedAscending;
        directory->files = getFiles(pathName);
        lastPathName=pathName;
        setList(directory,4);
        directory->sort(-1,false);
        directory->sort(sortedBy,sortedAscending);
      } else {
        if (modeLoading) {
          String file = directory->getString(4,selectedLine);
          int a = VAlert::box("Load File:"+file.last(20),"YES","NO");
          if (a == 1) {
            key = VK_ESCAPE;
            fileNameSelected = file;
          }
        } else {
          String file = directory->getString(6,selectedLine);
          int a = VTxtAlrt::box(file,"      Save File:      ","SAVE","Don't");
          if (a == 1) {
            key = VK_ESCAPE;
            fileNameSelected = lastPathName+file;
          }
        }
      }
    }
  }
  return 0;
}

void VFleSlct::setList(IStrLst *list, int listWidth) {
  VStrLst::setList(list,listWidth);
}

void VFleSlct::init(const String &path) {
  lastPathName=path;
  directory = new IFiles();
  directory->files = getFiles(path);
  setList(directory,4);
  directory->sort(0,true);
  setKeyboardFocus(this);
}
