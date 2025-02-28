#ifndef __INPUT_HPP__
#define __INPUT_HPP__

extern int mouseX;
extern int mouseY;
extern int mouseB;
extern int lastMouseB;
extern int key;
extern bool shiftKey;
extern bool ctrlKey;
extern bool altKey;

void updateKeyboard();
void updateMouse();
void setupMouse();

#endif //__INPUT_HPP__