#ifndef __ARRAY_HPP__
#define __ARRAY_HPP__

#include "types.hpp"

template<class T>
class Array {
public:
        T *data;
        int usedSize;
        int allocatedSize;

        Array() {
                usedSize = 0;
                allocatedSize = 100;
                data = new T[allocatedSize];
        }

        const Array &operator=(const Array &b) {
                T *copy = new T[b.allocatedSize];
                for (int i = 0; i < b.usedSize; i++)
                        copy[i] = b.data[i];
                if (data != NULL) delete[] data;
                data = copy;
                usedSize = b.usedSize;
                allocatedSize = b.allocatedSize;
                return *this;
        }

        ~Array() {
                if (data != NULL) delete[] data;
                usedSize = 0;
                allocatedSize = 0;
        }

        void reAlloc(int numElements) {
                T *data2 = new T[numElements];
                for (int i = 0; i < numElements; i++)
                        data2[i] = i < usedSize ? data[i] : T();
                delete[] data;
                data = data2;
                allocatedSize = numElements;
        }

        void resize(int numElements) {
                if (numElements<allocatedSize) {
                        usedSize = numElements;
                } else {
                        reAlloc(numElements*1.5);
                        usedSize = numElements;
                }
        }

        void push_back(const T& elem) {
                if (usedSize < allocatedSize) {
                        data[usedSize] = elem;
                        usedSize++;
                } else {
                        reAlloc(allocatedSize*1.5);
                        data[usedSize] = elem;
                        usedSize++;
                }
        }

        void clear() {
                usedSize = 0;
        }

        int size() {
                return usedSize;
        }

        T &operator[](int index) {
                return data[index];
        }

        T& back() {
                return data[usedSize-1];
        }

        void pop_back() {
                if (usedSize > 0)
                        usedSize--;
        }

};
#endif //__ARRAY_HPP__