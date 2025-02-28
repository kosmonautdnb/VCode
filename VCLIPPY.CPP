#include "vclippy.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constant.hpp"
#include "valert.hpp"
#include "input.hpp"
#include "pngload.hpp"
#include "vtxtalrt.hpp"
#include <math.h>
#include "vga.hpp"

extern bool showAppTime;

void VClippy::draw() {
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,0xff100004);
  cLayerStack->drawAtXY(0,0);
  cMnuBar->setRect(position.x,position.y,position.x+size.width,position.y+size.height);
  cMnuBar->text = getMenuString();
  cMnuBar->draw();
  cWPal->draw();
  cWZoom->draw();
  cWLayer->draw();
  cWTools->draw();
  switch(cWTools->currentTool) {
  case CWTools::TOOL_MOVE: toolMoveLayers(false); break;
  case CWTools::TOOL_PEN: toolPen(false); break;
  }
  popRect();
}

int VClippy::update() {
  if (mouseB==0) lockedMouse = false;
  if (!lockedMouse) {
      if (cMnuBar->update()!=0) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        return 0;
      }
      if (cWPal->update()!=0) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        return 0;
      }
      if (cWZoom->update()!=0) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        return 0;
      }
      if (cWLayer->update()!=0) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        return 0;
      }
      if (cWTools->update()!=0) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        return 0;
      }
  }
  toolMoveCanvas();
  switch(cWTools->currentTool) {
  case CWTools::TOOL_MOVE: toolMoveLayers(true); break;
  case CWTools::TOOL_PEN: toolPen(true); break;
  }

  if (function == "LOADPNG") {
    vFleSlct->init(currentPath);
    vFleSlct->setRect(frameBufferWidth/4,frameBufferHeight/4,frameBufferWidth*3/4,frameBufferHeight*3/4);
    vFleSlct->modeLoading = true;
    vFleSlct->doModal();
    load(vFleSlct->fileNameSelected,-1);
  }

  if (function == "LOADPNGINTOLAYER") {
    vFleSlct->init(currentPath);
    vFleSlct->setRect(frameBufferWidth/4,frameBufferHeight/4,frameBufferWidth*3/4,frameBufferHeight*3/4);
    vFleSlct->modeLoading = true;
    vFleSlct->doModal();
    load(vFleSlct->fileNameSelected,cWLayer->currentLayer);
  }

  if (function == "NEW") {
    String width = String(cLayerStack->width);
    String height = String(cLayerStack->height);
    int a = VTxtAlrt::box(width,"Canvas width:","Set", "Don't");
    if (a == 1) {
      a = VTxtAlrt::box(height,"Canvas height:","Set", "Don't");
      if (a == 1) {
        int w = width.toInt();
        int h = height.toInt();
        if (w < 0) w = 0;
        if (h < 0) h = 0;
        cLayerStack->newStack(w,h);
        CLayer *layer = new CLayer;
        layer->init(NULL,w,h);
        cLayerStack->layers.push_back(layer);
        cLayerStack->reBlit();
      }
    }
  }

  if (function == "CANVASSIZE") {
    String xp = "0";
    String yp = "0";
    String width = String(cLayerStack->width);
    String height = String(cLayerStack->height);
    int a = VTxtAlrt::box(xp,"Canvas x:","Set", "Don't");
    if (a == 1) {
      int a = VTxtAlrt::box(yp,"Canvas y:","Set", "Don't");
      if (a == 1) {
        int a = VTxtAlrt::box(width,"Canvas width:","Set", "Don't");
        if (a == 1) {
          a = VTxtAlrt::box(height,"Canvas height:","Set", "Don't");
          if (a == 1) {
            int x = xp.toInt();
            int y = yp.toInt();
            int w = width.toInt();
            int h = height.toInt();
            if (w < 0) w = 0;
            if (h < 0) h = 0;
            cLayerStack->reCanvas(x,y,x+w,y+h);
            cLayerStack->reBlit();
          }
        }
      }
    }
  }

  if (function == "ZOOM") {
    String zoom = String::flt(cLayerStack->zoom);
    int a = VTxtAlrt::box(zoom,"New zoom value:","Set", "Don't");
    if (a == 1) {
      cLayerStack->zoom = zoom.toFloat();
      if (cLayerStack->zoom>8) cLayerStack->zoom=8;
      if (cLayerStack->zoom<-8) cLayerStack->zoom=-8;
    }
  }


  if (function == "RESIZELAYER") {
    cWLayer->resize();
  }

  if (function == "SAVEPNG") {
    vFleSlct->init(currentPath);
    vFleSlct->setRect(frameBufferWidth/4,frameBufferHeight/4,frameBufferWidth*3/4,frameBufferHeight*3/4);
    vFleSlct->modeLoading = false;
    vFleSlct->doModal();
    String fileToSave = toLower(vFleSlct->fileNameSelected);
    save(vFleSlct->fileNameSelected,-1);
  }

  if (cWPal->rightClicked) {
    cWPal->rightClicked=false;
    function="PALED";
  }

  if (function == "PALED") {
    cWPalEd->init(currentPath,cWPal->foregroundColor);
    cWPalEd->setRect(frameBufferWidth/4,frameBufferHeight/4,frameBufferWidth*3/4,frameBufferHeight*3/4);
    cWPalEd->doModal();
  }

  if (function == "DOUBLEPIXELS") {
    doublePixels = true;
    cLayerStack->doublePixels(doublePixels);
    cLayerStack->reBlit();
  }

  if (function == "SINGLEPIXELS") {
    doublePixels = false;
    cLayerStack->doublePixels(doublePixels);
    cLayerStack->reBlit();
  }

  if (function == "SWITCHGRID") {
    gridOn = !gridOn;
    cLayerStack->enableGrid(gridOn,gridWidth,gridHeight);
    cLayerStack->reBlit();
  }

  showAppTime = true;
  if (key == VK_ESCAPE || function == "EXIT") {
    int a = VAlert::box("Exit Clippy?","YES","NO");
    if (a == 1) {
      key = VK_ESCAPE;
      showAppTime = true;
    }
  }

  if (key == '-') {
    cLayerStack->addStackZoom(-0.25f,mouseX,mouseY);
  }

  if (key == '+') {
    cLayerStack->addStackZoom(0.25f,mouseX,mouseY);
  }

  if (key == 'a' || function=="ROTATE90") {
    cLayerStack->layers[0]->selected = true;
    cLayerStack->rotateLayers(90.f);
    cLayerStack->reBlit();
  }

  if (function=="MIRRORX") {
    cLayerStack->layers[0]->selected = true;
    cLayerStack->mirrorLayers(-1,1);
    cLayerStack->reBlit();
  }

  if (function=="MIRRORY") {
    cLayerStack->layers[0]->selected = true;
    cLayerStack->mirrorLayers(1,-1);
    cLayerStack->reBlit();
  }

  cWPal->mouseOverColor=cLayerStack->getPixel(mouseX,mouseY);
  for (int y = 0; y < CWZOOMY; y++) {
    for (int x = 0; x < CWZOOMX; x++) {
      cWZoom->zoom[x+y*CWZOOMX] = cLayerStack->getPixel(mouseX+x-(CWZOOMX-1)/2,mouseY+y-(CWZOOMY-1)/2);
    }   
  }

  function = cMnuBar->clickedFunctionName; // one frame delay for redraw menubar
  cMnuBar->clickedFunctionName = "";
  lastMouseX = mouseX;
  lastMouseY = mouseY;
  return 0;
}

