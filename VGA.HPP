#ifndef __VGA_HPP__
#define __VGA_HPP__

extern unsigned int palette[256];
extern int mouseX;
extern int mouseY;
extern int mouseB;
extern int key;

void updateKeyboard();
void updateMouse();
void setBiosGraphicsMode(int mode);
void setBiosCursor(int x, int y);
void setPalette(int index, unsigned int color);
void randomPalette();
void frameBufferToScreen(unsigned int *frameBuffer);

#endif //__VGA_HPP__