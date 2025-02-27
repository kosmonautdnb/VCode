#include "c_wpaled.hpp"
#include "blit.hpp"
#include "constant.hpp"
#include "colors.hpp"
#include "vga.hpp"
#include "input.hpp"
#include "util.hpp"
#include "string.hpp"
#include <math.h>
#include "pngload.hpp"

uint32_t swapRB(uint32_t rgba) {
  return (rgba&0xff00ff00)|((rgba&0xff)<<16)|((rgba>>16)&0xff);
}

static float sin2(float a) {
  a /= 2.f * 3.1415927f;
  a = fmod(a+100.f,1.f);
  if (a > 0.5) return 0.f;
  if (a < 0.25) return a / 0.25f;
  return 1.f-(a-0.25f)/0.25f;
}

static uint32_t getCol(float h,float s,float l) {
  float r = sin2(h*2.f*3.1415927f);
  float g = sin2(h*2.f*3.1415927f+2.f*3.1415927f/3);
  float b = sin2(h*2.f*3.1415927f+2.f*2.f*3.1415927f/3);
  r += s * 1.f;
  g += s * 1.f;
  b += s * 1.f;
  int ri = r*255.f;
  int gi = g*255.f;
  int bi = b*255.f;
  if (ri<0)ri = 0;
  if (gi<0)gi = 0;    
  if (bi<0)bi = 0;
  if (ri>255) ri = 255;
  if (gi>255) gi = 255;    
  if (bi>255) bi = 255;
  ri *= l;
  gi *= l;
  bi *= l;
  return (ri|(gi<<8)|(bi<<16)|0xff000000);
}

static uint32_t getCol2(float h,float s,float l) {
  int ri = h*255.f;
  int gi = s*255.f;
  int bi = l*255.f;
  if (ri<0)ri = 0;
  if (gi<0)gi = 0;    
  if (bi<0)bi = 0;
  if (ri>255) ri = 255;
  if (gi>255) gi = 255;    
  if (bi>255) bi = 255;
  return (ri|(gi<<8)|(bi<<16)|0xff000000);
}

