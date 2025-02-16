#ifndef __VSTRLST_HPP__
#define __VSTRLST_HPP__

#include "vbase.hpp"
#include "array.hpp"
#include "string.hpp"
#include "colors.hpp"
#include "istrlst.hpp"


class VStrLst : public VBase {
public:
        unsigned int backgroundColor;
        unsigned int textColor;
        int listItemCountX;
        IStrLst *items;
        Array<int> separators;
        int selectedLine;
        int firstVisibleLine;
        int lastVisibleLine;
        int scrollY;
        int scrollX;

        VStrLst() : VBase() {
                backgroundColor = COLOR_MENUBACKGROUND;
                textColor = COLOR_MENUTEXT;
                listItemCountX = 0;
                selectedLine = 0;
                firstVisibleLine = 0;
                lastVisibleLine = 0;
                scrollY = 0;
        }

        virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
        virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
        virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
        virtual void draw();
        virtual void update();

        virtual void setList(IStrLst *list, int listWidth) {
                items = list;
                listItemCountX = listWidth;
                separators.resize(listWidth+1);
                int here = 0;
                selectedLine = 0;
                scrollY = 0;
                scrollX = 0;
                list->assigned();
                for (int i = 0; i < separators.size(); i++) {
                        separators[i] = here;
                        if(i==separators.size()-1) break;
                        int width = 0;
                        int y = -1;
                        do {
                                String k = list->getString(i,y);
                                if (k == NIL) break;
                                int kWidth = k.pixelWidth();
                                if (kWidth > width) width = kWidth;
                                y++;
                        } while(1==1);
                        here += width+2;
                }
        }
};

#endif //__VSTRLST_HPP__