#include "c_wlayer.hpp"
#include "blit.hpp"
#include "constant.hpp"
#include "colors.hpp"
#include "vclippy.hpp"
#include "vtxtalrt.hpp"
#include "input.hpp"

String getLayerTypeString(CLayer *l) {
  String layerType = "<type>"; 
  switch(l->layerType) {
    case CLayer::LAYERTYPE_NONE: layerType="None";break;
    case CLayer::LAYERTYPE_RASTER: layerType="Raster";break;
  }
  return layerType;
}

String getLayerFunctionString(CLayer *l) {
  String layerFunction = "<func>"; 
    switch(l->layerFunction) {
    case CLayer::LAYERFUNCTION_NONE: layerFunction="None";break;
    case CLayer::LAYERFUNCTION_ALPHA: layerFunction="Alpha";break;
    }
  return layerFunction;
}

void CWLayer::draw() {
  CWBase::draw();
  pushRect(position.x,position.y,position.x+size.width,position.y+size.height);
  CLayerStack *layerStack = data->cLayerStack;

  Point pos = position;
  if (currentLayer<layerStack->layers.size()) {
    CLayer *l = layerStack->layers[currentLayer];
    String layerType = getLayerTypeString(l);
    String layerFunction = getLayerFunctionString(l);
    String n = layerType + ":" + layerFunction+" " + String::flt(l->opacity);
    drawString(position.x,position.y+CHAR_HEIGHT,n, COLOR_BLACK);
    opacity->setRect(position.x,position.y+CHAR_HEIGHT*2,position.x+size.width,position.y+CHAR_HEIGHT*2+6);
    opacity->value = l->opacity;
    opacity->draw();
    drawString(position.x,position.y+CHAR_HEIGHT*2,"Opacity",COLOR_WHITE);
    pos.y += CHAR_HEIGHT*2+6;
  }
  int posy = pos.y;
  pushRect(pos.x,pos.y,position.x+size.width,position.y+size.height);
  pos.y = position.y+size.height;
  pos.y += (1-scroll->value) * (layerStack->layers.size()*CHAR_HEIGHT*2);
  mouseOverLayer = -1;
  mouseOverVisible = false;
  mouseOverSelected = false;
  for (int i = 0; i < layerStack->layers.size(); i++) {
    pos.y -= CHAR_HEIGHT*2;
    pushRect(pos.x,pos.y,pos.x+size.width,pos.y+CHAR_HEIGHT*2);
    if (mouseOver(Rect(pos.x,pos.y,pos.x+size.width,pos.y+CHAR_HEIGHT*2)) && mouseY>posy)
      mouseOverLayer = i;
    CLayer *layer = layerStack->layers[i];
    bool selected = layer->selected;
    bool visible = layer->visible;
    uint32_t colText = COLOR_BLACK;
    uint32_t colBack = COLOR_WHITE;
    if (selected) {
      colText = COLOR_WHITE;
      colBack = COLOR_BLACK;
    }
    drawBox(pos.x,pos.y,pos.x+size.width,pos.y+CHAR_HEIGHT*2,colBack);
    drawRectangle(pos.x,pos.y,pos.x+size.width,pos.y+1,colText);
    if (i == currentLayer) {
      drawBox(pos.x+size.width*2/3,pos.y,pos.x+size.width,pos.y+CHAR_HEIGHT*2,COLOR_RED);
    }

    
    if (mouseOver(Rect(pos.x+1,pos.y+1,pos.x+CHAR_HEIGHT-1,pos.y+CHAR_HEIGHT-1))) mouseOverVisible = true;
    drawBox(pos.x+1,pos.y+1,pos.x+CHAR_HEIGHT-1,pos.y+CHAR_HEIGHT-1,visible ? COLOR_WHITE : COLOR_BLACK);
    drawRectangle(pos.x+1,pos.y+1,pos.x+CHAR_HEIGHT-1,pos.y+CHAR_HEIGHT-1, colText);

    if (mouseOver(Rect(pos.x+1,pos.y+1+CHAR_HEIGHT,pos.x+CHAR_HEIGHT-1,pos.y+CHAR_HEIGHT*2-1))) mouseOverSelected = true;
    drawBox(pos.x+1,pos.y+1+CHAR_HEIGHT,pos.x+CHAR_HEIGHT-1,pos.y+CHAR_HEIGHT*2-1,selected ? COLOR_WHITE : COLOR_BLACK);
    drawRectangle(pos.x+1,pos.y+1+CHAR_HEIGHT,pos.x+CHAR_HEIGHT-1,pos.y+CHAR_HEIGHT*2-1, colText);
    
    drawString(pos.x+CHAR_HEIGHT,pos.y+1, layer->name+"|" + getLayerTypeString(layer), colText);
    drawString(pos.x+CHAR_HEIGHT,pos.y+1+CHAR_HEIGHT,String(selected ? "sel " : "") + String(visible ? "vis ":"")+String(layer->width)+":"+String(layer->height), colText);
    popRect();
  }
  popRect();
  popRect();
  scroll->setRect(position.x+size.width-4,posy+1,position.x+size.width-1,position.y+size.height-1);
  //scroll->draw();
  cMnuBar->setRect(position.x,position.y,position.x+size.width,position.y+size.height);
  cMnuBar->draw();
}