void CWPalEd::draw() {
  String n = name;
  if (mouseOverColor != 0x00) name += " #" + String::hex(swapRB(mouseOverColor),8);
  CWBase::draw();
  name = n;
  position.y += CHAR_HEIGHT;
  size.height -= CHAR_HEIGHT;
  pushRect(position,size);
  int mul = 4*frameBufferWidth/320;
  for (int y = 0; y < 16*mul; y++) {
    for (int x = 0; x < 16*mul; x++) {
      int index = x/mul+y/mul*16;
      setPixel(position.x+x,position.y+y,palette[index]);
    }
  }
  int c0 = startColor;
  int c1 = endColor;
  drawRectangle(position.x+(c0&15)*mul-2,position.y+(c0/16)*mul-2,position.x+(c0&15)*mul+mul+2,position.y+(c0/16)*mul+mul+2,COLOR_RED);
  drawRectangle(position.x+(c1&15)*mul-1,position.y+(c1/16)*mul-1,position.x+(c1&15)*mul+mul+1,position.y+(c1/16)*mul+mul+1,COLOR_RED);
  drawRectangle(position.x+(c0&15)*mul-1,position.y+(c0/16)*mul-1,position.x+(c0&15)*mul+mul+1,position.y+(c0/16)*mul+mul+1,COLOR_WHITE);

  drawString(position.x+1,position.y+16*mul,"Start:$"+String::hex(c0,2),COLOR_BLACK);
  drawString(position.x+1,position.y+16*mul+CHAR_HEIGHT,"End:$"+String::hex(c1,2),COLOR_BLACK);
  drawBox(position.x+1,position.y+16*mul+CHAR_HEIGHT*2, position.x+1+12,position.y+16*mul+CHAR_HEIGHT*2+12,palette[startColor]);
  
  if (startColor != endColor) { 
    int x = position.x+1+12+4;
    vButtonFlip->setRect(x,3+position.y+16*mul+CHAR_HEIGHT*2-2,x+vButtonFlip->text.pixelWidth()+4,3+position.y+16*mul+CHAR_HEIGHT*3+2);
    vButtonFlip->draw();
    x += vButtonFlip->text.pixelWidth()+4+4;
    vButtonCopy->setRect(x,3+position.y+16*mul+CHAR_HEIGHT*2-2,x+vButtonCopy->text.pixelWidth()+4,3+position.y+16*mul+CHAR_HEIGHT*3+2);
    vButtonCopy->draw();
  }

  mouseOverColorIndex = -1;
  if (mouseX>=position.x&&mouseY>=position.y&&mouseX<position.x+mul*16&&mouseY<position.y+mul*16) {
    mouseOverColorIndex = (mouseX-position.x)/mul+(mouseY-position.y)/mul*16;
  }

  int px = position.x+16*mul+4;
  int py = position.y+16*mul;
  int s = size.height-(py-position.y)-2;
  py -= 8;
  int ld = 256*256*3;
  int lx = 0; 
  int ly = 0; 
  int r0 = palette[startColor] & 255;
  int g0 = (palette[startColor]>>8) & 255;
  int b0 = (palette[startColor]>>16) & 255;
  for(int y2 = 0; y2 < s; y2++) {
    float yf = (float)y2/s;
    for(int x2 = 0; x2 < s; x2++) {
      float xf = (float)x2/s;
      uint32_t col=getCol(xf,yf,1.f-slider_Intens->value);
      int r1 = col & 255;
      int g1 = (col>>8) & 255;
      int b1 = (col>>16) & 255;
      int d = (r1-r0)*(r1-r0)+(g1-g0)*(g1-g0)+(b1-b0)*(b1-b0);
      if (d < ld) {
        ld = d;
        lx = x2;
        ly = y2;
      }
      setPixel(px+x2,py+y2,col);
    }
  }
  setPixel(px+lx,py+ly,COLOR_WHITE);
  setPixel(px+lx-1,py+ly,COLOR_BLACK);
  setPixel(px+lx+1,py+ly,COLOR_BLACK);
  setPixel(px+lx,py+ly-1,COLOR_BLACK);
  setPixel(px+lx,py+ly+1,COLOR_BLACK);
  slider_Intens->setRect(px+s+4,py,px+s+4+CHAR_HEIGHT,py+s);
  slider_Intens->draw();
  if (mouseX>=px&&mouseY>=py&&mouseX<px+s&&mouseY<py+s) {
      float xf = (float)(mouseX-px)/s;
      float yf = (float)(mouseY-py)/s;
      uint32_t rgba=getCol(xf,yf,1.f-slider_Intens->value);
      if (mouseB==1) {
        setColor(rgba);
        setPalette(startColor,rgba);
      }
  }   

  ld = 256*256*3;
  lx = 0; 
  ly = 0; 
  int px2 = px+s+4+CHAR_HEIGHT+4;
  int py2 = py;
  int s2 = s*0.5;
  for(int y3 = 0; y3 < s2; y3++) {
    float yf = (float)y3/s2;
    for(int x3 = 0; x3 < s2; x3++) {
      float xf = (float)x3/s2;
      uint32_t col=getCol2(xf,yf,slider_Intens->value);
      int r1 = col & 255;
      int g1 = (col>>8) & 255;
      int b1 = (col>>16) & 255;
      int d = (r1-r0)*(r1-r0)+(g1-g0)*(g1-g0)+(b1-b0)*(b1-b0);
      if (d < ld) {
        ld = d;
        lx = x3;
        ly = y3;
      }
      setPixel(px2+x3,py2+y3,col);
    }
  }
  setPixel(px2+lx,py2+ly,COLOR_WHITE);
  setPixel(px2+lx-1,py2+ly,COLOR_BLACK);
  setPixel(px2+lx+1,py2+ly,COLOR_BLACK);
  setPixel(px2+lx,py2+ly-1,COLOR_BLACK);
  setPixel(px2+lx,py2+ly+1,COLOR_BLACK);
  if (mouseX>=px2&&mouseY>=py2&&mouseX<px2+s2&&mouseY<py2+s2) {
      float xf = (float)(mouseX-px2)/s2;
      float yf = (float)(mouseY-py2)/s2;
      uint32_t rgba=getCol2(xf,yf,slider_Intens->value);
      if (mouseB==1) {
        setColor(rgba);
        setPalette(startColor,rgba);
      }
  }   

  int k = 32*frameBufferWidth/320;
  slider_R->setRect(position.x+17*mul,position.y+CHAR_HEIGHT*0,position.x+17*mul+k,position.y+CHAR_HEIGHT*1+1);
  slider_G->setRect(position.x+17*mul,position.y+CHAR_HEIGHT*2,position.x+17*mul+k,position.y+CHAR_HEIGHT*3+1);
  slider_B->setRect(position.x+17*mul,position.y+CHAR_HEIGHT*4,position.x+17*mul+k,position.y+CHAR_HEIGHT*5+1);
  slider_A->setRect(position.x+17*mul,position.y+CHAR_HEIGHT*6,position.x+17*mul+k,position.y+CHAR_HEIGHT*7+1);
  k+=4;
  enter_R->setRect(k+position.x+17*mul,position.y+CHAR_HEIGHT*0,k+position.x+17*mul+CHAR_HEIGHT*2+1,position.y+CHAR_HEIGHT*1+1);
  enter_G->setRect(k+position.x+17*mul,position.y+CHAR_HEIGHT*2,k+position.x+17*mul+CHAR_HEIGHT*2+1,position.y+CHAR_HEIGHT*3+1);
  enter_B->setRect(k+position.x+17*mul,position.y+CHAR_HEIGHT*4,k+position.x+17*mul+CHAR_HEIGHT*2+1,position.y+CHAR_HEIGHT*5+1);
  enter_A->setRect(k+position.x+17*mul,position.y+CHAR_HEIGHT*6,k+position.x+17*mul+CHAR_HEIGHT*2+1,position.y+CHAR_HEIGHT*7+1);
  enter_RGBA->setRect(position.x+17*mul,position.y+CHAR_HEIGHT*8,position.x+17*mul+CHAR_HEIGHT*7+1,position.y+CHAR_HEIGHT*9+1);
  slider_R->draw();
  slider_G->draw();
  slider_B->draw();
  slider_A->draw();
  enter_R->draw();
  enter_G->draw();
  enter_B->draw();
  enter_A->draw();
  enter_RGBA->draw();
  drawString(enter_R->position.x + enter_R->size.width + 1,enter_R->position.y+1,"Red:"+String(red),COLOR_BLACK);
  drawString(enter_G->position.x + enter_G->size.width + 1,enter_G->position.y+1,"Green:"+String(green),COLOR_BLACK);
  drawString(enter_B->position.x + enter_B->size.width + 1,enter_B->position.y+1,"Blue:"+String(blue),COLOR_BLACK);
  drawString(enter_A->position.x + enter_A->size.width + 1,enter_A->position.y+1,"Alpha:"+String(alpha),COLOR_BLACK);
  drawString(enter_RGBA->position.x + enter_RGBA->size.width + 1,enter_RGBA->position.y+1,"ARGB",COLOR_BLACK);
  popRect();
  position.y -= CHAR_HEIGHT;
  size.height += CHAR_HEIGHT;
  cMnuBar->setRect(position.x,position.y,position.x+size.width,position.y+size.height);
  cMnuBar->draw();
}

