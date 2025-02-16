#ifndef __CHARSET_HPP__
#define __CHARSET_HPP__

typedef struct Letter {
    int letter, width, height, postAddX, postAddY, preAddX, preAddY;
    const char* bitmap;
} Letter;

Letter* getLetter(char s);

#endif //__CHARSET_HPP__