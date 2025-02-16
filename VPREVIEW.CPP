#include "vpreview.hpp"
#include "blit.hpp"
#include "constant.hpp"
#include "vga.hpp"
#include "dos.hpp"
#include "pngload.hpp"

void VPreview::draw() {
  if (viewMode != VIEWMODE_NONE) {
    Point pos = position;
    if (fullScreen)
      pos = Point(0,0);
    pushRect(pos.x,pos.y,pos.x+viewSize.width,pos.y+viewSize.height);
    drawBox(pos.x,pos.y,pos.x+viewSize.width,pos.y+viewSize.height,COLOR_BLACK);
    if (data != NULL) {
      switch(viewMode) {
        case VIEWMODE_IMAGE: {
            for (int y = 0; y < viewSize.height; y++) {
              for (int x = 0; x < viewSize.width; x++) {
                setPixel(pos.x+x,pos.y+y,data[x+y*viewSize.width]);
              }
            }
            if (key!=0&&key!=13&&key!=80*256&&key!=72*256) {fullScreen=false;lastFileName="";}
        } break;
      }
      popRect();
    }
  } else {
    fullScreen=false;
  }
}

void VPreview::update() {
  if (lastFileName!=fileName) {
    viewMode = VIEWMODE_NONE;
    lastFileName = fileName;
    if (data != NULL) {
      delete[] data;
      data = NULL;
    }
    String extension = getExtension(fileName);
    if (toLower(extension) == ".png") {
      ImageLayer layer;
      loadPNG(&layer, fileName.c_str());
      if (layer.width>0&&layer.height>0) {
        viewMode = VIEWMODE_IMAGE;
        viewSize = size;
        if (fullScreen) {
          viewSize = Size(320,200);
          if (layer.width>layer.height)
            viewSize.height = viewSize.width * layer.height / layer.width;
          else
            viewSize.width = viewSize.height * layer.width / layer.height;
        } else {
          viewSize.height = viewSize.width * layer.height / layer.width;
        }
        data = new unsigned int[viewSize.width*viewSize.height];
        unsigned int *d = data;
        for (int y = 0; y < viewSize.height; y++) {
          int ry = y * layer.height/viewSize.height;
          for (int x = 0; x < viewSize.width; x++) {
            int rx = x * layer.width/viewSize.width;
            *d++ = layer.data[rx+ry*layer.width];
          }
        }
        free(layer.data);
      }
    }
  }
  if (hasKeyboardFocus(this)) {
  }
}