int CWLayer::update() {
  int r = CWBase::update();
  if (r) return r;
  if (cMnuBar->update()!=0) {
    return 0;
  }
  opacity->update();
  //if (scroll->update()) return 1;
      
  String function = cMnuBar->clickedFunctionName;
  cMnuBar->clickedFunctionName = "";
  CLayerStack *layerStack = data->cLayerStack;

  if (lastMouseB==1&&mouseB==0) {
    if (mouseOverLayer!=-1)
      currentLayer = mouseOverLayer;
    if (mouseOverVisible && currentLayer < layerStack->layers.size()) {
      layerStack->layers[currentLayer]->visible = !layerStack->layers[currentLayer]->visible;
      layerStack->reBlit();
    }
    if (mouseOverSelected && currentLayer < layerStack->layers.size()) {
      layerStack->layers[currentLayer]->selected = !layerStack->layers[currentLayer]->selected;
      layerStack->reBlit();
    }
  }

  if (lastMouseB==2&&mouseB==0) {
      if (mouseOverLayer >=0 && mouseOverLayer<layerStack->layers.size())
        layerStack->layers[mouseOverLayer]->selected = !layerStack->layers[mouseOverLayer]->selected;
  }

  if (mouseB==2) {
    if (lastMouseOverLayer >= 0 && abs(lastMouseOverLayer-mouseOverLayer)==1) {
      if (mouseOverLayer >=0 && mouseOverLayer<layerStack->layers.size() && lastMouseOverLayer >=0 && lastMouseOverLayer<layerStack->layers.size()) {
        CLayer *l = layerStack->layers[lastMouseOverLayer];
        layerStack->layers[lastMouseOverLayer] = layerStack->layers[mouseOverLayer];
        layerStack->layers[mouseOverLayer] = l;
        layerStack->reBlit();
      }
    }
    lastMouseOverLayer = mouseOverLayer;
  } else {
    lastMouseOverLayer = -1;
  }

  if (function == "ADDLAYER") {
    CLayer *layer = new CLayer();
    layer->init(NULL,0, 0);
    layerStack->layers.push_back(layer);
    layerStack->reBlit();
  }

  if (function == "INSERTLAYER" && currentLayer<=layerStack->layers.size()) {
    CLayer *layer = new CLayer();
    layer->init(NULL,0, 0);
    layerStack->layers.insert(layer,currentLayer);
    layerStack->reBlit();
  }

  if (function == "DUPLICATE" && currentLayer<=layerStack->layers.size()) {
    CLayer *layerOld = layerStack->layers[currentLayer];
    CLayer *layer = new CLayer();
    layer->init((uint32_t*)layerOld->image,layerOld->width, layerOld->height);
    layerStack->layers.insert(layer,currentLayer);
    layerStack->reBlit();
  }


  if (function == "DELETELAYER" && currentLayer<layerStack->layers.size()) {
    CLayer *layer = layerStack->layers[currentLayer];
    delete layer;
    layerStack->layers.erase(currentLayer,1);
    layerStack->reBlit();
  }

  if (function == "RENAMELAYER" && currentLayer<layerStack->layers.size()) {
    CLayer *layer = layerStack->layers[currentLayer];
    String name = layer->name;
    int a = VTxtAlrt::box(name,"New Layer Name:","Rename", "Don't");
    if (a == 1) {
      layer->name = name;
    }
  }

  if (currentLayer<layerStack->layers.size()) {
    if (opacity->value != opacity->lastValue) {
      layerStack->layers[currentLayer]->opacity = opacity->value;
      layerStack->reBlit();
    }
  }

  if (function == "OPACITY" && currentLayer<layerStack->layers.size()) {
    CLayer *layer = layerStack->layers[currentLayer];
    String v = String::flt(layer->opacity);
    int a = VTxtAlrt::box(v,"New Layer Opacity:","Set", "Don't");
    if (a == 1) {
      layer->opacity = v.toFloat();
      if (layer->opacity<0) layer->opacity=0;
      if (layer->opacity>1) layer->opacity=1;
      layerStack->reBlit();
    }
  }

  if (function == "ZOOM" && currentLayer<layerStack->layers.size()) {
    CLayer *layer = layerStack->layers[currentLayer];
    String v = String::flt(layer->zoom);
    int a = VTxtAlrt::box(v,"New Layer Zoom:","Set", "Don't");
    if (a == 1) {
     layer->zoom = v.toFloat();
     if (layer->zoom>8) layer->zoom=8;
     if (layer->zoom<-8) layer->zoom=-8;
     layerStack->reBlit();
    }
  }

  if (function == "RESIZE") {
    resize();
  }

  if (function == "POSITION") {
    rePosition();
  }
  return mouseIn() ? 1 : 0;
}

