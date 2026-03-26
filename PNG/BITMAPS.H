#pragma once

#include <vector>
#include <string>

#define MAXWIDTH 4096
#define MAXHEIGHT (4096*2)

extern unsigned int palette[256];
extern unsigned int palette2[256];
extern unsigned int paletteC64[17];
extern unsigned int pictureS[MAXWIDTH * MAXHEIGHT];
extern unsigned int pictureWriteOut[MAXWIDTH * MAXHEIGHT];
extern int pictureWidth;
extern int pictureHeight;

unsigned char getColor(unsigned int color);
unsigned char getColorC64(unsigned int color);
unsigned int getPalette(unsigned char color);
unsigned int getPaletteC64(unsigned char color);

class Plus4Bitmap {
public:
	std::string name;
	// !!!!ATTENTION!!!!! textures are currently not deleted
	int width = 0;
	int height = 0;
	std::vector<unsigned char> pixels;
	unsigned int openGLId = 0;
	void upload(bool asC64);
};

class C64Bitmap {
public:
	std::string name;
	// !!!!ATTENTION!!!!! textures are currently not deleted
	int width = 0;
	int height = 0;
	std::vector<unsigned char> pixels;
	unsigned int openGLId = 0;
	void upload();
};

void setupPalette(const std::string &paletteKichyDir, const std::string &paletteNeroDir);

Plus4Bitmap toPlus4Bitmap(bool generateTexture = false);

void LoadPNG(const char* png);
void SavePNG(const char* file_name, bool flipped = false);
void LoadBMP(char* name);
std::vector<Plus4Bitmap> LoadPSD(const char* psd, unsigned int transparentColor);
std::vector<C64Bitmap> LoadPSDC64(const char* psd, unsigned int transparentColor);
//std::vector<Plus4Bitmap> LoadTIFF(const char* tifName, unsigned int transparentColor);