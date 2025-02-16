#ifndef __SIZE_HPP__
#define __SIZE_HPP__

class Size {
public:
        int width,height;
        Size() {width = 0; height = 0; }
        Size(int _w, int _h) {width = _w; height = _h;}
};

#endif //__SIZE_HPP__