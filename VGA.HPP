#ifndef __VGA_HPP__
#define __VGA_HPP__

extern int paletteCount;
extern unsigned int palette[256];

void setBiosGraphicsMode(int mode);
int  getBiosGraphicsMode();
void setBiosCursor(int x, int y);
void setPalette(int index, unsigned int color);
unsigned char getPaletteIndex(unsigned int rgba);
void randomPalette();
void palette16Colors();
void setLinearFrameBuffer32Bit(unsigned int *frameBuffer);
void setBanking32Bit();
void frameBufferToScreen(unsigned int *frameBuffer);

#endif //__VGA_HPP__