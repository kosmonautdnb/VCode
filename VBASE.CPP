#include "vbase.hpp"

VBase *keyboardFocus = NULL;

void setKeyboardFocus(VBase *window) {
  keyboardFocus = window;
}

bool hasKeyboardFocus(VBase *window) {
        return keyboardFocus == window;
}