int CWPalEd::update() {
  int r = CWBase::update();
  if (r) return r;
  if (cMnuBar->update()!=0) {
    return 0;
  }
  
  if (!mouseIn()) {
    mouseOverColor = lastCapturedFrameBuffer[mouseX+mouseY*frameBufferWidth];
    if (mouseB==0x00&&lastMouseB==0x02)
    {
      setPalette(startColor,mouseOverColor);
      setColor(mouseOverColor);
    }
  } else {
    mouseOverColor = 0x00;
  }

  if (startColor!=endColor) {
    vButtonFlip->update();
    vButtonCopy->update();
    if (vButtonFlip->clicked()) {
      vButtonFlip->wasClicked=false;
      uint32_t t = palette[startColor];
      setPalette(startColor,palette[endColor]);
      setPalette(endColor,t);
      setColor(palette[startColor]);
    }
    if (vButtonCopy->clicked()) {
      vButtonCopy->wasClicked=false;
      setPalette(endColor,palette[startColor]);
    }
  }

  r = enter_R->update();
  r |= enter_G->update();
  r |= enter_B->update();
  r |= enter_A->update();
  r |= enter_RGBA->update();

  r |= slider_R->update();
  r |= slider_G->update();
  r |= slider_B->update();
  r |= slider_A->update();

  r |= slider_Intens->update();

  if (mouseOverColorIndex!=-1) {
    if (mouseB==1) {
      startColor = mouseOverColorIndex;
      setColor(palette[startColor]);
    }
    if (mouseB==2)
      endColor = mouseOverColorIndex;
  }

  if (slider_R->hasChanged()) {
    int k = (int)slider_R->value;
    rgba &= 0xffffff00;
    rgba |= k;
    setColor(rgba);
    setPalette(startColor,rgba);
  }

  if (slider_G->hasChanged()) {
    int k = (int)slider_G->value;
    rgba &= 0xffff00ff;
    rgba |= k << 8;
    setColor(rgba);
    setPalette(startColor,rgba);
  }

  if (slider_B->hasChanged()) {
    int k = (int)slider_B->value;
    rgba &= 0xff00ffff;
    rgba |= k << 16;
    setColor(rgba);
    setPalette(startColor,rgba);
  }

  if (slider_A->hasChanged()) {
    int k = (int)slider_A->value;
    rgba &= 0x00ffffff;
    rgba |= k << 24;
    setColor(rgba);
    setPalette(startColor,rgba);
  }

  if (key == VK_ENTER) {
    red = enter_R->text.fromHex() & 255;
    green = enter_G->text.fromHex() & 255;
    blue = enter_B->text.fromHex() & 255;
    alpha = enter_A->text.fromHex() & 255;
    rgba = swapRB(enter_RGBA->text.fromHex()) & 0xffffffff;
    if (hasKeyboardFocus(enter_R)) rgba = (rgba & 0xffffff00)|(red<<0);
    if (hasKeyboardFocus(enter_G)) rgba = (rgba & 0xffff00ff)|(green<<8);
    if (hasKeyboardFocus(enter_B)) rgba = (rgba & 0xff00ffff)|(blue<<16);
    if (hasKeyboardFocus(enter_A)) rgba = (rgba & 0x00ffffff)|(alpha<<24);
    if (hasKeyboardFocus(enter_RGBA)) {
      red = (rgba>>0) & 255;
      green = (rgba>>8) & 255;
      blue = (rgba>>16) & 255;
      alpha = (rgba>>24) & 255;
    }
    setColor(rgba);
    setPalette(startColor,rgba);
  }

  if (function == "COPY") {
    int startColor2 = startColor;
    int endColor2 = endColor;
    if (startColor2>endColor2) {
      int t = startColor2;
      startColor2 = endColor2;
      endColor2 = t;
    }
    clipBoard.resize((endColor2+1-startColor2)*4);
    memcpy(&(clipBoard[0]),&palette[startColor2],clipBoard.length());
  }

  if (function == "ERASE") {
    int startColor2 = startColor;
    int endColor2 = endColor;
    if (startColor2>endColor2) {
      int t = startColor2;
      startColor2 = endColor2;
      endColor2 = t;
    }
    for (int i = startColor2; i<256; i++) {
      if (i+endColor2+1-startColor2<256)
        setPalette(i,palette[i+endColor2+1-startColor2]);
      else
        setPalette(i,0xff000000);
    }
    startColor=startColor2;
    endColor=startColor2;
    setColor(palette[startColor]);
  }

  if (function == "PASTE") {
    for (int i = startColor; i<startColor+clipBoard.length()/4&&i<256; i++) {
      setPalette(i,((uint32_t*)(&clipBoard[0]))[i-startColor]);
    }
    setColor(palette[startColor]);
  }

  if (function == "INSERT") {
    for (int i = 255; i>=startColor; i--) {
      int i2 = i+clipBoard.length()/4;
      if (i2<256)
        setPalette(i2,palette[i]);
    }
    for (int j = 0; j < clipBoard.length()/4; j++)
      if (j+startColor<256)
        setPalette(j+startColor,((uint32_t*)(&clipBoard[0]))[j]);
    setColor(palette[startColor]);
  }

  if (function == "LOAD") {
    vFleSlct->init(currentPath);
    vFleSlct->setRect(frameBufferWidth/4,frameBufferHeight/4,frameBufferWidth*3/4,frameBufferHeight*3/4);
    vFleSlct->modeLoading = true;
    vFleSlct->doModal();
    loadPalette(vFleSlct->fileNameSelected);
  }

  if (function == "PALFROMPICTURE") {
    vFleSlct->init(currentPath);
    vFleSlct->setRect(frameBufferWidth/4,frameBufferHeight/4,frameBufferWidth*3/4,frameBufferHeight*3/4);
    vFleSlct->modeLoading = true;
    vFleSlct->doModal();
    paletteFromPicture(vFleSlct->fileNameSelected);
    function = "SORTUP";
    setColor(palette[startColor]);
  }

  if (function == "SORTUP") {
    int startColor2 = startColor;
    int endColor2 = endColor;
    if (startColor2>endColor2) {
      int t = startColor2;
      startColor2 = endColor2;
      endColor2 = t;
    }
    for(int i = startColor2; i <= endColor2; i++) {
      for(int j = i + 1; j <= endColor2; j++) {
        uint32_t rgba = palette[i];
        int r = rgba & 255;        
        int g = (rgba>>8) & 255;        
        int b = (rgba>>16) & 255;        
        int h1 = r*r*0.25*0.25+g*g*0.6*0.6+b*b*0.15*0.15;
        uint32_t rgba2 = palette[j];
        int r2 = rgba2 & 255;        
        int g2 = (rgba2>>8) & 255;        
        int b2 = (rgba2>>16) & 255;        
        int h2 = r2*r2*0.25*0.25+g2*g2*0.6*0.6+b2*b2*0.15*0.15;
        if (h2 < h1) {
          uint32_t t = palette[i];
          setPalette(i,palette[j]);
          setPalette(j,t);
        }
      }
    }
    setColor(palette[startColor]);
  }                                                 

  if (function == "SORTDOWN") {
    int startColor2 = startColor;
    int endColor2 = endColor;
    if (startColor2>endColor2) {
      int t = startColor2;
      startColor2 = endColor2;
      endColor2 = t;
    }
    for(int i = startColor2; i <= endColor2; i++) {
      for(int j = i + 1; j <= endColor2; j++) {
        uint32_t rgba = palette[i];
        int r = rgba & 255;        
        int g = (rgba>>8) & 255;        
        int b = (rgba>>16) & 255;        
        int h1 = r*r*0.25*0.25+g*g*0.6*0.6+b*b*0.15*0.15;
        uint32_t rgba2 = palette[j];
        int r2 = rgba2 & 255;        
        int g2 = (rgba2>>8) & 255;        
        int b2 = (rgba2>>16) & 255;        
        int h2 = r2*r2*0.25*0.25+g2*g2*0.6*0.6+b2*b2*0.15*0.15;
        if (h2 > h1) {
          uint32_t t = palette[i];
          setPalette(i,palette[j]);
          setPalette(j,t);
        }
      }
    }
    setColor(palette[startColor]);
  }                                                 

  if (function == "MAKEGRADIENT") {
    int startColor2 = startColor;
    int endColor2 = endColor;
    if (startColor2>endColor2) {
      int t = startColor2;
      startColor2 = endColor2;
      endColor2 = t;
    }
    float r0 = palette[startColor2] & 255;
    float g0 = (palette[startColor2]>>8) & 255;
    float b0 = (palette[startColor2]>>16) & 255;
    float a0 = (palette[startColor2]>>24) & 255;
    float r1 = palette[endColor2] & 255;
    float g1 = (palette[endColor2]>>8) & 255;
    float b1 = (palette[endColor2]>>16) & 255;
    float a1 = (palette[endColor2]>>24) & 255;
    for (int i = 0; i <= endColor2-startColor2; i++) {
      float ri = (float)i / (endColor2-startColor2);
      int r = (int)((r1-r0)*ri+r0);
      int g = (int)((g1-g0)*ri+g0);
      int b = (int)((b1-b0)*ri+b0);
      int a = (int)((a1-a0)*ri+a0);
      int32_t rgba = r|(g<<8)|(b<<16)|(a<<24);
      setColor(rgba);
      setPalette(i+startColor2,rgba);
    }
    setColor(palette[startColor]);
  }

  if (function == "SAVE") {
    vFleSlct->init(currentPath);
    vFleSlct->setRect(frameBufferWidth/4,frameBufferHeight/4,frameBufferWidth*3/4,frameBufferHeight*3/4);
    vFleSlct->modeLoading = false;
    vFleSlct->doModal();
    savePalette(vFleSlct->fileNameSelected);
  }

  if (function == "EXIT") {
    key = VK_ESCAPE;
  }

  function = cMnuBar->clickedFunctionName; // one frame delay for redraw menubar
  cMnuBar->clickedFunctionName = "";
  return mouseIn() ? 1 : 0;
}
          
