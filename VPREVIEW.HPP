#ifndef __VPREVIEW_HPP__
#define __VPREVIEW_HPP__

#include "vbase.hpp"
#include "vtext.hpp"
#include "vcode.hpp"
#include "array.hpp"
#include "string.hpp"
#include "colors.hpp"
#include "util.hpp"
#include "dos.hpp"

enum {
        VIEWMODE_NONE = 0,
        VIEWMODE_IMAGE = 1
};

class VPreview : public VBase {
public:
        unsigned int backgroundColor;
        unsigned int textColor;
        String fileName;
        String lastFileName;
        int viewMode;
        unsigned int *data;
        bool fullScreen;
        Size viewSize;
        VText *text;
        VCode *code;

        VPreview() : VBase() {
                backgroundColor = COLOR_MENUBACKGROUND;
                textColor = COLOR_MENUTEXT;
                viewMode = VIEWMODE_NONE;
                data = NULL;
                text = new VText();
                code = new VCode();
                fullScreen = false;
        }

        virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
        virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
        virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
        virtual void draw();
        virtual void update();

        void setFileName(const String &string, bool _fullScreen) {

                if (key == VK_F4) {
                        String extension = toLower(getExtension(string));
                        if (extension == ".cpp" ||
                                extension == ".hpp" ||
                                extension == ".c" ||
                                extension == ".h" ||
                                extension == ".s" ||
                                extension == ".asm" ||
                                extension == ".inc" ||
                                extension == ".lst" ||
                                extension == ".log" ||
                                extension == ".bak" ||
                                extension == ".txt" ||
                                extension == ".mak" ||
                                extension == ".bat") {
                                        code->init(stringFromFile(string),string);
                                        code->setRect(0,0,frameBufferWidth,frameBufferHeight);
                                        code->setMessage(fileName);
                                        code->doModal();
                                        return;
                                }
                }

                fileName = string;

                if (_fullScreen) {if (!fullScreen) {lastFileName = "";fullScreen = true;}}
                update();
        }
};

#endif //__VPREVIEW_HPP__