void CWLayer::init() {
  cMnuBar->init(
  "Tree+Add layer[ADDLAYER]+Insert layer[INSERTLAYER]+Delete layer[DELETELAYER]+Duplicate layer[DUPLICATE]\n"
  "Layer+Size[RESIZE]+Position[POSITION]+Opacity[OPACITY]+Zoom[ZOOM]+Rename[RENAMELAYER]\n"
  );
  opacity->init(0,0,1);
  scroll->init(1,0,1);
}

void CWLayer::resize() {
  CLayerStack *layerStack = data->cLayerStack;
  if (currentLayer<layerStack->layers.size()) {
    CLayer *layer = layerStack->layers[currentLayer];
    String width = String(layer->width);
    int a = VTxtAlrt::box(width,"Layer width:","Set", "Don't");
    if (a != 1) return;
    String height = String(layer->height);
    a = VTxtAlrt::box(height,"Layer height:","Set", "Don't");
    if (a != 1) return;
    int w = width.toInt();
    int h = height.toInt();
    if (w<0) w = 0;
    if (h<0) h = 0;
    layer->newSize(w,h);
    layerStack->reBlit();
  }
}

void CWLayer::rePosition() {
  CLayerStack *layerStack = data->cLayerStack;
  if (currentLayer<layerStack->layers.size()) {
    CLayer *layer = layerStack->layers[currentLayer];
    String x = String(layer->xpos);
    int a = VTxtAlrt::box(x,"Layer x:","Set", "Don't");
    if (a != 1) return;
    String y = String(layer->ypos);
    a = VTxtAlrt::box(y,"Layer y:","Set", "Don't");
    if (a != 1) return;
    layer->xpos = x.toInt();
    layer->ypos = y.toInt();
    layerStack->reBlit();
  }
}
