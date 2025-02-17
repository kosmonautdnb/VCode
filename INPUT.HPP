#ifndef __INPUT_HPP__
#define __INPUT_HPP__

extern int mouseX;
extern int mouseY;
extern int mouseB;
extern int key;
extern bool shiftKey;
extern bool strgKey;
extern bool altKey;

void updateKeyboard();
void updateMouse();

#endif //__INPUT_HPP__