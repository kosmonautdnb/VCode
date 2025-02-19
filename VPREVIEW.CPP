#include "vpreview.hpp"
#include "blit.hpp"
#include "constant.hpp"
#include "input.hpp"
#include "dos.hpp"
#include "pngload.hpp"
#include "util.hpp"

void VPreview::draw() {
  if (data != NULL) {
    Point pos = position;
    Size siz = size;
    pushRect(pos.x,pos.y,pos.x+siz.width,pos.y+siz.height);
    for (int y = 0; y < siz.height; y++) {
      for (int x = 0; x < siz.width; x++) {
        setPixel(pos.x+x,pos.y+y,data[x+y*siz.width]);
      }
    }
    popRect();
  }
}

int VPreview::update() {
  if (lastFileName!=fileName) {
    lastFileName = fileName;
    if (data != NULL) {
      delete[] data;
      data = NULL;
    }
    String extension = toLower(getExtension(fileName));
    if (extension == ".png") {
      ImageLayer layer;
      loadPNG(&layer, fileName.c_str());
      if (layer.width>0&&layer.height>0) {
        size.height = size.width*layer.height/layer.width;
        if (size.height==0) size.height = 1;
        data = new unsigned int[size.width*size.height];
        unsigned int *d = data;
        int re=0,ge=0,be=0;
        int ry,rx,r2,g2,b2,r,g,b;
        unsigned int rgba,pal;
        for (int y = 0; y < size.height; y++) {
          ry = y * layer.height/size.height;
          for (int x = 0; x < size.width; x++) {
            rx = x * layer.width/size.width;
            rgba = layer.data[rx+ry*layer.width];
            r2 = (rgba & 255);
            g2 = ((rgba>>8) & 255);
            b2 = ((rgba>>16) & 255);
            r = r2 + re;
            g = g2 + ge;
            b = b2 + be;
            if (r<0) r = 0;
            if (g<0) g = 0;
            if (b<0) b = 0;
            if (r>255) r = 255;
            if (g>255) g = 255;
            if (b>255) b = 255;
            if (paletteCount > 0)
              pal = palette[getPaletteIndex(r|(g<<8)|(b<<16)|0xff000000)];
            else
              pal = rgba;
            re = r2-(pal & 255);
            ge = g2-((pal>>8) & 255);
            be = b2-((pal>>16) & 255);
            if ((x+y) & 1) {
              re = ge = be = 0;
            }
            *d++ = pal;
          }
        }
        delete[] layer.data;
      }
    }
  }
  return 0;
}

void VPreview::init(const String &_fileName) {
  fileName = _fileName;
  update();
}