String VClippy::getMenuString() {
  String r = "VClippy\n(f)File+New[NEW]+(L)Load PNG[LOADPNG]+Load PNG into layer[LOADPNGINTOLAYER]+(S)Save PNG[SAVEPNG]+(Q)Exit[EXIT]\n";
  r+= "Edit+Undo+Redo[REDO]+Cut[CUT]+Copy[COPY]+----------"
  "+(R)Rotate 90 Degrees[ROTATE90]"
  "+(H)Flip horizontaly[MIRRORX]"
  "+(V)Flip verticaly[MIRRORY]"
  "+(P)Palettte[PALED]"
  "\n";
  r+= "Layer+Resize[RESIZELAYER]\n";
  r+= "Image+Canvas size[CANVASSIZE]\n";
  r+= "Text\n";
  r+= "Window+Zoom[ZOOM]";
  if (!doublePixels) r += "+Doubled width[DOUBLEPIXELS]";  else r += "+Normal width[SINGLEPIXELS]";
  if (!gridOn) r += "+Grid on[SWITCHGRID]"; else r += "+Grid off[SWITCHGRID]";
  r+= "\n";
  r+= "Effects\n";
  return r;
}

void VClippy::init(const String &path, int width, int height) {
  reset();
  currentPath = path;
  cLayerStack->newStack(0,0);
  cLayerStack->reBlit();
  cMnuBar->init(getMenuString());
  cWPal->setRect(frameBufferWidth*3/4,frameBufferHeight*3/4,frameBufferWidth,frameBufferHeight);
  cWZoom->setRect(frameBufferWidth*7/8,frameBufferHeight*4/8,frameBufferWidth,frameBufferHeight*5/8);
  cWLayer->setRect(frameBufferWidth*6/8,frameBufferHeight*1/8,frameBufferWidth,frameBufferHeight*3/8);
  cWLayer->init();
  cWTools->setRect(CHAR_HEIGHT,frameBufferHeight*2/8,frameBufferWidth*1/16,frameBufferHeight-CHAR_HEIGHT*2);
  cWTools->init();
}