void CWPalEd::setColor(int color) {
  red = color & 255;
  green = (color>>8) & 255;
  blue = (color>>16) & 255;
  alpha = (color>>24) & 255;
  rgba = color;
  enter_R->text = String::hex(red,2);
  enter_G->text = String::hex(green,2);
  enter_B->text = String::hex(blue,2);
  enter_A->text = String::hex(alpha,2);
  enter_RGBA->text = String::hex(swapRB(rgba),8);
  slider_R->value = red;
  slider_G->value = green;
  slider_B->value = blue;
  slider_A->value = alpha;
}

void CWPalEd::init(const String &path, int selectedColorIndex) {
  currentPath = path;
  startColor = selectedColorIndex;
  endColor = selectedColorIndex;
  enter_R->init("00");
  enter_G->init("00");
  enter_B->init("00");
  enter_A->init("00");
  enter_RGBA->init("00000000");
  slider_R->init(0,0,255);
  slider_G->init(0,0,255);
  slider_B->init(0,0,255);
  slider_A->init(0,0,255);
  slider_Intens->init(0,0,1);
  setColor(palette[startColor]);
  String r = "(AltF)File+(l)Load[LOAD]+(s)Save[SAVE]+(q)Exit[EXIT]\n";
  r += "(t)Tools+Palette from picture[PALFROMPICTURE]+Make gradient[MAKEGRADIENT]+Sort up[SORTUP]+Sort down[SORTDOWN]+Copy[COPY]+Erase[ERASE]+Paste[PASTE]+Insert[INSERT]\n";
  cMnuBar->init(r);
  vButtonFlip->init("Flip");
  vButtonCopy->init("Copy");
}