void VClippy::load(const String &fileName, int layerNr) {
  if (getExtension(toLower(fileName))==".png") {
    ImageLayer l;
    if (loadPNG(&l, fileName.c_str())) {
      if (layerNr < 0) {
        cLayerStack->newStack(l.width,l.height);
        cLayerStack->layers.push_back(new CLayer());
        layerNr = 0;
      }
      if (layerNr < cLayerStack->layers.size()) {
        cLayerStack->layers[layerNr]->init(l.data,l.width,l.height);
        cLayerStack->reBlit();
      }
    }
    delete[] l.data;
  }
}

void VClippy::save(const String &fileName, int layerNr) {
  if (getExtension(toLower(fileName))==".png") {
    ImageLayer l;
    int a = 0;
    if (layerNr<0) {
      bool gridOn = cLayerStack->gridOn;
      cLayerStack->gridOn = false;
      cLayerStack->reBlit();
      cLayerStack->gridOn = gridOn;
      l.width = cLayerStack->width;
      l.height = cLayerStack->height;
      l.data = cLayerStack->image;
      a = 1;
      FILE *o = fopen(fileName.c_str(),"rb");
      if (o != NULL) {
        fclose(o);
        a = VAlert::box("Overwrite:"+fileName.last(20),"YES","Don't");
      }
    }
    if (a == 1) {
      savePNG(&l, fileName.c_str());
    }
  }
  cLayerStack->reBlit();
}

void VClippy::toolMoveCanvas() {
  if (mouseB==1 && ctrlKey)
    cLayerStack->moveStack(mouseX-lastMouseX,mouseY-lastMouseY);
}

void VClippy::toolMoveLayers(bool update) {
  if (!update) return;
  if (mouseB==1  && shiftKey && !ctrlKey) {
    lockedMouse=true;
  } else lockedMouse=false;

  if (lockedMouse) {
    cLayerStack->moveLayers(mouseX-lastMouseX,mouseY-lastMouseY);
    cLayerStack->reBlit();
  }
}

void VClippy::toolPen(bool update) {
  if (cWLayer->currentLayer>=cLayerStack->layers.size()) 
    return;
  CLayer *l = cLayerStack->layers[cWLayer->currentLayer];
  int mx,my;
  cLayerStack->mousePosToLayerPos(mouseX,mouseY,l,mx,my);
  if (mx<0||my<0||mx>=l->width||my>=l->height) return;

  if (!update) {
    int kx0,ky0;
    cLayerStack->layerPosToMousePos(mx,my,l,kx0,ky0);
    int kx1,ky1;
    cLayerStack->layerPosToMousePos(mx+1,my+1,l,kx1,ky1);
    if (kx0>kx1) {float t = kx0; kx0=kx1; kx1 = t;kx0+=1;kx1+=1;}
    if (ky0>ky1) {float t = ky0; ky0=ky1; ky1 = t;ky0+=1;ky1+=1;}
    drawRectangle(kx0-1,ky0-1,kx1,ky1,COLOR_WHITE);
  } else {
    if (mouseB==1 && !shiftKey && !altKey && !ctrlKey) {
      int mx2,my2;
      cLayerStack->mousePosToLayerPos(lastMouseX,lastMouseY,l,mx2,my2);
      int d = abs(mx2-mx);
      if (abs(my2-my)>d) d = abs(my2-my);
      l->setPixel(mx2,my2,palette[cWPal->foregroundColor]);
      for (int i = 0; i < d; i++) {
        int x = (mx2-mx)*i/d+mx;
        int y = (my2-my)*i/d+my;
        l->setPixel(x,y,palette[cWPal->foregroundColor]);
      }
      cLayerStack->reBlit();
    }
  }

}