void CWPalEd::loadPalette(const String &path) {
  String data = stringFromFile(path,false);
  unsigned int *r = (unsigned int*)&(data[0]);
  for (int i = 0; i < 256 && i < data.length()/4; i++) {
    setPalette(i,r[i]);
  }
}

void CWPalEd::savePalette(const String &path) {
  String palString;
  palString.resize(256*4);
  memcpy(&(palString[0]),palette,256*4);
  stringToFile(palString,path,false);
}

void CWPalEd::paletteFromPicture(const String &fileName) {
  int startColor2 = startColor;
  int endColor2 = endColor;
  if (startColor2>endColor2) {
    int t = startColor2;
    startColor2 = endColor2;
    endColor2 = t;
  }

  if (getExtension(toLower(fileName))==".png") {
    ImageLayer l;
    if (loadPNG(&l, fileName.c_str())) {
      int colorCount = endColor2-startColor2;
      Array<Array<unsigned int> > kMeansParticles;
      Array<float> kMeansCenters_r;
      Array<float> kMeansCenters_g;
      Array<float> kMeansCenters_b;
      kMeansParticles.resize(colorCount);
      kMeansCenters_r.resize(colorCount);
      kMeansCenters_g.resize(colorCount);
      kMeansCenters_b.resize(colorCount);
      for(int i = 0; i < colorCount; i++) {
        kMeansCenters_r[i]=(rand()&1023)/4.f;
        kMeansCenters_g[i]=(rand()&1023)/4.f;
        kMeansCenters_b[i]=(rand()&1023)/4.f;
        kMeansParticles[i].clear();
      }
      for(int j = 0; j < 32; j++) {
        for(int i = 0; i < colorCount; i++) {
          kMeansParticles[i].clear();
        }
        for (int k = 0; k < l.width*l.height; k++) {
          uint32_t rgba = l.data[k];
          int r = rgba & 255;        
          int g = (rgba>>8) & 255;        
          int b = (rgba>>16) & 255;        
          int center=0;
          float ld = 256*256*4;
          for (int v = 0; v < kMeansCenters_r.size(); v++) {
            float r2 = kMeansCenters_r[v];        
            float g2 = kMeansCenters_g[v];        
            float b2 = kMeansCenters_b[v];        
            float d = (r2-r)*(r2-r)+(g2-g)*(g2-g)+(b2-b)*(b2-b);
            if (d < ld) {
              ld = d;
              center=v;
            }
          }
          kMeansParticles[center].push_back(rgba);
        }
        for (int w = kMeansCenters_r.size()-1; w >= 0; w--) {
          float r = 0;
          float g = 0;
          float b = 0;
          int s = kMeansParticles[w].size();
          for (int u = 0; u < s; u++) {
            uint32_t rgba2 = kMeansParticles[w][u];
            int r2 = rgba2 & 255;        
            int g2 = (rgba2>>8) & 255;        
            int b2 = (rgba2>>16) & 255;        
            r += r2;
            g += g2;
            b += b2;
          }
          if (s>0) {
            r/=(float)s;
            g/=(float)s;
            b/=(float)s;
            kMeansCenters_r[w]=r;
            kMeansCenters_g[w]=g;
            kMeansCenters_b[w]=b;
          } else {
            kMeansCenters_r.erase(w,1);
            kMeansCenters_g.erase(w,1);
            kMeansCenters_b.erase(w,1);
          }
        }
      }
      for (int w = 0; w < kMeansCenters_r.size(); w++) {
        int r = kMeansCenters_r[w];
        int g = kMeansCenters_g[w];
        int b = kMeansCenters_b[w];
        uint32_t rgba = r|(g<<8)|(b<<16)|0xff000000;
        setPalette(w+startColor2,rgba);
      }
      if (startColor<endColor) 
        endColor=startColor+kMeansCenters_r.size()-1;
      else
        startColor=endColor+kMeansCenters_r.size()-1;
    }
  }
}
